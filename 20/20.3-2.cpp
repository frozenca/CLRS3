#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

namespace sr = std::ranges;

template <std::size_t t, typename T>
class VEBTree {
    using data_t = std::optional<std::pair<std::size_t, T>>;

    static_assert(t > 1 && t < 31);
    std::unique_ptr<VEBTree<t - t / 2, T>> summary;
    std::array<std::unique_ptr<VEBTree<t / 2, T>>, (1u << (t - t / 2))> clusters;

    static constexpr std::size_t v = (1u << (t / 2));

    data_t Min;
    data_t Max;

    [[nodiscard]] std::size_t high(std::size_t x) const {
        return x / v;
    }

    [[nodiscard]] std::size_t low(std::size_t x) const {
        return x % v;
    }

    [[nodiscard]] std::size_t index(std::size_t x, std::size_t y) const {
        return x * v + y;
    }

public:
    VEBTree() : summary {std::make_unique<VEBTree<t - t / 2, T>>()} {
        for (std::size_t i = 0; i < (1u << (t - t / 2)); i++) {
            clusters[i] = std::make_unique<VEBTree<t / 2, T>>();
        }
    }

    void EmptyInsert(std::size_t x, const T& data) {
        assert(!Min.has_value());
        Min = {x, data};
        Max = {x, data};
    }

    [[nodiscard]] data_t Minimum() const {
        return Min;
    }

    [[nodiscard]] data_t Maximum() const {
        return Max;
    }

    [[nodiscard]] bool Member(std::size_t x) const {
        if (x == Min->first || x == Max->first) {
            return true;
        } else {
            return clusters[high(x)]->Member(low(x));
        }
    }

    [[nodiscard]] data_t Successor(std::size_t x) const {
        if (Min.has_value() && x < Min->first) {
            return Min;
        } else {
            auto max_low = clusters[high(x)]->Maximum();
            if (max_low.has_value() && low(x) < max_low->first) {
                auto offset = clusters[high(x)]->Successor(low(x));
                return std::make_pair(index(high(x), offset->first), offset->second);
            } else {
                auto succ_cluster = summary->Successor(high(x));
                if (!succ_cluster.has_value()) {
                    return {};
                } else {
                    auto offset = clusters[succ_cluster->first]->Minimum();
                    return std::make_pair(index(succ_cluster->first, offset->first), offset->second);
                }
            }
        }
    }

    [[nodiscard]] data_t Predecessor(std::size_t x) const {
        if (Max.has_value() && x > Max->first) {
            return Max;
        } else {
            auto min_low = clusters[high(x)]->Minimum();
            if (min_low.has_value() && low(x) > min_low->first) {
                auto offset = clusters[high(x)]->Predecessor(low(x)).value();
                return std::make_pair(index(high(x), offset->first), offset->second);
            } else {
                auto pred_cluster = summary->Predecessor(high(x));
                if (!pred_cluster->has_value()) {
                    if (Min.has_value() && x > Min->first) {
                        return Min;
                    } else {
                        return {};
                    }
                } else {
                    auto offset = clusters[pred_cluster->first]->Maximum().value();
                    return std::make_pair(index(pred_cluster->first, offset->first), offset->second);
                }
            }
        }
    }

    void Insert(std::size_t x, const T& data) {
        if (!Min.has_value()) {
            EmptyInsert(x, data);
        } else {
            std::pair<std::size_t, T> y = {x, data};
            if (x < Min->first) {
                std::swap(y, Min.value());
            }
            x = y.first;
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Insert(high(x), data);
                clusters[high(x)]->EmptyInsert(low(x), data);
            } else {
                clusters[high(x)]->Insert(low(x), data);
            }
            if (x > Max->first) {
                Max = y;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            Min = {};
            Max = {};
        } else {
            if (x == Min->first) {
                auto first_cluster = summary->Minimum();
                auto next = clusters[first_cluster->first]->Minimum();
                x = index(first_cluster->first, next->first);
                Min = {x, next->second};
            }
            clusters[high(x)]->Delete(low(x));
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Delete(high(x));
                if (x == Max->first) {
                    auto summary_max = summary->Maximum();
                    if (!summary_max.has_value()) {
                        Max = Min;
                    } else {
                        auto prev = clusters[summary_max->first]->Maximum();
                        Max = {index(summary_max->first, prev->first), prev->second};
                    }
                }
            } else if (x == Max->first) {
                auto prev = clusters[high(x)]->Maximum();
                Max = {index(high(x), prev->first), prev->second};
            }
        }
    }

};

template <typename T>
class VEBTree<1, T> {
    using data_t = std::optional<std::pair<std::size_t, T>>;

    data_t Min;
    data_t Max;

public:
    void EmptyInsert(std::size_t x, const T& data) {
        assert(!Min.has_value());
        Min = {x, data};
        Max = {x, data};
    }

    [[nodiscard]] data_t Minimum() const {
        return Min;
    }

    [[nodiscard]] data_t Maximum() const {
        return Max;
    }

    [[nodiscard]] bool Member(std::size_t x) const {
        if (x == Min->first || x == Max->first) {
            return true;
        } else {
            return false;
        }
    }

    [[nodiscard]] data_t Successor(std::size_t x) const {
        if (!x && Max->first == 1) {
            return Max;
        } else {
            return {};
        }
    }

    [[nodiscard]] data_t Predecessor(std::size_t x) const {
        if (x && Min->first == 0) {
            return Min;
        } else {
            return {};
        }
    }

    void Insert(std::size_t x, const T& data) {
        if (!Min.has_value()) {
            EmptyInsert(x, data);
        } else {
            std::pair<std::size_t, T> y = {x, data};
            if (x < Min->first) {
                std::swap(y, Min.value());
            }
            x = y.first;
            if (x > Max->first) {
                Max = y;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            Min = {};
            Max = {};
        } else {
            if (!x) {
                Min = Max;
            }
            Max = Min;
        }
    }
};


int main() {
    VEBTree<12, int> tree;

    constexpr std::size_t N = 1'000;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Insert(n, 0);
    }

    auto curr = tree.Minimum();
    while (curr.has_value()) {
        std::cout << curr->first << ' ';
        curr = tree.Successor(curr->first);
    }

    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Delete(n);
    }

    curr = tree.Minimum();
    assert(!curr.has_value());


}