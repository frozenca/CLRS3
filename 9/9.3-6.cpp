#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t i = p;
    for (size_t j = p; j < r; j++) {
        if (A[j] <= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
size_t randomizedPartition(std::vector<T>& A, size_t p, size_t r) {
    std::uniform_int_distribution<> dst (p, r);
    size_t i = dst(gen);
    std::swap(A[i], A[r]);
    return partition(A, p, r);
}

template <typename T>
std::pair<size_t, T> randomizedSelect(std::vector<T>& A, size_t p, size_t r, size_t i) {
    while (true) {
        if (p == r) {
            return {p, A[p]};
        }
        size_t q = randomizedPartition(A, p, r);
        size_t k = q - p + 1;
        if (i == k) {
            return {q, A[q]};
        }
        if (i < k) {
            r = q;
        } else {
            p = q + 1;
            i = i - k;
        }
    }
}

template <typename T>
void kthQuantilesHelper(std::vector<T>& A, size_t p, size_t r,
        std::vector<size_t>& pos, size_t f, size_t e, std::vector<T>& quantiles) {
    if (f >= e) {
        return;
    }
    size_t mid = (f + e) / 2;
    auto [q, val] = randomizedSelect(A, p, r, pos[mid]);
    quantiles[mid] = val;
    size_t k = q - p + 1;
    for (size_t i = mid + 1; i < e; i++) {
        pos[i] -= k;
    }
    kthQuantilesHelper(A, q + 1, r, pos, mid + 1, e, quantiles);
}

template <typename T>
std::vector<T> kthQuantiles(std::vector<T>& A, size_t k) {
    std::vector<size_t> pos (k);
    for (size_t i = 0; i < pos.size(); i++) {
        pos[i] = i * A.size() / k;
    }
    std::vector<T> quantiles (k);
    kthQuantilesHelper(A, 0, A.size() - 1, pos, 0, pos.size(), quantiles);
    return quantiles;
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4, 8, 7};
    auto v_3 = kthQuantiles(v, 3);
    for (auto n : v_3) {
        std::cout << n << ' ';
    }

}