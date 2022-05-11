#ifndef SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H
#define SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H

#include "cmath"
#include "random"
#include "stdexcept"

class AcceptanceDistribution {
protected:
    std::mt19937 mt;
    double Random(double a, double b) {

        std::uniform_real_distribution<double> dist(a, b);
        return dist(mt);
    }

public:
    AcceptanceDistribution() {
        std::random_device rd;
        mt = std::mt19937(rd());
    }
    virtual double getProbability(double temperature, double deltaError)=0;
    virtual bool isAccepted(double temperature, double deltaError) {
        return Random(0.0, 1.0) <= this -> getProbability(temperature, deltaError);
    };
    virtual std::shared_ptr<AcceptanceDistribution> clone() = 0;

};

class MetropolisDistribution : public AcceptanceDistribution {
public:
    double getProbability(double temperature, double deltaError) override {
        if (deltaError <= 0) return 1.0;
        return exp(-deltaError / temperature);
    }

    std::shared_ptr<AcceptanceDistribution> clone() override {
        return std::make_shared<MetropolisDistribution>();
    }
};

class HastingsDistribution: public AcceptanceDistribution {
private:
    double gamma;
public:
    HastingsDistribution(double gamma) : AcceptanceDistribution(), gamma(gamma) {
        if (gamma < 1.0) {
            throw new std::logic_error("Bad value for gamma in Hastings criterion");
        }
    }

    double getProbability(double temperature, double deltaError) override {
        double pi_ij = exp(deltaError/temperature);
        double pi_ji = exp(-deltaError/temperature);
        return (1 + 2 * pow(0.5*std::min(pi_ij, pi_ji), gamma))/(1+pi_ij);
    }

    std::shared_ptr<AcceptanceDistribution> clone() override {
        return std::make_shared<HastingsDistribution>(gamma);
    }
};

class BarkerDistribution: public AcceptanceDistribution {
public:
    BarkerDistribution() = default;

    double getProbability(double temperature, double deltaError) override {
        double pi_ij = exp(deltaError/temperature);
        return 1/(1+pi_ij);
    }

    std::shared_ptr<AcceptanceDistribution> clone() override {
        return std::make_shared<BarkerDistribution>();
    }
};

#endif //SA_SCHEDULING_RESOURCES_ACCEPTANCEDISTRIBUTION_H
