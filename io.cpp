#include "include/io.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;


static string trim(const string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a==string::npos) return ""; return s.substr(a, b-a+1);
}

static vector<string> split_csv(const string &line){
    vector<string> out; stringstream ss(line); string tok;
    while (getline(ss, tok, ',')) out.push_back(tok);
    return out;
}

unordered_map<string,double> io::load_interest_csv(const string &path){
    unordered_map<string,double> m; m.reserve(400);
    ifstream f(path);
    if (!f) throw runtime_error("Cannot open "+path);
    string header; getline(f,header);
    auto cols = split_csv(header);
    int idx_date=-1, idx_rate=-1;
    for (int i=0;i<(int)cols.size();++i){
        string h = trim(cols[i]);
        if (idx_date<0 && (h=="date" || h=="Date")) idx_date=i;
        if (idx_rate<0 && (h=="rate(%)" || h=="Rate" || h=="risk_free" || h=="riskfree")) idx_rate=i;
    }
    if (idx_date<0 || idx_rate<0) throw runtime_error("interest.csv: expected headers include date, rate");

    string line;
    while (getline(f,line)){
        if (line.empty()) continue; auto toks = split_csv(line);
        if ((int)toks.size() <= max(idx_date, idx_rate)) continue;
        string date = trim(toks[idx_date]);
        string rate = trim(toks[idx_rate]);
        if (date.empty()||rate.empty()) continue;
        double r_pct = stod(rate);
        double r = r_pct / 100.0;
        m[date]=r;
    }
    return m;
}

unordered_map<string,double> io::load_sec_csv(const string &path){
    unordered_map<string,double> m; m.reserve(400);
    ifstream f(path);
    if (!f) throw runtime_error("Cannot open "+path);
    string header; getline(f,header);
    auto cols = split_csv(header);
    int idx_date=-1, idx_close=-1;
    for (int i=0;i<(int)cols.size();++i){
        string h = trim(cols[i]);
        if (idx_date<0 && (h=="date" || h=="Date")) idx_date=i;
        if (idx_close<0 && (h=="close adj" || h=="close_adj" || h=="adj_close" || h=="Adj Close")) idx_close=i;
    }
    if (idx_date<0 || idx_close<0) throw runtime_error("sec_GOOG.csv: expected headers include date, close adj");

    string line;
    while (getline(f,line)){
        if (line.empty()) continue; auto toks = split_csv(line);
        if ((int)toks.size() <= max(idx_date, idx_close)) continue;
        string date = trim(toks[idx_date]);
        string v = trim(toks[idx_close]);
        if (date.empty()||v.empty()) continue;
        m[date] = stod(v);
    }
    return m;
}

vector<DeltaHedger::MarketRow> io::load_options_slice(
    const string &path,
    double K,
    const string &expiryISO,
    const string &t0ISO,
    const string &tNISO,
    const unordered_map<string,double> &Smap,
    const unordered_map<string,double> &rmap){

    YMD t0 = parse_date(t0ISO), tN = parse_date(tNISO);

    ifstream f(path);
    if (!f) throw runtime_error("Cannot open "+path);
    string header; getline(f,header);
    auto cols = split_csv(header);

    int i_date=-1, i_exdate=-1, i_cp=-1, i_strike=-1, i_bid=-1, i_ask=-1;
    for (int i=0;i<(int)cols.size();++i){
        string h = trim(cols[i]);
        if (i_date<0   && (h=="date" || h=="Date")) i_date=i;
        if (i_exdate<0 && (h=="exdate" || h=="expiration" || h=="expiry")) i_exdate=i;
        if (i_cp<0     && (h=="cp_flag" || h=="cp" || h=="type")) i_cp=i;
        if (i_strike<0 && (h=="strike_price" || h=="K")) i_strike=i;
        if (i_bid<0    && (h=="best_bid" || h=="bid")) i_bid=i;
        if (i_ask<0    && (h=="best_offer" || h=="ask" || h=="best_ask")) i_ask=i;
    }
    if (i_date<0||i_exdate<0||i_cp<0||i_strike<0||i_bid<0||i_ask<0)
        throw runtime_error("op_GOOG.csv: expected headers include date, exdate, cp_flag, strike, best_bid, best_offer");

    vector<DeltaHedger::MarketRow> rows;
    string line;
    while (getline(f,line)){
        if (line.empty()) continue; auto toks = split_csv(line);
        if ((int)toks.size() <= max({i_date,i_exdate,i_cp,i_strike,i_bid,i_ask})) continue;
        string date  = trim(toks[i_date]);
        string exd   = trim(toks[i_exdate]);
        string cp    = trim(toks[i_cp]);
        string sK    = trim(toks[i_strike]);
        string sbid  = trim(toks[i_bid]);
        string sask  = trim(toks[i_ask]);
        if (cp != "C") continue;
        double Krow = stod(sK);
        if (fabs(Krow - K) > 1e-9) continue;
        if (to_string(parse_date(exd)) != expiryISO) continue;
        YMD d = parse_date(date);
        bool within = (business_days_between(t0,d) >= 0) && (business_days_between(d,tN) >= 0);
        if (!within) continue;
        auto itS = Smap.find(to_string(d));
        auto itr = rmap.find(to_string(d));
        if (itS==Smap.end() || itr==rmap.end()) continue;
        double bb = stod(sbid), bo = stod(sask);
        double mid = 0.5*(bb+bo);
        int bd = business_days_between(d, parse_date(expiryISO));
        double tau = max(0, bd) / 252.0;
        rows.push_back(DeltaHedger::MarketRow{to_string(d), itS->second, mid, itr->second, tau});
    }
    sort(rows.begin(), rows.end(), [](auto &a, auto &b){ return a.date < b.date; });
    return rows;
}
