#include "include/hedger.h"
#include "include/bs.h"
#include "include/utils.h"
#include <iostream>
#include <iomanip>

using namespace std;

vector<HedgePathResult> DeltaHedger::simulate_gbm_and_hedge(
        int Npaths, int Nsteps, double S0, double mu, double sigma, double r,
        double T, double K, unsigned seed, const string &out_dir){
    mt19937_64 rng(seed);
    normal_distribution<double> Z(0.0,1.0);

    double dt = T / static_cast<double>(Nsteps);
    EuropeanCall call(K, "sim-expiry");

    vector<HedgePathResult> results; results.reserve(Npaths);

    ofstream f_paths(out_dir+"/out/sim_paths.csv");
    ofstream f_call(out_dir+"/out/sim_option_prices.csv");
    ofstream f_he(out_dir+"/out/sim_hedge_errors.csv");

    f_paths << "path,step,S\n";
    f_call << "path,step,C\n";
    f_he   << "path,HE_T\n";

    for (int p=0; p<Npaths; ++p){
        HedgePathResult R; R.S.resize(Nsteps+1); R.C.resize(Nsteps+1); R.delta.resize(Nsteps+1); R.HE.resize(Nsteps+1);
        R.S[0]=S0;
        double t=0.0;
        double tau0 = T - t;
        double C0 = call.price(R.S[0], r, sigma, tau0);
        double d0 = call.delta(R.S[0], r, sigma, tau0);
        R.C[0]=C0; R.delta[0]=d0; R.HE[0]=0.0;

        double B_prev = C0 - d0*R.S[0];

        for (int i=1; i<=Nsteps; ++i){
            t += dt;
            double S_prev = R.S[i-1];
            double dS = S_prev*mu*dt + S_prev*sqrt(dt)*Z(rng)*sigma;
            double S_now = S_prev + dS; if (S_now<=0) S_now=1e-8;
            R.S[i]=S_now;

            double tau = max(0.0, T - t);
            double C_now = call.price(S_now, r, sigma, tau);
            double d_now = call.delta(S_now, r, sigma, tau);
            R.C[i]=C_now; R.delta[i]=d_now;

            double HE_i = R.delta[i-1]*S_now + B_prev*exp(r*dt) - C_now;
            R.HE[i]=HE_i;
            B_prev = R.delta[i-1]*S_now + B_prev*exp(r*dt) - d_now*S_now;
        }
        R.HE_T = R.HE.back();
        results.push_back(R);

        if (p<100){
            for (int i=0;i<=Nsteps;++i) f_paths<<p<<","<<i<<","<<R.S[i]<<"\n";
            for (int i=0;i<=Nsteps;++i) f_call <<p<<","<<i<<","<<R.C[i]<<"\n";
        }
        f_he<<p<<","<<R.HE_T<<"\n";
    }
    return results;
}

void DeltaHedger::hedge_with_market_data(
        const vector<MarketRow> &rows,
        const EuropeanCall &call,
        const string &out_csv){
    if (rows.empty()) return;
    ofstream out(out_csv);
    out << fixed << setprecision(6);
    out << "date,S,V,imp_vol,delta,HE,PNL,PNL_with_hedge\n";

    double S0 = rows.front().S;
    double V0 = rows.front().V;

    double tau0 = rows.front().tau;
    double r0   = rows.front().r;
    double iv0  = bs::implied_vol_call(S0, call.strike(), r0, tau0, V0);
    double d0   = isnan(iv0) ? 0.0 : bs::call_delta(bs::BSInputs{S0, call.strike(), r0, iv0, tau0});
    double C0   = V0;
    double B_prev = C0 - d0*S0;

    for (size_t i=0;i<rows.size();++i){
        const auto &row = rows[i];
        double tau = row.tau;
        double r   = row.r;
        double S   = row.S;
        double V   = row.V;

        double iv = bs::implied_vol_call(S, call.strike(), r, tau, V);
        double d  = isnan(iv) ? 0.0 : bs::call_delta(bs::BSInputs{S, call.strike(), r, iv, tau});

        double dt = 1.0/252.0;
        double HE_i = (i==0) ? 0.0 : (d0 * S + B_prev * exp(r0 * dt) - V);
        if (i==0) HE_i = 0.0;

        double PNL_i = V0 - V;
        double PNLH_i = HE_i;

        out << row.date << "," << S << "," << V << "," << iv << "," << d << "," << HE_i
            << "," << PNL_i << "," << PNLH_i << "\n";

        if (i+1<rows.size()){
            B_prev = d0 * S + B_prev * exp(r0 * dt) - d * S;
            d0 = d; r0 = r;
        }
    }
}
