// monte_carlo.hpp
// Monte Carlo pricer — GBM paths, European payoff
// GeoPol Risk Monitor — M. Jihed Gari

#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <numeric>

struct MCResult {
    double price;
    double std_error;
    double convergence_pct; // std_error / price * 100
};

// Prices a European option via Monte Carlo under GBM
// n_paths: number of simulated paths (100k recommended)
// n_steps: time steps per path (daily = T*252)
MCResult monte_carlo_price(
    double S, double K, double r, double sigma,
    double T_exp, char option_type = 'C',
    int n_paths = 100000, int n_steps = 252,
    unsigned int seed = 42)
{
    std::mt19937_64 rng(seed);
    std::normal_distribution<double> Z(0.0, 1.0);

    double dt = T_exp / n_steps;
    double drift     = (r - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);
    double discount  = std::exp(-r * T_exp);

    std::vector<double> payoffs(n_paths);

    for (int i = 0; i < n_paths; ++i) {
        double St = S;
        for (int j = 0; j < n_steps; ++j)
            St *= std::exp(drift + diffusion * Z(rng));

        double payoff = (option_type == 'C')
            ? std::max(St - K, 0.0)
            : std::max(K - St, 0.0);
        payoffs[i] = payoff;
    }

    double mean = std::accumulate(payoffs.begin(), payoffs.end(), 0.0) / n_paths;

    double variance = 0.0;
    for (double p : payoffs) variance += (p - mean) * (p - mean);
    variance /= (n_paths - 1);

    MCResult res;
    res.price           = discount * mean;
    res.std_error       = discount * std::sqrt(variance / n_paths);
    res.convergence_pct = (res.price > 0) ? res.std_error / res.price * 100.0 : 0.0;

    return res;
}
