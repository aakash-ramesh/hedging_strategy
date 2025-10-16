#ifndef DATE_H
#define DATE_H
#include <string>
#include <ctime>
#include <stdexcept>
#include <sstream>

using namespace std;

struct YMD { int y; int m; int d; };

inline YMD parse_date(const string &s){
    if (s.size() >= 10 && (s[4]=='-' || s[4]=='/')){
        YMD out{stoi(s.substr(0,4)), stoi(s.substr(5,2)), stoi(s.substr(8,2))};
        return out;
    } else if (s.size() >= 10 && s[2]=='/' && s[5]=='/'){
        YMD out{stoi(s.substr(6,4)), stoi(s.substr(0,2)), stoi(s.substr(3,2))};
        return out;
    }
    throw runtime_error("Unrecognized date format: "+s);
}

inline string to_string(const YMD &d){
    ostringstream os; os<<d.y<<"-"<<(d.m<10?"0":"")<<d.m<<"-"<<(d.d<10?"0":"")<<d.d; return os.str();
}

inline bool is_leap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

inline int days_in_month(int y, int m){
    static const int L[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (m==2) return L[1] + (is_leap(y)?1:0); return L[m-1];
}

inline int weekday(const YMD &d){
    int y=d.y, m=d.m, q=d.d;
    if (m<3){ m+=12; y-=1; }
    int K = y%100, J = y/100;
    int h = (q + (13*(m+1))/5 + K + K/4 + J/4 + 5*J) % 7;
    int w = (h+6)%7;
    return w;
}

inline bool is_weekend(const YMD &d){ int w=weekday(d); return (w==0 || w==6); }

inline YMD add_days(YMD d, int n){
    int dd = d.d + n;
    int y=d.y, m=d.m;
    while (dd > days_in_month(y,m)){
        dd -= days_in_month(y,m); m++; if (m>12){ m=1; y++; }
    }
    while (dd <= 0){
        m--; if (m<1){ m=12; y--; }
        dd += days_in_month(y,m);
    }
    return YMD{y,m,dd};
}

inline int business_days_between(YMD a, YMD b){
    if (to_string(a) == to_string(b)) return 0;
    int step = ( (a.y<b.y) || (a.y==b.y && (a.m<b.m || (a.m==b.m && a.d<b.d))) ) ? 1 : -1;
    int count=0; YMD cur=a;
    while (to_string(cur) != to_string(b)){
        cur = add_days(cur, step);
        if (!is_weekend(cur)) count += (step>0?1:-1);
    }
    return count;
}

#endif
