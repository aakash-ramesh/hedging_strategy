#include <iostream>
#include <stdexcept>
#include <string>
#include "include/hedger.h"
#include "include/io.h"

using namespace std;

static string get_arg(int argc, char**argv, const string &key, const string &def=""){
    string k = key + "=";
    for (int i=1; i<argc; ++i){
        string a(argv[i]);
        if (a.rfind(k, 0) == 0) return a.substr(k.size());
    }
    return def;
}

static int run_sim(){
    // Task 1 defaults (per spec)
    int npaths = 1000;
    int nsteps = 100;
    double s0=100.0, mu=0.05, sigma=0.24, r=0.025, T=0.4, K=105.0;

    auto results = DeltaHedger::simulate_gbm_and_hedge(npaths, nsteps, s0, mu, sigma, r, T, K, 50, ".");
    (void)results;
    cout << "Generated sim_paths.csv, sim_option_prices.csv, sim_hedge_errors.csv\n";
    return 0;
}

static int run_market(int argc, char** argv){
    // sensible defaults so ./app mode=market works with your CSVs
    
        string t0 = get_arg(argc, argv, "t0");
        string tN = get_arg(argc, argv, "tN");
        string T   = get_arg(argc, argv, "T");
        double K        = stod(get_arg(argc, argv, "K"));
        string f_ir= get_arg(argc, argv, "interest", "interest.csv");
        string f_sec= get_arg(argc, argv, "sec", "sec_GOOG.csv");
        string f_opt= get_arg(argc, argv, "opt", "op_GOOG.csv");
        string f_out= get_arg(argc, argv, "out", "result.csv");

        auto rmap = io::load_interest_csv(f_ir);
        auto smap = io::load_sec_csv(f_sec);
        auto rows = io::load_options_slice(f_opt, K, T, t0, tN, smap, rmap);

        if (rows.empty()){
            cerr << "No rows matched the filters. Check K, T, dates, or CSV columns.\n";
            return 1;
        }

        EuropeanCall call(K, T);
        DeltaHedger::hedge_with_market_data(rows, call, f_out);

        cout << "Wrote " << f_out << " with columns: date,S,V,imp_vol,delta,HE,PNL,PNL_with_hedge\n";
        return 0;
}



int main(int argc, char** argv){
    try{
        string mode = get_arg(argc, argv, "mode", "sim"); // default to simulation
        if (mode == "sim")    return run_sim();
        if (mode == "market") return run_market(argc, argv);

        cerr << "Unknown mode. Use mode=sim or mode=market\n";
        return 2;
    }catch(const exception &e){
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
