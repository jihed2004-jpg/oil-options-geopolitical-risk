// main.cpp
// GeoPol Risk Monitor — C++ Pricing Engine
// Benchmarks Black-Scholes analytical vs Monte Carlo (100k paths)
// Stress test: base vol 25% -> shock 45% -> extreme 65%
// M. Jihed Gari — Polytech Lyon MAM

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include "black_scholes.hpp"
#include "monte_carlo.hpp"

void print_separator() {
    std::cout << std::string(65, '-') << "\n";
}

void run_stress_test(double S, double K, double r, double T_exp) {
    std::vector<std::pair<std::string, double>> scenarios = {
        {"Base     (vol=25%)", 0.25},
        {"Stress   (vol=45%)", 0.45},
        {"Extreme  (vol=65%)", 0.65}
    };

    std::cout << "\nSTRESS TEST — Call option (S=" << S << ", K=" << K << ", T=" << T_exp << "y)\n";
    print_separator();
    std::cout << std::left
              << std::setw(24) << "Scenario"
              << std::setw(10) << "Price"
              << std::setw(10) << "Delta"
              << std::setw(10) << "Gamma"
              << std::setw(10) << "Vega\n";
    print_separator();

    for (auto& [label, sigma] : scenarios) {
        BSResult res = black_scholes(S, K, r, sigma, T_exp, 'C');
        std::cout << std::left
                  << std::setw(24) << label
                  << std::setw(10) << std::fixed << std::setprecision(3) << res.price
                  << std::setw(10) << res.delta
                  << std::setw(10) << res.gamma
                  << std::setw(10) << res.vega << "\n";
    }
    print_separator();
}

void benchmark_mc_vs_bs(double S, double K, double r, double sigma, double T_exp) {
    std::cout << "\nBENCHMARK — Monte Carlo (100k paths) vs Black-Scholes\n";
    print_separator();

    // Black-Scholes — analytical
    auto t0 = std::chrono::high_resolution_clock::now();
    BSResult bs = black_scholes(S, K, r, sigma, T_exp, 'C');
    auto t1 = std::chrono::high_resolution_clock::now();
    double bs_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // Monte Carlo — 100k paths
    auto t2 = std::chrono::high_resolution_clock::now();
    MCResult mc = monte_carlo_price(S, K, r, sigma, T_exp, 'C', 100000);
    auto t3 = std::chrono::high_resolution_clock::now();
    double mc_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Black-Scholes price : " << bs.price << "  (" << bs_ms << " ms)\n";
    std::cout << "Monte Carlo price   : " << mc.price << "  (" << mc_ms << " ms)\n";
    std::cout << "MC std error        : " << mc.std_error << "\n";
    std::cout << "MC convergence      : " << mc.convergence_pct << "%\n";
    std::cout << "Pricing error BS/MC : " << std::abs(bs.price - mc.price) / bs.price * 100 << "%\n";
    print_separator();
}

int main() {
    std::cout << "=== GeoPol Risk Monitor — C++ Pricing Engine ===\n";

    // Oil proxy parameters (WTI-like)
    double S     = 85.0;   // spot price $/bbl
    double K     = 85.0;   // ATM strike
    double r     = 0.05;   // risk-free rate
    double T_exp = 0.25;   // 3-month expiry
    double sigma = 0.25;   // base implied vol 25%

    run_stress_test(S, K, r, T_exp);
    benchmark_mc_vs_bs(S, K, r, sigma, T_exp);

    std::cout << "\nDone. Compile with: g++ -O2 -std=c++17 main.cpp -o bs_pricer\n";
    return 0;
}
