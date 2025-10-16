#ifndef HEDGER_H
#define HEDGER_H
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <unordered_map>
#include "option.h"
#include "date.h"

using namespace std;

struct HedgePathResult {
    vector<double> S;
    vector<double> C;
    vector<double> delta;
    vector<double> HE;
    double HE_T = 0.0;
};

class DeltaHedger {
public:
    static vector<HedgePathResult> simulate_gbm_and_hedge(
        int Npaths, int Nsteps, double S0, double mu, double sigma, double r,
        double T, double K, unsigned seed=42, const string &out_dir=".");

    struct MarketRow { string date; double S; double V; double r; double tau; };

    static void hedge_with_market_data(
        const vector<MarketRow> &rows,
        const EuropeanCall &call,
        const string &out_csv);
};

#endif
