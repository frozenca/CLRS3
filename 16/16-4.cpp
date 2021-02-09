#include <algorithm>
#include <cassert>
#include <vector>
#include <limits>
#include <utility>
#include <iostream>
#include <random>
#include <ranges>

namespace sr = std::ranges;

size_t Find(std::vector<size_t>& parents, size_t x) {
    assert(parents[x] != std::numeric_limits<size_t>::max() && x < parents.size());
    while (parents[x] != x) {
        parents[x] = parents[parents[x]];
        x = parents[x];
    }
    return x;
}

void Union(std::vector<size_t>& parents, std::vector<size_t>& sizes, std::vector<size_t>& low, size_t x, size_t y) {
    auto xRoot = Find(parents, x);
    auto yRoot = Find(parents, y);

    if (xRoot == yRoot) {
        return;
    }

    if (sizes[xRoot] < sizes[yRoot]) {
        std::swap(xRoot, yRoot);
    }

    parents[yRoot] = xRoot;
    low[yRoot] = std::min(low[yRoot], low[xRoot]);
    low[xRoot] = std::min(low[yRoot], low[xRoot]);
    sizes[xRoot] += sizes[yRoot];
}

// (penalty, deadlines)
std::pair<double, std::vector<size_t>> schedulingVariations(const std::vector<std::pair<double, size_t>>& tasks) {
    std::vector<size_t> slots (tasks.size(), std::numeric_limits<size_t>::max());
    std::vector<size_t> parents (tasks.size(), std::numeric_limits<size_t>::max());
    std::vector<size_t> low (tasks.size(), std::numeric_limits<size_t>::max());
    std::vector<size_t> sizes (tasks.size());
    double penalty = 0;
    for (size_t i = 0; i < tasks.size(); i++) {
        size_t time = tasks[i].second;
        if (slots[time] != std::numeric_limits<size_t>::max()) { // occupied
            size_t target = low[Find(parents, time)] - 1;
            if (target < tasks.size()) {
                time = target;
            } else if (slots[tasks.size() - 1] != std::numeric_limits<size_t>::max()) { // rightmost is still occupied
                target = low[Find(parents, tasks.size() - 1)] - 1;
                assert(target < tasks.size());
                time = target;
            } else { // rightmost is unoccupied
                time = tasks.size() - 1;
            }
        }
        parents[time] = time;
        sizes[time] = 1;
        slots[time] = i;
        low[time] = time;
        if (time > tasks[i].second) {
            penalty += tasks[i].first;
        }
        if (time - 1 < tasks.size() && slots[time - 1] != std::numeric_limits<size_t>::max()) {
            Union(parents, sizes, low, time - 1, time);
        }
        if (time + 1 < tasks.size() && slots[time + 1] != std::numeric_limits<size_t>::max()) {
            Union(parents, sizes, low, time + 1, time);
        }
    }

    return {penalty, slots};
}

int main() {
    constexpr size_t sz = 10;
    std::mt19937 gen(std::random_device{}());

    std::vector<std::pair<double, size_t>> tasks (sz);
    std::uniform_int_distribution<> distDeadlines (0, sz - 1);
    std::uniform_real_distribution<> distPenalty (0.0, 1.0);
    for (size_t i = 0; i < sz; i++) {
        tasks[i] = {distPenalty(gen), distDeadlines(gen)};
    }
    sr::sort(tasks, [](auto& task1, auto& task2) {return task1.first > task2.first;});
    for (const auto& [pen, dead] : tasks) {
        std::cout << "penalty " << pen << ", deadline " << dead << '\n';
    }
    auto [penalty, slots] = schedulingVariations(tasks);
    std::cout << "Total penalty : " << penalty << '\n';
    std::cout << "Slots : ";
    for (auto slot : slots) {
        std::cout << slot << ' ';
    }

}
