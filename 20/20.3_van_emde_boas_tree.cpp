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

template <std::size_t t>
class VEBTree {
    static_assert(t > 1 && t < 31);
    std::unique_ptr<VEBTree<t - t / 2>> summary;
    std::array<std::unique_ptr<VEBTree<t / 2>>, (1u << (t - t / 2))> clusters;

    static constexpr std::size_t v = (1u << (t / 2));

    std::optional<std::size_t> Min;
    std::optional<std::size_t> Max;

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
    VEBTree() : summary {std::make_unique<VEBTree<t - t / 2>>()} {
        for (std::size_t i = 0; i < (1u << (t - t / 2)); i++) {
            clusters[i] = std::make_unique<VEBTree<t / 2>>();
        }
    }

    void EmptyInsert(std::size_t x) {
        assert(!Min.has_value());
        Min = x;
        Max = x;
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        return Min;
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        return Max;
    }

    [[nodiscard]] bool Member(std::size_t x) const {
        if (x == Min || x == Max) {
            return true;
        } else {
            return clusters[high(x)]->Member(low(x));
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t x) const {
        if (Min.has_value() && x < Min) {
            return Min;
        } else {
            auto max_low = clusters[high(x)]->Maximum();
            if (max_low.has_value() && low(x) < max_low) {
                auto offset = clusters[high(x)]->Successor(low(x));
                return index(high(x), offset.value());
            } else {
                auto succ_cluster = summary->Successor(high(x));
                if (!succ_cluster.has_value()) {
                    return {};
                } else {
                    auto offset = clusters[succ_cluster.value()]->Minimum();
                    return index(succ_cluster.value(), offset.value());
                }
            }
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t x) const {
        if (Max.has_value() && x > Max) {
            return Max;
        } else {
            auto min_low = clusters[high(x)]->Minimum();
            if (min_low.has_value() && low(x) > min_low) {
                auto offset = clusters[high(x)]->Predecessor(low(x));
                return index(high(x), offset);
            } else {
                auto pred_cluster = summary->Predecessor(high(x));
                if (!pred_cluster->has_value()) {
                    if (Min.has_value() && x > Min) {
                        return Min;
                    } else {
                        return {};
                    }
                } else {
                    auto offset = clusters[pred_cluster]->Maximum();
                    return index(pred_cluster, offset);
                }
            }
        }
    }

    void Insert(std::size_t x) {
        if (!Min.has_value()) {
            EmptyInsert(x);
        } else {
            if (x < Min) {
                std::swap(x, Min.value());
            }
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Insert(high(x));
                clusters[high(x)]->EmptyInsert(low(x));
            } else {
                clusters[high(x)]->Insert(low(x));
            }
            if (x > Max) {
                Max = x;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            Min = {};
            Max = {};
        } else {
            if (x == Min) {
                auto first_cluster = summary->Minimum();
                x = index(first_cluster.value(), clusters[first_cluster.value()]->Minimum().value());
                Min = x;
            }
            clusters[high(x)]->Delete(low(x));
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Delete(high(x));
                if (x == Max) {
                    auto summary_max = summary->Maximum();
                    if (!summary_max.has_value()) {
                        Max = Min;
                    } else {
                        Max = index(summary_max.value(), clusters[summary_max.value()]->Maximum().value());
                    }
                }
            } else if (x == Max) {
                Max = index(high(x), clusters[high(x)]->Maximum().value());
            }
        }
    }

};

template <>
class VEBTree<1> {
    std::optional<std::size_t> Min;
    std::optional<std::size_t> Max;

public:
    void EmptyInsert(std::size_t x) {
        assert(!Min.has_value());
        Min = x;
        Max = x;
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        return Min;
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        return Max;
    }

    [[nodiscard]] bool Member(std::size_t x) const {
        if (x == Min || x == Max) {
            return true;
        } else {
            return false;
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t x) const {
        if (!x && Max == 1) {
            return 1;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t x) const {
        if (x && Min == 0) {
            return 0;
        } else {
            return {};
        }
    }

    void Insert(std::size_t x) {
        if (!Min.has_value()) {
            EmptyInsert(x);
        } else {
            if (x < Min) {
                std::swap(x, Min.value());
            }
            if (x > Max) {
                Max = x;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            Min = {};
            Max = {};
        } else {
            if (!x) {
                Min = 1;
            } else {
                Min = 0;
            }
            Max = Min;
        }
    }
};


int main() {
    VEBTree<12> tree;

    constexpr std::size_t N = 1'000;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Insert(n);
    }

    auto curr = tree.Minimum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = tree.Successor(curr.value());
    }

    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Delete(n);
    }

    curr = tree.Minimum();
    assert(!curr.has_value());


}