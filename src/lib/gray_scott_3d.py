import numpy as np
from numba import stencil, njit

from pathlib import Path

from conduit import Node
from installer import install_openfpm
__OFPM__ROOT__ = str(Path().home() / 'opt/openfpm')
install_openfpm(
    openfpm_install_dir=__OFPM__ROOT__ + '/install',
    openfpm_dep_dir=__OFPM__ROOT__ + '/deps'
)
import openfpm

from minimon import MiniMon
minimon = MiniMon()

from collections import namedtuple
from dataclasses import dataclass
from copy import deepcopy

import pyvista as pv
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

Point = namedtuple('Point', 'coords')
Box = namedtuple('Box', 'low high')


@dataclass
class Domain:
    U: np.ndarray
    V: np.ndarray
    spacing: np.ndarray
    size: np.ndarray


DIM = 3
PERIODIC = 1
x, y, z = 0, 1, 2
K, F = 0.053, 0.014  # K and F (Physical constant in the equation)

np.random.seed(42)


def convert_between_ranges(x, domain_x, domain_y):
    ratio = (x - domain_x[0]) / (domain_x[1] - domain_x[0])
    return domain_y[0] + ratio * (domain_y[1] - domain_y[0])


def domain_point2array_index(point, domain, grid_size, to_int=False):
    """ point ~ (1 x) D with coordinates of point in domain space
        domain ~ Domain (with .low and .high)
        grid_size ~ (1 x) D with size of grid in every dimension

        return ~ (1 x) D of type float
    """

    f = lambda d: convert_between_ranges(
        point[d], [domain.low.coords[d], domain.high.coords[d]], [0, grid_size[d]]
    )
    dims = len(point)
    raw = map(f, range(dims))

    if to_int:
        raw = map(np.around, raw)
        return np.int64(list(raw))

    return np.float64(list(raw))


def init_domain(old, new, whole_domain):
    old.U = np.ones_like(old.U)
    old.V = np.ones_like(old.V)

    new.U = np.zeros_like(new.U)
    new.V = np.zeros_like(new.V)

    x_start = old.size[0] * 1.55 / whole_domain.high.coords[0]
    y_start = old.size[1] * 1.55 / whole_domain.high.coords[1]
    z_start = old.size[1] * 1.55 / whole_domain.high.coords[2]

    x_stop = old.size[0] * 1.85 / whole_domain.high.coords[0]
    y_stop = old.size[1] * 1.85 / whole_domain.high.coords[1]
    z_stop = old.size[1] * 1.85 / whole_domain.high.coords[2]

    start = [x_start, y_start, z_start]
    stop = [x_stop, y_stop, z_stop]

    grid_size = [old.U.shape[0], ] * 3  # assuming square grid
    start_i, start_j, start_k = domain_point2array_index(
        start, whole_domain, grid_size, to_int=True
    )
    stop_i, stop_j, stop_k = domain_point2array_index(
        stop, whole_domain, grid_size, to_int=True
    )

    old.U[start_i:stop_i, start_j:stop_j, start_k:stop_k] =\
        0.5 + (np.random.rand(*old.U[start_i:stop_i, start_j:stop_j, start_k:stop_k].shape) - 0.5) / 10.0
    old.V[start_i:stop_i, start_j:stop_j, start_k:stop_k] =\
        0.25 + (np.random.rand(*old.V[start_i:stop_i, start_j:stop_j, start_k:stop_k].shape) - 0.5) / 20.0


def get_stencils(uFactor, vFactor, format='numpy'):
    if format == 'numpy':
        return None, None
    elif format == 'numba':
        @njit(fastmath=True)  # parallel=True
        def stencil_U(U, V, dT, uFactor):
            return stencil(
                lambda U, V, dT, uFactor, F: U[0, 0, 0] +\
                    uFactor * (
                        U[-1, 0, 0] + U[-1, 0, 0] + U[0, -1, 0] + U[0, +1, 0] + U[0, 0, -1] + U[0, 0, +1] - 6 * U[0, 0, 0]
                    ) -\
                    dT * U[0, 0, 0] * V[0, 0, 0] * V[0, 0, 0] -\
                    dT * F * (U[0, 0, 0] - 1.0)  # update based on Eq 2
            )(U, V, dT, uFactor, F)

        @njit(fastmath=True)
        def stencil_V(U, V, dT, vFactor):
            return stencil(
                lambda U, V, dT, vFactor, F, K: V[0, 0, 0] +\
                    vFactor * (
                        V[-1, 0, 0] + V[-1, 0, 0] + V[0, -1, 0] + V[0, +1, 0] + V[0, 0, -1] + V[0, 0, +1] - 6 * V[0, 0, 0]
                    ) -\
                    dT * U[0, 0, 0] * V[0, 0, 0] * V[0, 0, 0] -\
                    dT * (F + K) * V[0, 0, 0]
            )(U, V, dT, vFactor, F, K)  # update based on Eq 2

        return stencil_U, stencil_V


def add_ghosts(real, gh=1):
    ghosted = np.zeros(
        (real.shape[0] + 2 * gh, real.shape[1] + 2 * gh, real.shape[2] + 2 * gh)
    )
    ghosted[gh:-gh, gh:-gh, gh:-gh] = real.copy()

    ghosted[0, gh:-gh, gh:-gh] = real[0, :, :].copy()
    ghosted[-1, gh:-gh, gh:-gh] = real[-1, :, :].copy()

    ghosted[gh:-gh, 0, gh:-gh] = real[:, 0, :].copy()
    ghosted[gh:-gh, -1, gh:-gh] = real[:, -1, :].copy()

    ghosted[gh:-gh, gh:-gh, 0] = real[:, :, 0].copy()
    ghosted[gh:-gh, gh:-gh, -1] = real[:, :, -1].copy()

    return ghosted

def make_loop(timeSteps, uFactor, vFactor, F, debug_every=300, save_every=500):
    should_debug = lambda timeStep: timeStep % debug_every == 0
    should_save = lambda timeStep: timeStep % save_every == 0
    output_filename = 'output'  # much fantasy
    ghost = 1  # todo as param

    def _f(stencils, old_copy, new_copy, dT):
        kernel_U, kernel_V = stencils  # unpack

        for i in range(timeSteps):
            minimon.enter()
            if should_debug(i):
                print('STEP: {:d}'.format(i))

            oldU, oldV = add_ghosts(old_copy.U), add_ghosts(old_copy.V)

            new_copy.U = kernel_U(oldU, oldV, dT, uFactor)[ghost:-ghost, ghost:-ghost, ghost:-ghost]  # numba replaces ghosts with 0 => discard them all!
            new_copy.V = kernel_V(oldU, oldV, dT, vFactor)[ghost:-ghost, ghost:-ghost, ghost:-ghost]

            old_copy.U = new_copy.U  # sync
            old_copy.V = new_copy.V

            # todo After copy we synchronize again the ghost part U and V

            minimon.leave('dT')

            if should_save(i):
                # openfpm.save_grid(0, output_filename)
                print('    saved to {}'.format(output_filename))

    return _f


def create_grid_dist(size, domain, gh, periodicity=[0, ] * 3):
    grid_node = Node()
    grid_node['dim'] = DIM
    grid_node['n props'] = 2
    grid_node['gh'] = gh
    grid_node['size'] = size
    grid_node['periodicity'] = periodicity
    grid_node['domain/low'] = domain.low.coords
    grid_node['domain/high'] = domain.high.coords

    minimon.enter()
    openfpm.create_grid(grid_node)
    minimon.leave('openfpm.create_grid')

    return Domain(
        U=np.zeros(size),
        V=np.zeros(size),
        spacing=np.float64(grid_node['spacing']),
        size=size
    )


def plot3d(domain, subsampling=[20, 20, 20]):
    plt.rcParams["figure.autolayout"] = True
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    stuff = domain[::subsampling[0], ::subsampling[1], ::subsampling[2]]
    zcoords, xcoords, ycoords = stuff.nonzero()
    viz = stuff[stuff != 0]
    ax.scatter(
        xcoords, ycoords, zcoords,
        c=viz.ravel(),
        cmap='viridis', vmin=viz.min(), vmax=viz.max()
    )
    # debug only print(stuff)
    plt.show()


def main():
    """ https://git.mpi-cbg.de/openfpm/openfpm_pdata/-/blob/master/example/Grid/3_gray_scott_3d/main.cpp """

    whole_domain = Box(
        low=Point(np.float64([0, 0, 0])),
        high=Point(np.float64([5.0, 5.0, 5.0]))
    )
    grid_size = np.int64([128, ] * 3)
    periodicity = np.int64([PERIODIC, ] * 3)
    g = 1  # Ghost in grid unit
    deltaT, du, dv = 1, 2e-5, 1e-5
    timeSteps = 100  # todo debug 5000

    old_copy = create_grid_dist(
        grid_size,
        whole_domain,
        g,
        periodicity=periodicity
    )
    new_copy = deepcopy(old_copy)
    spacing = deepcopy(old_copy.spacing)

    init_domain(old_copy, new_copy, whole_domain)

    uFactor = deltaT * du / (spacing[x] * spacing[x])
    vFactor = deltaT * dv / (spacing[x] * spacing[x])
    stencils = get_stencils(uFactor, vFactor, format='numba')
    loop = make_loop(timeSteps, uFactor, vFactor, F, debug_every=1, save_every=2)

    minimon.enter()
    loop(stencils, old_copy, new_copy, deltaT)
    minimon.leave('tot_sim')
    minimon.print_stats()

    plot3d(old_copy.U)


if __name__ == '__main__':
    openfpm.openfpm_init()

    main()

    openfpm.openfpm_finalize()
