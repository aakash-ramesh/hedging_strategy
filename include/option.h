#ifndef OPTION_H
#define OPTION_H
#include "bs.h"
#include <string>

using namespace std;


class EuropeanCall {
public:
    EuropeanCall(double K, const string &expiryISO): K_(K), expiry_(expiryISO) {}
    double price(double S, double r, double sigma, double tau) const {
        return bs::call_price(bs::BSInputs{S,K_,r,sigma,tau});
    }
    double delta(double S, double r, double sigma, double tau) const {
        return bs::call_delta(bs::BSInputs{S,K_,r,sigma,tau});
    }
    double strike() const { return K_; }
    const string& expiry() const { return expiry_; }
private:
    double K_{};
    string expiry_{};
};

#endif
