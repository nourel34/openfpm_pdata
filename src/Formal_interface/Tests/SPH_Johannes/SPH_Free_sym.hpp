//
// Created by landfried on 02.03.22.
//
// source /home/peter/openfpm_vars
// make run 

#ifndef OPENFPM_PDATA_PSE_FREE_UNSYM_HPP
#define OPENFPM_PDATA_PSE_FREE_UNSYM_HPP


#include "Vector/vector_dist.hpp"
#include "Formal_interface/Particle.hpp"
#include "Formal_interface/ParticleData.hpp"
#include "Formal_interface/ParticleMethod.hpp"
#include "Formal_interface/Transition.hpp"
#include "Formal_interface/SimulationParameters.hpp"
#include "Formal_interface/InitialCondition.hpp"
#include "Formal_interface/Neighborhood.hpp"
#include "Formal_interface/Instance.hpp"




constexpr int DIMENSION = 3;
typedef double POSITIONTYPE;

struct SPH_ParticleSignature {
    static constexpr int dimension = DIMENSION;
    typedef POSITIONTYPE position;
    typedef aggregate<double[dimension], double[dimension], double[dimension], double, double, double[dimension],double, bool> properties;
    typedef FREE_PARTICLES dataStructure;
};

// Property identifier
constexpr int positionOld = 0;
constexpr int velocity = 1;
constexpr int velocityOld = 2;
constexpr int density = 3;
constexpr int densityOld = 4;
constexpr int deltaVelocity = 5;
constexpr int deltaDensity = 6;
constexpr int boundary = 7;

struct GlobalVariable {

    double t=0;
    double dt=0.00004;
    double endT= 2.0;

    double particleSpacing=1.0/64.0;
//    double particleSpacing=1.0/64.0;
    double particleSpacingWater=particleSpacing;
    double mass=pow(particleSpacing,3)*1000;
    Point<DIMENSION, POSITIONTYPE> gravity{0.0,0.0,-9.81};
    double c0=45.0;
    double density0=1000;
    int gamma=7;
    double nu=1.0/10000.0;
    double h=1.3*particleSpacing;//characteristic length

    double phase=0;
    int support=2;
    double rc=support*h;//cutof radius
    double epsilon=0.01;



    // Point<DIMENSION, POSITIONTYPE> domain_min;
    // Point<DIMENSION, POSITIONTYPE> domain_max;
    // Point<DIMENSION, POSITIONTYPE> meshDim;
//    double domain_min = -1;
//    double domain_max = 4;

    double domain_min[3] = {-.9, -.9, -.9}; // 0.5
    double domain_max[3] = {3.3, 1.3, 1.3};  //3.5, 2, 3

    // calculate number of particles in each dimension
    size_t sz[3] = {uint((domain_max[0] - domain_min[0]) / particleSpacing),
                    uint((domain_max[1] - domain_min[1]) / particleSpacing),
                    uint((domain_max[2] - domain_min[2]) / particleSpacing)};

} g;


template <typename ParticleSignature>
class SPH_ParticleMethod : public ParticleMethod<ParticleSignature> {
    static constexpr int dimension = ParticleSignature::dimension;
    using PositionType = typename ParticleSignature::position;


    double pressure_density2(double density){
        return g.c0*g.c0*g.density0/g.gamma * (pow(density/g.density0, g.gamma)-1)/density/density;
    }
public:


    void interact(Particle<ParticleSignature> particle, Particle<ParticleSignature> neighbor) override {

        Point<dimension, PositionType> n_pos = neighbor.position();
        Point<dimension, PositionType> p_pos = particle.position();
        Point<dimension, PositionType> r_pq = n_pos - p_pos;
        Point<dimension, PositionType> v_pq =  neighbor.template property_vec<velocity>()
                                               -particle.template property_vec<velocity>();
//        double dist2_pq = r_pq.abs2();
        double dist2_pq = p_pos.distance2(n_pos);
        double dist_pq = sqrt(dist2_pq);

        if (dist_pq > g.rc)
            return;

        double f_pq=  pow(1.0-dist_pq/2.0/g.h , 3);
        double vr = 0;
        for (int i = 0; i < DIMENSION; i++)
            vr += v_pq[i] * r_pq[i];


//        double vr=v_pq*r_pq;

        double p_pressure_density2=pressure_density2(particle.template property_vec<density>());
        double q_pressure_density2=pressure_density2(neighbor.template property_vec<density>());
        double interim01= p_pressure_density2+q_pressure_density2 ;
        double interim02 = 10*g.nu/dist2_pq * vr;

        particle.template property_vec<deltaVelocity>() += (interim01-interim02/particle.template property_vec<density>())* r_pq * f_pq;
        neighbor.template property_vec<deltaVelocity>() -= (interim01-interim02/neighbor.template property_vec<density>())* r_pq * f_pq;



double densityChange= vr * f_pq;

particle.template property_vec<deltaDensity>() += densityChange;
neighbor.template property_vec<deltaDensity>() += densityChange;


}

void evolve(Particle<ParticleSignature> particle) override {
double prefact = g.mass* -5.0*21.0/16.0/M_PI/pow(g.h,5);

Point<dimension, PositionType> acceleration = g.gravity + particle.template property_vec<deltaVelocity>()*prefact;

double densityacceleration = particle.template property_vec<deltaDensity>()*prefact;

        if (g.phase==0){
            if (particle.template property_vec<boundary>()==false){
                // fluid

                // move particles half step
                particle.template property_vec<positionOld>()=particle.position_vec();
                particle.position_vec() += g.dt/2.0f*particle.template property_vec<velocity>();

                // change velocity half step
                particle.template property_vec<velocityOld>()=particle.template property_vec<velocity>();
                particle.template property_vec<velocity>()+= g.dt/2.0*acceleration;
            }

            // fluid + boundary

            // change density
            particle.template property_vec<densityOld>()=particle.template property_vec<density>();
            particle.template property_vec<density>()+=g.dt/2.0*densityacceleration;
        }
        else{
            if (particle.template property_vec<boundary>()==false){
                // fluid

                // move particle from original position
                Point<dimension, PositionType> step_acc_half = g.dt  / 2.0 * acceleration;
                Point<dimension, PositionType> step_vel = g.dt * (particle.template property_vec<velocityOld>() + step_acc_half);
                Point<dimension, PositionType> new_pos = particle.template property_vec<positionOld>() + step_vel;
                particle.position_vec() = new_pos;

                // change velocity
                Point<dimension, PositionType> step_acc_full = g.dt * acceleration;
                particle.template property_vec<velocity>() = particle.template property_vec<velocityOld>() + step_acc_full;


//                particle.position_vec() = particle.template property_vec<positionOld>() + g.dt * (particle.template property_vec<velocityOld>()  +  (g.dt  / 2.0) * acceleration);
//                particle.template property_vec<velocity>() = particle.template property_vec<velocityOld>() +g.dt*acceleration;


            }

            // fluid + boundary

            // change density
            particle.template property_vec<density>()= particle.template property_vec<densityOld>() + g.dt*densityacceleration;
        }

        //set to 0 to have a fresh accumulators for the next time step
        particle.template property_vec<deltaVelocity>()=Point<dimension, PositionType> (0.0);
        particle.template property_vec<deltaDensity>()=0.0;
    }

    void evolveGlobalVariable() override {

        if (g.phase==0){
            g.phase=1;
        }
        else{
            g.t+=g.dt;//advance the current time by the time step
            g.phase=0;
        }

        std::cout << "\r" << "t = " << g.t << " (" << int(g.t / g.endT * 100) << "%) " << std::flush;

    }

    bool stop() override {

        // Check simulation time
        if (g.t > g.endT)
            return true;

        return false;
    }
};

template <typename ParticleSignatureType>
class SPH_SimulationParams : public SimulationParameters<ParticleSignatureType> {
    static constexpr int dimension = ParticleSignatureType::dimension;
    using PositionType = typename ParticleSignatureType::position;
    using PropertyType = typename ParticleSignatureType::properties;

public:

    SPH_SimulationParams() {

        // Domain
//        this->domainMin[0] = {-1.0, -1.0, -1.0};
//        this->domainMax = {4.0, 4.0, 4.0};
//        this->setDomain(g.domain_min,g.domain_max);
        this->domainMin[0] = g.domain_min[0];
        this->domainMin[1] = g.domain_min[1];
        this->domainMin[2] = g.domain_min[2];
        this->domainMax[0] = g.domain_max[0];
        this->domainMax[1] = g.domain_max[1];
        this->domainMax[2] = g.domain_max[2];

        this->setBoundaryConditions(PERIODIC);
        this->setCutoffRadius(g.rc);
        // this->setMeshSize(g.meshDim);
        this->setCellWidth(g.rc);
    }

    // Mesh initial condition
//    typedef INITIALCONDITION_MESH initialCondition;

    // Neighborhood method
    typedef NEIGHBORHHOD_CELLLIST neighborhoodDetermination;

    static const int interactionType = INTERACTION_SYMMETRIC;

    // Output
    bool writeOutput = true;

//     void initialization(Particle<ParticleSignatureType> particle) override {

// 	// std::cout << "ID " << particle.getID() << std::endl;

// // //        std::cout << "r_cut " << globalvar.r_cut << std::endl;
// //         for (int i = 0; i < dimension; i++) {
// //             // Randomize concentration (normal distribution)
// //             particle.template property<concentration>() = this->normalDistribution(0, 5);         }
//     }S



};


class Instance1 : Instance<SPH_ParticleMethod<SPH_ParticleSignature>, SPH_SimulationParams<SPH_ParticleSignature>> {

    static constexpr int dimension = SPH_ParticleSignature::dimension;
    using PositionType = typename SPH_ParticleSignature::position;


public:

    Instance1(ParticleData<SPH_ParticleMethod<SPH_ParticleSignature>, SPH_SimulationParams<SPH_ParticleSignature>> &particleData_in) :
            Instance<SPH_ParticleMethod<SPH_ParticleSignature>, SPH_SimulationParams<SPH_ParticleSignature>>(particleData_in){}


    virtual void shapePlacement() {

        // fluid particles
        Point<dimension, PositionType> waterblockMin{0.15,0.1,0.1};
        Point<dimension, PositionType> waterblockMax{0.55,0.9,0.9};

        auto iterator_fluid = boxIterator(waterblockMin, waterblockMax, g.sz);

        while (iterator_fluid.isNext()) {

            this->addParticle();

            this->position()[0] = iterator_fluid.get().get(0);
            this->position()[1] = iterator_fluid.get().get(1);
            this->position()[2] = iterator_fluid.get().get(2);

            this->property<boundary>() = false;
            this->property<velocity>()[0] = 0.0;
            this->property<velocity>()[1] = 0.0;
            this->property<velocity>()[2] = 0.0;
            this->property<density>() = 1000.0;
            this->property<deltaVelocity>()[0] = 0.0;
            this->property<deltaVelocity>()[1] = 0.0;
            this->property<deltaVelocity>()[2] = 0.0;
            this->property<deltaDensity>() = 0.0;

            ++iterator_fluid;

        }


        // obstacle column

        Point<dimension, PositionType> columnMin{2.0, 3.0/8.0 ,0.0};
        Point<dimension, PositionType> columnMax{2.25,5.0/8.0,1.0};

        auto iterator_column = skinIterator(columnMin, columnMax, g.particleSpacing, g.sz);

        while (iterator_column.isNext()) {

            this->addParticle();

            this->position()[0] = iterator_column.get().get(0);
            this->position()[1] = iterator_column.get().get(1);
            this->position()[2] = iterator_column.get().get(2);

            this->property<boundary>() = true;
            this->property<velocity>()[0] = 0.0;
            this->property<velocity>()[1] = 0.0;
            this->property<velocity>()[2] = 0.0;
            this->property<density>() = 1000.0;
            this->property<deltaVelocity>()[0] = 0.0;
            this->property<deltaVelocity>()[1] = 0.0;
            this->property<deltaVelocity>()[2] = 0.0;
            this->property<deltaDensity>() = 0.0;

            ++iterator_column;

        }


        // pool walls

        Point<dimension, PositionType> poolMin{0.0,0.0,0.0};
        Point<dimension, PositionType> poolMax{3.0,1.0,1.0};

        auto iterator_pool = skinOpenIterator(poolMin, poolMax, g.particleSpacing, g.sz);

        while (iterator_pool.isNext()) {

            this->addParticle();

            this->position()[0] = iterator_pool.get().get(0);
            this->position()[1] = iterator_pool.get().get(1);
            this->position()[2] = iterator_pool.get().get(2);

            this->property<boundary>() = true;
            this->property<velocity>()[0] = 0.0;
            this->property<velocity>()[1] = 0.0;
            this->property<velocity>()[2] = 0.0;
            this->property<density>() = 1000.0;
            this->property<deltaVelocity>()[0] = 0.0;
            this->property<deltaVelocity>()[1] = 0.0;
            this->property<deltaVelocity>()[2] = 0.0;
            this->property<deltaDensity>() = 0.0;

            ++iterator_pool;

        }


    }

    virtual void freePlacement() {


        // double particleSpacing=1.0/64.0;
        // double particleSpacingWater=particleSpacing;

        //     GlobalVariable g;
        //     g.endT=20;
        //     g.t=0.0;
        //     g.dt=0.00005;

        //     g.mass=power(particleSpacing,3)*1000;
        //     g.h=1.3*particleSpacing;

        //     g.epsilon=0.01;
        //     g.gamma=7;
        //     g.gravity=Point<dimension, PositionType>(0.0,0.0,-9.81);
        //     g.density0=1000;
        //     g.c0=45.0;
        //     g.nu=1.0/10000.0;
        //     g.phase=0;

        //     int support=2;
        //     g.rc=support*g.h;


        //these should be a multible of the particleSpacing
        Point<dimension, PositionType> poolMin{0.0,0.0,0.0};
        Point<dimension, PositionType> poolMax{3.0,1.0,1.0};
        Point<dimension, PositionType> columnMin{2.0, 3.0/8.0 ,0.0};
        Point<dimension, PositionType> columnMax{2.25,5.0/8.0,1.0};
        Point<dimension, PositionType> waterblockMin{0.0,0.0,0.0};
        Point<dimension, PositionType> waterblockMax{0.5,1.0,1.0};
        // waterblockMin+=Point<dimension, PositionType>(g.particleSpacing);
        // waterblockMax-=Point<dimension, PositionType>(g.particleSpacing);

        // g.domain_min=poolMin-Point<dimension, PositionType>(g.particleSpacing*g.support);
        // g.domain_max=Point<dimension, PositionType>{poolMax[0]+g.particleSpacing*g.support,
        //                                 poolMax[1]+g.particleSpacing*g.support,
        //                                 poolMax[2]
        // };



/*

        // fluid
        this->addParticle();
        this->position()[0] = 0.0;
        this->position()[1] = 0.0;
        this->position()[2] = 1.0;
        this->property<boundary>() = false;
        this->property<velocity>()[0] = 0.0;
        this->property<velocity>()[1] = 0.0;
        this->property<velocity>()[2] = 0.0;
        this->property<density>() = 1000.0;
        this->property<deltaVelocity>()[0] = 0.0;
        this->property<deltaVelocity>()[1] = 0.0;
        this->property<deltaVelocity>()[2] = 0.0;
        this->property<deltaDensity>() = 0.0;

        this->addParticle();
        this->position()[0] = 0.0;
        this->position()[1] = 0.0;
        this->position()[2] = 2.0;
        this->property<boundary>() = false;
        this->property<velocity>()[0] = 0.0;
        this->property<velocity>()[1] = 0.0;
        this->property<velocity>()[2] = 0.0;
        this->property<density>() = 1000.0;
        this->property<deltaVelocity>()[0] = 0.0;
        this->property<deltaVelocity>()[1] = 0.0;
        this->property<deltaVelocity>()[2] = 0.0;
        this->property<deltaDensity>() = 0.0;

        // wall
        this->addParticle();
        this->position()[0] = 0.0;
        this->position()[1] = 0.0;
        this->position()[2] = 0.0;
        this->property<boundary>() = true;
        this->property<velocity>()[0] = 0.0;
        this->property<velocity>()[1] = 0.0;
        this->property<velocity>()[2] = 0.0;
        this->property<density>() = 1000.0;
        this->property<deltaVelocity>()[0] = 0.0;
        this->property<deltaVelocity>()[1] = 0.0;
        this->property<deltaVelocity>()[2] = 0.0;
        this->property<deltaDensity>() = 0.0;
*/




        Point<dimension, PositionType> wallMin;
        Point<dimension, PositionType> wallMax;

        double d= g.particleSpacingWater/2.0;

/*

// fluid block
        wallMin[0]=waterblockMin[0]+g.particleSpacing;
        wallMin[1]=waterblockMin[1]+g.particleSpacing;
        wallMin[2]=waterblockMin[2]+g.particleSpacing;
        wallMax[0]=waterblockMax[0]-g.particleSpacing;
        wallMax[1]=waterblockMax[1]-g.particleSpacing;
        wallMax[2]=waterblockMax[2]-g.particleSpacing;
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = false;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

*/





/*

        //Pool Boundary
        d=g.particleSpacing/2.0;

        //Botom Pool Boundary 
        wallMin[0]=poolMin[0]-g.particleSpacing*g.support;
        wallMin[1]=poolMin[1]-g.particleSpacing*g.support;
        wallMin[2]=poolMin[2]-g.particleSpacing*g.support;
        wallMax[0]=poolMax[0]+g.particleSpacing*g.support;
        wallMax[1]=poolMax[1]+g.particleSpacing*g.support;
        wallMax[2]=poolMin[2]-g.particleSpacing;
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Left Pool Boundary 
        wallMin[0]=poolMin[0]-g.particleSpacing*g.support;
        wallMin[1]=poolMin[1]-g.particleSpacing*g.support;
        wallMin[2]=poolMin[2];
        wallMax[0]=poolMin[0]-g.particleSpacing;
        wallMax[1]=poolMax[1]+g.particleSpacing*g.support;
        wallMax[2]=poolMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Right Pool Boundary 
        wallMin[0]=poolMax[0]+g.particleSpacing;
        wallMin[1]=poolMin[1]-g.particleSpacing*g.support;
        wallMin[2]=poolMin[2];
        wallMax[0]=poolMax[0]+g.particleSpacing*g.support;
        wallMax[1]=poolMax[1]+g.particleSpacing*g.support;
        wallMax[2]=poolMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Front Pool Boundary 
        wallMin[0]=poolMin[0];
        wallMin[1]=poolMin[1]-g.particleSpacing*g.support;
        wallMin[2]=poolMin[2];
        wallMax[0]=poolMax[0];
        wallMax[1]=poolMin[1]-g.particleSpacing;
        wallMax[2]=poolMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Back Pool Boundary 
        wallMin[0]=poolMin[0];
        wallMin[1]=poolMax[1]+g.particleSpacing;
        wallMin[2]=poolMin[2];
        wallMax[0]=poolMax[0];
        wallMax[1]=poolMax[1]+g.particleSpacing*g.support;
        wallMax[2]=poolMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }


*/








/*        //Top Column Boundary
        wallMin[0]=columnMin[0]-g.particleSpacing*g.support;
        wallMin[1]=columnMin[1]-g.particleSpacing*g.support;
        wallMin[2]=columnMax[2]+g.particleSpacing;
        wallMax[0]=columnMax[0]+g.particleSpacing*g.support;
        wallMax[1]=columnMax[1]+g.particleSpacing*g.support;
        wallMax[2]=columnMax[2]+g.particleSpacing*g.support;
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Left Column Boundary 
        wallMin[0]=columnMin[0]-g.particleSpacing*g.support;
        wallMin[1]=columnMin[1]-g.particleSpacing*g.support;
        wallMin[2]=columnMin[2];
        wallMax[0]=columnMin[0]-g.particleSpacing;
        wallMax[1]=columnMax[1]+g.particleSpacing*g.support;
        wallMax[2]=columnMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Right Column Boundary 
        wallMin[0]=columnMax[0]+g.particleSpacing;
        wallMin[1]=columnMin[1]-g.particleSpacing*g.support;
        wallMin[2]=columnMin[2];
        wallMax[0]=columnMax[0]+g.particleSpacing*g.support;
        wallMax[1]=columnMax[1]+g.particleSpacing*g.support;
        wallMax[2]=columnMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Front Column Boundary 
        wallMin[0]=columnMin[0];
        wallMin[1]=columnMin[1]-g.particleSpacing*g.support;
        wallMin[2]=columnMin[2];
        wallMax[0]=columnMax[0];
        wallMax[1]=columnMin[1]-g.particleSpacing;
        wallMax[2]=columnMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }

        //Back Column Boundary 
        wallMin[0]=columnMin[0];
        wallMin[1]=columnMax[1]+g.particleSpacing;
        wallMin[2]=columnMin[2];
        wallMax[0]=columnMax[0];
        wallMax[1]=columnMax[1]+g.particleSpacing*g.support;
        wallMax[2]=columnMax[2];
        for (double x = wallMin[0]; x <= wallMax[0]+d; x+=g.particleSpacing) {
            for (double y = wallMin[1]; y <= wallMax[1]+d; y+=g.particleSpacing) {
                for (double z = wallMin[2]; z <= wallMax[2]+d; z+=g.particleSpacing) {
                    this->addParticle();
                    this->position()[0] = x;
                    this->position()[1] = y;
                    this->position()[2] = z;
                    this->property<boundary>() = true;
                    this->property<velocity>()[0] = 0.0;
                    this->property<velocity>()[1] = 0.0;
                    this->property<velocity>()[2] = 0.0;
                    this->property<density>() = 1000.0;
                    this->property<deltaVelocity>()[0] = 0.0;
                    this->property<deltaVelocity>()[1] = 0.0;
                    this->property<deltaVelocity>()[2] = 0.0;
                    this->property<deltaDensity>() = 0.0;
                }
            }
        }*/
    }
};



#endif //OPENFPM_PDATA_PSE_FREE_UNSYM_HPP
