// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
// Original code from project
// https://github.com/tfhe/tfhe/
// Copyright 2016 - Nicolas Gama <nicolas.gama@gmail.com> et al.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstdlib>
#include <iostream>
#include <random>
#include <cassert>
#include <limits.h>
#include "tfhe_io.h"
#include "tfhe_core.h"
#include "numeric_functions.h"

using namespace std;

default_random_engine generator;
uniform_int_distribution<Torus32> uniformTorus32_distrib(INT32_MIN, INT32_MAX);
uniform_int_distribution<int32_t> uniformInt_distrib(INT_MIN, INT_MAX);

/** sets the seed of the random number generator to the given values */
EXPORT void tfhe_random_generator_setSeed(uint32_t* values, int32_t size) {
    seed_seq seeds(values, values+size);
    generator.seed(seeds);
}

// Gaussian sample centered in message, with standard deviation sigma
EXPORT Torus32 gaussian32(Torus32 message, double sigma){
    //Attention: all the implementation will use the stdev instead of the gaussian fourier param
    normal_distribution<double> distribution(0.,sigma); //TODO: can we create a global distrib of param 1 and multiply by sigma?
    double err = distribution(generator);
    return message + dtot32(err);
}



// from double to Torus32
EXPORT Torus32 dtot32(double d) {
    return int32_t(int64_t((d - int64_t(d))*_two32));
}
// from Torus32 to double
EXPORT double t32tod(Torus32 x) {
    return double(x)/_two32_double;
}


// Used to approximate the phase to the nearest message possible in the message space
// The constant Msize will indicate on which message space we are working (how many messages possible)
//
// "travailler sur 63 bits au lieu de 64, car dans nos cas pratiques, c'est plus précis"
EXPORT Torus32 approxPhase(Torus32 phase, int32_t Msize){
    uint64_t interv = ((UINT64_C(1)<<63)/Msize)*2; // width of each intervall
    uint64_t half_interval = interv/2; // begin of the first intervall
    uint64_t phase64 = (uint64_t(phase)<<32) + half_interval;
    //floor to the nearest multiples of interv
    phase64 -= phase64%interv;
    //rescale to torus32
    return int32_t(phase64>>32);
}

// Used to approximate the phase to the nearest message possible in the message space
// The constant Msize will indicate on which message space we are working (how many messages possible)
//
// "travailler sur 63 bits au lieu de 64, car dans nos cas pratiques, c'est plus précis"
EXPORT int32_t modSwitchFromTorus32(Torus32 phase, int32_t Msize){
    uint64_t interv = ((UINT64_C(1)<<63)/Msize)*2; // width of each intervall
    uint64_t half_interval = interv/2; // begin of the first intervall
    uint64_t phase64 = (uint64_t(phase)<<32) + half_interval;
    //floor to the nearest multiples of interv
    return phase64/interv;
}

// Used to approximate the phase to the nearest message possible in the message space
// The constant Msize will indicate on which message space we are working (how many messages possible)
//
// "travailler sur 63 bits au lieu de 64, car dans nos cas pratiques, c'est plus précis"
EXPORT Torus32 modSwitchToTorus32(int32_t mu, int32_t Msize){
    uint64_t interv = ((UINT64_C(1)<<63)/Msize)*2; // width of each intervall
    uint64_t phase64 = mu*interv;
    //floor to the nearest multiples of interv
    return phase64>>32;
}
