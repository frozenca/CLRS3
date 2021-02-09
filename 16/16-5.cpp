#include <cassert>
#include <vector>
#include <iostream>
#include <set>
#include <queue>
#include <unordered_map>
#include <list>
#include <limits>
#include <utility>
#include <random>

void cacheManager(const std::vector<size_t>& R, size_t k) {
    std::set<size_t> T;
    std::priority_queue<std::pair<size_t, size_t>> P;
    std::unordered_map<size_t, std::list<size_t>> H;
    size_t ind = 0;
    for (size_t i = 0; i < R.size(); i++) {
        H[R[i]].push_back(i);
    }
    for (auto r : R) {
        H[r].pop_front();
        size_t time = std::numeric_limits<size_t>::max();
        if (!H[r].empty()) {
            time = H[r].front();
        }
        std::cout << "element " << r << ", time "
                  << (time == std::numeric_limits<size_t>::max() ? -1 : static_cast<int>(time)) << " : ";
        if (T.contains(r)) {
            std::cout << "cache hit\n";
            auto [evTime, evValue] = P.top();
            if (evValue == r) {
                P.pop();
                P.emplace(time, r);
            }
        } else if (T.size() < k) {
            T.insert(r);
            P.emplace(time, r);
            std::cout << "cache miss, inserted " << r << " in empty slot\n";
        } else {
            auto [evTime, evValue] = P.top();
            std::cout << " evTime " << (evTime == std::numeric_limits<size_t>::max() ? -1 : static_cast<int>(evTime))
                      << ", evValue " << evValue << " : ";
            if (evTime <= time) {
                std::cout << "cache miss, no data element evicted\n";
            } else {
                std::cout << "cache miss, evict data element " << evValue;
                P.pop();
                T.erase(evValue);
                P.emplace(time, r);
                std::cout << " and insert " << r << '\n';
                T.insert(r);
            }
        }
    }
}

int main() {
    constexpr size_t memory_sz = 25;
    constexpr size_t cache_sz = 5;
    constexpr size_t num_access = 125;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> memory_access(0, memory_sz);
    std::vector<size_t> memory_requests;
    memory_requests.reserve(num_access);
    for (size_t i = 0; i < num_access; i++) {
        memory_requests.push_back(memory_access(gen));
    }
    cacheManager(memory_requests, cache_sz);

}
