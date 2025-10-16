#ifndef IO_H
#define IO_H
#include <string>
#include <unordered_map>
#include <vector>
#include "date.h"
#include "hedger.h"

using namespace std;

namespace io {

unordered_map<string,double> load_interest_csv(const string &path);
unordered_map<string,double> load_sec_csv(const string &path);

vector<DeltaHedger::MarketRow> load_options_slice(
    const string &path,
    double K,
    const string &expiryISO,
    const string &t0ISO,
    const string &tNISO,
    const unordered_map<string,double> &Smap,
    const unordered_map<string,double> &rmap
);

}

#endif
