#include <cassert>
#include <chrono>
#include <cstdlib>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <utility>
#include <vector>

namespace crn = std::chrono;

using it_p_t = std::pair<std::forward_list<int>::const_iterator,
        std::forward_list<int>::const_iterator>;

std::forward_list<int> mergeKLists(const std::vector<std::forward_list<int>>& lists) {
    auto nodeComp = [](it_p_t& na, it_p_t& nb) {
        return *(na.first) > *(nb.first);
    };
    std::priority_queue<it_p_t, std::vector<it_p_t>, decltype(nodeComp)> iters(nodeComp);
    std::forward_list<int> merged_list;
    if (lists.empty()) return merged_list;
    for (const auto& list : lists) {
        if (!list.empty()) {
            iters.emplace(list.cbegin(), list.cend());
        }
    }
    if (iters.empty()) return merged_list;
    auto curr_iter = iters.top();
    merged_list.push_front(*(curr_iter.first));
    auto merged_it = merged_list.begin();
    while (curr_iter.first != curr_iter.second) {
        iters.pop();
        if (++curr_iter.first != curr_iter.second) {
            iters.push(curr_iter);
        }
        if (iters.empty()) {
            break;
        }
        curr_iter = iters.top();
        merged_it = merged_list.insert_after(merged_it, *(iters.top().first));
    }
    return merged_list;
}

int main() {
    std::forward_list<int> A {1, 4, 5};
    std::forward_list<int> B {1, 3, 4};
    std::forward_list<int> C {2, 6};
    std::vector<std::forward_list<int>> v {A, B, C};
    std::forward_list<int> ABC_correct {1, 1, 2, 3, 4, 4, 5, 6};
    auto ABC = mergeKLists(v);
    assert(ABC == ABC_correct);

    constexpr size_t MAX_SIZE = 1'000'000;
    constexpr size_t NUM_LISTS = 10;
    std::vector<std::forward_list<int>> v2 (NUM_LISTS);
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, NUM_LISTS - 1);
    for (size_t i = MAX_SIZE - 1; i < MAX_SIZE; i--) {
        v2[dist(gen)].push_front(i);
    }
    std::forward_list<int> v2_correct (MAX_SIZE);
    std::iota(v2_correct.begin(), v2_correct.end(), 0);
    auto start = crn::steady_clock::now();
    assert(mergeKLists(v2) == v2_correct);
    auto end = crn::steady_clock::now();
    auto diff = crn::duration_cast<crn::milliseconds>(end - start);
    assert(diff.count() < 1000);
    std::cout << "OK\n";

    return EXIT_SUCCESS;
}