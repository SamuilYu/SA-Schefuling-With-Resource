#ifndef SA_SCHEDULING_RESOURCES_CONDITIONS_H
#define SA_SCHEDULING_RESOURCES_CONDITIONS_H


class Conditions {
public:
    virtual double estimateMax() = 0;
    virtual double estimateMin() = 0;
};


#endif //SA_SCHEDULING_RESOURCES_CONDITIONS_H
