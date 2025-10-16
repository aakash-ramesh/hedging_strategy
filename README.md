# Delta Hedging (Single-Folder C++ + Python)

This project implements a **dynamic delta-hedging** strategy for a European call option using:

* **Task 1 (Simulation)**: GBM price paths + Black–Scholes pricing/delta; computes terminal hedging errors.
* **Task 2 (Market Data)**: Uses provided GOOG option/stock/rate CSVs to construct a daily delta-hedged portfolio and produce a results table (incl. IV, delta, HE, PNL, PNL with hedge).

Everything is in **one folder**.

---

## 1) Build


```bash
g++ -O3 -std=c++17 -o main main.cpp hedger.cpp io.cpp
```

---

## 2) Inputs & Outputs

### Required input files (Task 2)

Place these in the **`data` directory**:

* `interest.csv`  — columns include: `date`, `rate` (annualized percentage, e.g. `0.29` for 0.29%)
* `sec_GOOG.csv`  — columns include: `date`, `close adj` (or a recognized alias)
* `op_GOOG.csv`   — columns include: `date`, `exdate`, `cp_flag`, `strike`, `best_bid`, `best_offer`



### Output files

* **Task 1**

  * `sim_paths.csv` – (path, step, S). *By default, the code writes the first 100 paths for plotting; remove the cap in `hedger.cpp` to dump all.*
  * `sim_option_prices.csv` – (path, step, C)
  * `sim_hedge_errors.csv` – (path, HE_T), for all simulated paths
* **Task 2**

  * `result.csv` – (date, S, V, imp_vol, delta, HE, PNL, PNL_with_hedge)

---

## 3) Run


`main.cpp` dispatches by `mode=`:

* **Task 1 (simulation)**

```bash
./main mode=sim
```

Parameters baked in per spec: `npaths=1000, nsteps=100, S0=100, mu=0.05, sigma=0.24, r=0.025, T=0.4, K=105`.

* **Task 2 (market data)**

```bash
./main mode=market t0=2011-07-05 tN=2011-07-29 T=2011-09-17 K=500 \
  interest=data/interest.csv sec=data/sec_GOOG.csv opt=data/op_GOOG.csv out=out/result.csv
```

Parameters `t0, tN, T, K` can be changed as per the provided data.

---

## 4) Python plots

Install deps:

```bash
pip install pandas matplotlib
```

Create figures:

```bash
python plot_sim_paths.py            # 100 simulated paths together, option price overlay, HE histogram
python plot_market_results.py       # S, V, IV, Delta, PNL vs hedged PNL
python plot_delta_vs_price.py       # Optional overlay S vs Delta (sanity check)
```

---

## 5) File overview

* `main.cpp` — Unified file for tasks 1 and 2 (`mode=sim` or `mode=market`)
* `hedger.cpp` / `include/hedger.h` —
  * **Task 1**: GBM simulation, Black–Scholes pricing/delta, cumulative hedging error update per spec
  * **Task 2**: Daily re-hedging given market mid quotes; writes `result.csv`
* `io.cpp` / `include/io.h` — CSV loaders for interest, stock, and options slices (filters by K, expiry, window)
* `include/bs.h` — Black–Scholes helpers: price, delta, vega, bisection IV
* `include/date.h` — lightweight date utilities; business-day count ignores weekends (no holiday calendar)
* `include/utils.h` — math helpers, safe log, normal PDF/CDF
* `plot_*.py` — plotting scripts for your report

---


## 6) Unit tests

Two simple tests are included:

```bash
./tests_bin
```

* **Implied volatility recovery** (price → IV close to true sigma)
* **Delta edge cases** (deep ITM ≈ 1, deep OTM ≈ 0)

---

