#ifndef BS_H
#define BS_H
#include <cmath>
#include <limits>
#include "utils.h"

using namespace std;

namespace bs {

struct BSInputs {
    double S;
    double K;
    double r;
    double sigma;
    double tau;
};

inline double d1(const BSInputs &in){
    return (utils::slog(in.S / in.K) + (in.r + 0.5 * in.sigma * in.sigma) * in.tau) / (in.sigma * sqrt(in.tau));
}
inline double d2(const BSInputs &in){
    return d1(in) - in.sigma * sqrt(in.tau);
}

inline double call_price(const BSInputs &in){
    if (in.tau <= 0.0) return max(0.0, in.S - in.K);
    double D1 = d1(in), D2 = d2(in);
    return in.S * utils::norm_cdf(D1) - in.K * exp(-in.r * in.tau) * utils::norm_cdf(D2);
}

inline double call_delta(const BSInputs &in){
    if (in.tau <= 0.0) return (in.S > in.K) ? 1.0 : 0.0;
    return utils::norm_cdf(d1(in));
}

inline double call_vega(const BSInputs &in){
    if (in.tau <= 0.0) return 0.0;
    return in.S * sqrt(in.tau) * utils::norm_pdf(d1(in));
}

inline double implied_vol_call(double S, double K, double r, double tau, double market_price,
                               double lo=1e-4, double hi=5.0, double tol=1e-7, int max_iter=100){
    if (tau <= 0.0) return numeric_limits<double>::quiet_NaN();
    auto f = [&](double sig){ BSInputs in{S,K,r,sig,tau}; return call_price(in) - market_price; };
    double flo = f(lo), fhi = f(hi);
    if (isnan(flo) || isnan(fhi)) return numeric_limits<double>::quiet_NaN();
    if (flo * fhi > 0.0){
        hi = 10.0; fhi = f(hi);
        if (flo * fhi > 0.0) return numeric_limits<double>::quiet_NaN();
    }
    for(int it=0; it<max_iter; ++it){
        double mid = 0.5*(lo+hi);
        double fm = f(mid);
        if (fabs(fm) < tol || fabs(hi-lo) < tol) return mid;
        if (flo * fm <= 0.0){ hi = mid; fhi = fm; }
        else{ lo = mid; flo = fm; }
    }
    return 0.5*(lo+hi);
}

} // namespace bs

#endif
