#include <cassert>
#include <cmath>
#include <iostream>
#include "include/bs.h"

using namespace std;


int main(){
    double S=100,K=100,r=0.01,tau=0.5,sigma_true=0.2;
    double price = bs::call_price(bs::BSInputs{S,K,r,sigma_true,tau});
    double iv = bs::implied_vol_call(S,K,r,tau,price);
    assert(fabs(iv - sigma_true) < 1e-4);

    double d1 = bs::call_delta(bs::BSInputs{200,100,0.0,0.2,1.0});
    double d2 = bs::call_delta(bs::BSInputs{50, 100,0.0,0.2,1.0});
    assert(d1 > 0.99);
    assert(d2 < 0.01);

    cout << "All tests passed.\n";
    return 0;
}
