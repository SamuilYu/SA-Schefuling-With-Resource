#ifndef SA_SCHEDULING_RESOURCES_CONDITIONS_H
#define SA_SCHEDULING_RESOURCES_CONDITIONS_H

#include "memory"

class Conditions {
public:
    virtual double estimateMaxDelta() = 0;
    virtual double estimateMax() = 0;
    virtual double estimateMin() = 0;
    virtual std::shared_ptr<Conditions> clone() = 0;
};


#endif //SA_SCHEDULING_RESOURCES_CONDITIONS_H
