#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <string>
#include <stdexcept>
#include <limits>
#include <algorithm>

using namespace std;

namespace utils {

inline double clamp(double x, double a, double b){ return max(a, min(x,b)); }

inline double norm_pdf(double x){
    static const double INV_SQRT_2PI = 1.0 / sqrt(2.0 * M_PI);
    return INV_SQRT_2PI * exp(-0.5 * x * x);
}
inline double norm_cdf(double x){
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

inline double slog(double x){
    if (x <= 0.0) throw domain_error("log of non-positive");
    return log(x);
}

inline double rel_diff(double a, double b){
    return fabs(a-b) / (fabs(a)+fabs(b)+1e-12);
}

} // namespace utils
#endif
