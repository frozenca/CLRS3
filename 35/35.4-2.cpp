#include <algorithm>
#include <iostream>
#include <random>
#include <ranges>
#include <numeric>
#include <utility>
#include <unordered_set>
#include <vector>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());
std::bernoulli_distribution to_add(0.5);

std::size_t CountSatisfiedClauses(const std::vector<std::unordered_set<std::size_t>>& clauses, std::size_t num_variables) {
    std::unordered_set<std::size_t> satisfied;
    for (std::size_t i = 0; i < num_variables; i++) {
        if (to_add(gen)) {
            satisfied.insert(i);
        }
    }
    std::size_t cnt = 0;
    for (const auto& clause : clauses) {
        for (auto var : satisfied) {
            if (clause.contains(var)) {
                cnt++;
                break;
            }
        }
    }
    return cnt;
}

int main() {
    constexpr std::size_t num_clauses = 100;
    constexpr std::size_t num_variables = 100;
    std::vector<std::size_t> vars (num_variables);
    std::iota(vars.begin(), vars.end(), 0);
    std::vector<std::unordered_set<std::size_t>> clauses;

    for (std::size_t i = 0; i < num_clauses; i++) {
        sr::shuffle(vars, gen);
        std::unordered_set<std::size_t> clause;
        clause.insert(vars[0]);
        clause.insert(vars[1]);
        clause.insert(vars[2]);
        std::size_t j = 3;
        while (to_add(gen) && j < num_variables) {
            clause.insert(vars[j]);
            j++;
        }
        clauses.push_back(std::move(clause));
    }

    auto count_clauses = CountSatisfiedClauses(clauses, num_variables);
    std::cout << count_clauses << " / " << num_clauses << '\n';
}