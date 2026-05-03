// black_scholes.hpp
// Black-Scholes option pricer + Greeks
// GeoPol Risk Monitor — M. Jihed Gari

#pragma once
#include <cmath>
#include <stdexcept>

// Standard normal CDF via erfc approximation
inline double norm_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Standard normal PDF
inline double norm_pdf(double x) {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

struct BSResult {
    double price;
    double delta;
    double gamma;
    double vega;
    double theta;
};

// option_type: 'C' for call, 'P' for put
template<typename T>
BSResult black_scholes(T S, T K, T r, T sigma, T T_exp, char option_type = 'C') {
    if (sigma <= 0 || T_exp <= 0)
        throw std::invalid_argument("sigma and T must be strictly positive");

    double d1 = (std::log((double)S / K) + (r + 0.5 * sigma * sigma) * T_exp)
                / (sigma * std::sqrt(T_exp));
    double d2 = d1 - sigma * std::sqrt(T_exp);

    BSResult res;

    if (option_type == 'C') {
        res.price = S * norm_cdf(d1) - K * std::exp(-r * T_exp) * norm_cdf(d2);
        res.delta = norm_cdf(d1);
    } else {
        res.price = K * std::exp(-r * T_exp) * norm_cdf(-d2) - S * norm_cdf(-d1);
        res.delta = norm_cdf(d1) - 1.0;
    }

    // Greeks shared between calls and puts
    res.gamma = norm_pdf(d1) / (S * sigma * std::sqrt(T_exp));
    res.vega  = S * norm_pdf(d1) * std::sqrt(T_exp) / 100.0; // per 1% vol move
    res.theta = -(S * norm_pdf(d1) * sigma) / (2.0 * std::sqrt(T_exp))
                - r * K * std::exp(-r * T_exp) * norm_cdf(d2);

    return res;
}
