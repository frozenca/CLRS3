#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cmath>
#include <vector>
#include <optional>
#include <utility>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
std::optional<std::size_t> Search(const std::vector<std::vector<T>>& A, const T& x) {
    for (std::size_t i = 0; i < A.size(); i++) {
        if (sr::binary_search(A[i], x)) {
            return i;
        }
    }
    return {};
}

template <typename T>
void Insert(std::vector<std::vector<T>>& A, const T& x) {
    std::vector<T> B;
    B.push_back(x);
    bool inserted = false;
    for (std::size_t i = 0; i < A.size(); i++) {
        if (A[i].empty()) {
            A[i] = std::move(B);
            inserted = true;
            break;
        } else {
            assert(A[i].size() == B.size());
            std::vector<T> B2;
            sr::merge(A[i], B, std::back_inserter(B2));
            std::swap(B, B2);
        }
    }
    if (!inserted) {
        A.push_back(std::move(B));
    }
}

template <typename T>
void Delete(std::vector<std::vector<T>>& A, const T& x) {
    auto i_ = Search(A, x);
    if (!i_.has_value()) {
        return;
    }
    std::size_t i = i_.value();
    auto j = std::distance(A.begin(), sr::find_if(A, [](auto& cont) {return !cont.empty();}));
    auto y = A[j].back();
    A[i].erase(sr::find(A[i], x));
    A[i].insert(sr::lower_bound(A[i], y), y);
    A[j].pop_back();
    for (std::size_t k = 0; k < j; k++) {
        sr::copy(A[j] | srv::take(std::pow(2, k)) | srv::drop(std::pow(2, k) / 2), A[k].begin());
    }
    A[j].clear();
}

int main() {
    std::vector<std::vector<int>> A;

}