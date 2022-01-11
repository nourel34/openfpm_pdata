//
// Created by landfried on 05.01.22.
//

#ifndef OPENFPM_PDATA_INITIALCONDITION_HPP
#define OPENFPM_PDATA_INITIALCONDITION_HPP

#include <iostream>


struct InitialConditionMesh {};
struct InitialConditionRandom {};


// primary template
template <typename IC, typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {
        std::cout << "primary template" << std::endl;
    }
};

template <typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl<InitialConditionMesh, ParticleMethodType, SimulationParametersType> {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {
        std::cout << "Mesh particle placement" << std::endl;

        // create particle mesh
        particleData.vd.clear();
        auto meshIterator = particleData.vd.getGridIterator(SimulationParametersType::meshSize);
        while (meshIterator.isNext())
        {
            particleData.vd.add();
            auto node = meshIterator.get();
            for (int i = 0; i < ParticleMethodType::spaceDimension; i++) {
                particleData.vd.getLastPos()[i] = node.get(i) * meshIterator.getSpacing(i);
            }
            ++meshIterator;
        }
    }
};

template <typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl<InitialConditionRandom, ParticleMethodType, SimulationParametersType> {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {
        int dimension = ParticleMethodType::spaceDimension;
        SimulationParametersType simulationParameters;


        std::cout << "Random particle placement" << std::endl;

        // RNG
        std::random_device rd;  // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

        std::vector<std::uniform_real_distribution<>> dis_pos_v;
        for (int i = 0; i < dimension; ++i) {
            dis_pos_v.push_back(std::uniform_real_distribution<>(simulationParameters.domainMin[i], simulationParameters.domainMax[i]));
//            dis_pos[i](simulationParameters.domainMin.get(i), simulationParameters.domainMax.get(i));
        }
        std::normal_distribution<> dis_vel(0, .5);

        // move particles to random positions
        auto iterator = particleData.vd.getDomainIterator();
        while (iterator.isNext())
        {
            auto p = iterator.get();
            for (int i = 0; i < ParticleMethodType::spaceDimension; i++) {
                // random positions
                particleData.vd.getPos(p)[i] = dis_pos_v[i](gen);
//                std::cout << "pos " << i << ": " << particleData.vd.getPos(p)[i] << std::endl;

                particleData.vd.template getProp<0>(p)[i] = dis_vel(gen);
//                std::cout << "vel " << i << ": " << particleData.vd.template getProp<0>(p)[i] << std::endl;

            }
            ++iterator;
        }
    }
};

#endif //OPENFPM_PDATA_INITIALCONDITION_HPP