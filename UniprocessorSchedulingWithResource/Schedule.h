#ifndef SA_SCHEDULING_RESOURCES_SCHEDULE_H
#define SA_SCHEDULING_RESOURCES_SCHEDULE_H

#include "ScopedSchedule.h"

class Schedule: public ScopedSchedule {
    Schedule(const SchedulingConditions& conditions): ScopedSchedule(conditions, conditions) {}
};


#endif //SA_SCHEDULING_RESOURCES_SCHEDULE_H
