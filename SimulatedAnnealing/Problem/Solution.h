#ifndef SA_SCHEFULING_WITH_RESOURCE_SOLUTION_H
#define SA_SCHEFULING_WITH_RESOURCE_SOLUTION_H

#include "memory"
#include "vector"

class Solution {
public:
    Solution(void);
    double Initialize(void);
    double Perturb(double temperature, double initialTemperature);
    double GetError(void);
    virtual Solution& operator=(const Solution& init) = 0;

    virtual void SetPrevious()=0;
    virtual std::vector<std::shared_ptr<Solution>> breakScope(int numScopes) = 0;
    virtual std::shared_ptr<Solution> clone() = 0;

protected:
    double error;
private:
    virtual void OnInitialize(void)=0;
    virtual void OnPerturb(double temperature, double initialTemperature)=0;
    virtual double OnComputeError(void)=0;
};


#endif //SA_SCHEFULING_WITH_RESOURCE_SOLUTION_H
