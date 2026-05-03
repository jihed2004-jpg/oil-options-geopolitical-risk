# GeoPol Risk Monitor — Oil Options Pricing under Geopolitical Stress

Started this project after watching oil prices swing 15% in a week during a Middle East escalation. I wanted to understand concretely what that kind of volatility shock does to an options portfolio — not just intuitively, but with real numbers.

The Python notebook handles the analytics and visualization. The C++ engine (`pricer/`) was built separately to benchmark performance and run the Monte Carlo at scale without waiting 3 minutes per simulation.

---

## What it does

- Prices vanilla call/put options on crude oil using Black-Scholes
- Computes Greeks: Delta (Δ), Gamma (Γ), Vega (ν)
- Runs Monte Carlo simulation (100k paths) to stress-test the portfolio under vol shocks
- Benchmarks Python vs C++ execution time — C++ runs ~12x faster on the same MC workload

---

## Stress Testing

The main scenario simulates a geopolitical shock: implied volatility jumps from ~25% to ~45% (roughly what happened during major supply disruptions). The notebook shows how Greeks and PnL evolve across that range.

Key observation: Vega exposure dominates at short maturities. Delta becomes secondary when vol moves that fast.

---

## C++ Pricing Engine

The `pricer/` folder contains a standalone C++ implementation of the Black-Scholes pricer and Monte Carlo engine, built with STL templates for type flexibility.

Compile and run:
g++ -O2 -std=c++17 pricer/main.cpp -o pricer/bs_pricer
./pricer/bs_pricer

Benchmark: 100k MC paths in ~0.3s vs ~3.6s in Python (NumPy). The speed-up matters when you're running sensitivity grids across strike/maturity combinations.

---

## Repo structure

.
GeoPol_Risk_Monitor.ipynb   # Main analytics notebook

pricer
 
  main.cpp                # C++ entry point
  black_scholes.hpp       # BS formula + Greeks
  monte_carlo.hpp         # MC engine (100k paths)
  README.md

---

## Stack

Python (NumPy, SciPy, Matplotlib) · C++17 (STL) · Jupyter

---

## Results summary

| Scenario | Vol | Call Price | Delta | Vega |
|---|---|---|---|---|
| Base | 25% | $4.82 | 0.54 | 12.3 |
| Stress +80% | 45% | $8.91 | 0.61 | 18.7 |
| Extreme | 65% | $12.4 | 0.67 | 22.1 |

Under the stress scenario, Vega exposure increases ~52% — the portfolio becomes far more sensitive to further vol moves than to directional price moves.
