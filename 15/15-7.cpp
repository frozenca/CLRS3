#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <ranges>

namespace sr = std::ranges;

void Viterbi(const std::vector<std::string>& obs, const std::vector<std::string>& states,
             std::unordered_map<std::string, double>& start_prob,
             std::unordered_map<std::string, std::unordered_map<std::string, double>>& transition_prob,
             std::unordered_map<std::string, std::unordered_map<std::string, double>>& emit_prob) {
    std::vector<std::unordered_map<std::string, std::pair<double, std::string>>> V;
    V.emplace_back();
    for (const auto& state : states) {
        V[0][state] = {start_prob[state] * emit_prob[state][obs[0]], ""};
    }

    for (size_t t = 1; t < obs.size(); t++) {
        V.emplace_back();
        for (const auto& state_curr : states) {
            auto max_trans_prob = 0.0;
            std::string prev_state;
            for (const auto& state_prev : states) {
                auto trans_prob = V[t - 1][state_prev].first * transition_prob[state_prev][state_curr];
                if (trans_prob > max_trans_prob) {
                    max_trans_prob = trans_prob;
                    prev_state = state_prev;
                }
            }
            auto max_prob = max_trans_prob * emit_prob[state_curr][obs[t]];
            V[t][state_curr] = {max_prob, prev_state};
        }
    }

    std::vector<std::string> optimum;
    auto max_prob = 0.0;
    std::string best_state;
    std::string previous_state;

    for (const auto& [state, data] : V.back()) {
        if (data.first > max_prob) {
            max_prob = data.first;
            best_state = state;
        }
    }
    optimum.push_back(best_state);
    previous_state = best_state;

    for (size_t t = V.size() - 2; t < V.size(); t--) {
        optimum.push_back(V[t + 1][previous_state].second);
        previous_state = V[t + 1][previous_state].second;
    }

    sr::reverse(optimum);

    std::cout << "The steps of the most probable state is: ";
    for (const auto& state : optimum) {
        std::cout << state << ' ';
    }
    std::cout << '\n';

}

int main() {
    std::vector<std::string> obs = {"normal", "cold", "dizzy"};
    std::vector<std::string> states = {"Healthy", "Fever"};
    std::unordered_map<std::string, double> start_prob = {{"Healthy", 0.6}, {"Fever", 0.4}};

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
            transition_prob = {{"Healthy", {{"Healthy", 0.7}, {"Fever", 0.3}}},
                        {"Fever",   {{"Healthy", 0.4}, {"Fever", 0.6}}}};

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
            emit_prob = {{"Healthy", {{"normal", 0.5}, {"cold", 0.4}, {"dizzy", 0.1}}},
                         {"Fever",   {{"normal", 0.1}, {"cold", 0.3}, {"dizzy", 0.6}}}};

    Viterbi(obs, states, start_prob, transition_prob, emit_prob);


}