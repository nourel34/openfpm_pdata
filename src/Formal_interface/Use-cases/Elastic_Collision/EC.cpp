//
// Created by landfried on 27.09.22.
//

#include "EC_Algorithm.hpp"
#include "EC_Instance.hpp"


int main(int argc, char* argv[]) {

    openfpm_init(&argc,&argv);

    // Particle container
    ParticleData<DEM_ParticleMethod, PEC_SimulationParams> particleData;

    // State transition
    Transition<DEM_ParticleMethod, PEC_SimulationParams, PEC_Instance> transition(particleData);

    // Main loop
    while (!transition.stop(particleData)) {

        // Execute simulation step
        transition.run_step(particleData);
    }

    openfpm_finalize();

    return 0;
}
