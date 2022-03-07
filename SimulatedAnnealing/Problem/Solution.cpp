#include <cmath>
#include "./Solution.h"

Solution::Solution() {
    error = 1.0;
}

double Solution::GetError() {
    return error;
}

double Solution::Initialize() {
    OnInitialize();
    error = fabs(OnComputeError());
    return error;
}

double Solution::Perturb(double temperature, double initialTemperature) {
    OnPerturb(temperature, initialTemperature);
    error = fabs(OnComputeError());
    return error;
}