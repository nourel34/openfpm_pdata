//
// Created by landfried on 15.12.21.
//

#ifndef OPENFPM_PDATA_DOMAIN_HPP
#define OPENFPM_PDATA_DOMAIN_HPP

#include <Vector/vector_dist.hpp>

template <int dimension>
struct BoundaryConditionGenerator {
    size_t array[dimension]{};
    BoundaryConditionGenerator(size_t value) {
        for (int i = 0; i < dimension; i++) {
            array[i] = value;
        }
    }
};


template <int dimension, typename T>
Box<dimension, T> getDomain(T min, T max) {
    T domainMin[dimension];
    T domainMax[dimension];
    std::fill(std::begin(domainMin), std::end(domainMin), min);
    std::fill(std::begin(domainMax), std::end(domainMax), max);
    return Box<dimension, T>(domainMin, domainMax);
}


#endif //OPENFPM_PDATA_DOMAIN_HPP