#ifndef SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H
#define SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H

#include "cmath"
#include "random"

class AcceptanceDistribution {
protected:
    static double Random(double a, double b) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(a, b);
        return dist(mt);
    }

public:
    virtual bool isAccepted(double temperature, double deltaError) = 0;

};

class OneWingDistribution : public AcceptanceDistribution {
private:
    double threshold;
public:
    explicit OneWingDistribution(double threshold) {
        if (threshold > 1.0 || threshold < 0.0) {
            throw std::logic_error("Illegal threshold for probability.");
        }
        this->threshold = threshold;
    }

    bool isAccepted(double temperature, double deltaError) override {
        if (deltaError <= 0) return true;
        return Random(0.0, 1.0) < exp(-log(threshold) * deltaError / temperature);
    }
};

class TwoWingDistribution : public AcceptanceDistribution {
private:
    double improvementThreshold;
    double deteriorationThreshold;
public:
    TwoWingDistribution(double threshold1, double threshold2) {
        if (threshold1 > 1.0 || threshold1 < 0.0 || threshold2 > 1.0 || threshold2 < 0.0) {
            throw std::logic_error("Illegal threshold for probability.");
        }
        this->improvementThreshold = threshold1;
        this->deteriorationThreshold = threshold2;
    }

    bool isAccepted(double temperature, double deltaError) override {
        double random = Random(0.0, 0.1);
        if (deltaError <= 0) {
            return random < exp(log(deteriorationThreshold) * deltaError / temperature);
        }
        return random < exp(-log(improvementThreshold) * deltaError / temperature);
    }
};

#endif //SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H
