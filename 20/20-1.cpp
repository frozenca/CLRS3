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
#include <unordered_map>
#include <vector>

namespace sr = std::ranges;

template <std::size_t t>
class RSVEBTree {
    static_assert(t > 1 && t < 31);
    std::unique_ptr<RSVEBTree<t - t / 2>> summary;
    std::unordered_map<std::size_t, std::unique_ptr<RSVEBTree<t / 2>>> clusters;

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
            if (clusters.contains(high(x))) {
                auto max_low = clusters.at(high(x))->Maximum();
                if (max_low.has_value() && low(x) < max_low) {
                    auto offset = clusters.at(high(x))->Successor(low(x));
                    return index(high(x), offset.value());
                } else {
                    auto succ_cluster = summary->Successor(high(x));
                    if (!succ_cluster.has_value()) {
                        return {};
                    } else {
                        auto offset = clusters.at(succ_cluster.value())->Minimum();
                        return index(succ_cluster.value(), offset.value());
                    }
                }
            } else {
                auto succ_cluster = summary->Successor(high(x));
                if (!succ_cluster.has_value()) {
                    return {};
                } else {
                    auto offset = clusters.at(succ_cluster.value())->Minimum();
                    return index(succ_cluster.value(), offset.value());
                }
            }
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t x) const {
        if (Max.has_value() && x > Max) {
            return Max;
        } else {
            if (clusters.contains(high(x))) {
                auto min_low = clusters.at(high(x))->Minimum();
                if (min_low.has_value() && low(x) > min_low) {
                    auto offset = clusters.at(high(x))->Predecessor(low(x));
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
                        auto offset = clusters.at(pred_cluster)->Maximum();
                        return index(pred_cluster, offset);
                    }
                }
            } else {
                auto pred_cluster = summary->Predecessor(high(x));
                if (!pred_cluster->has_value()) {
                    if (Min.has_value() && x > Min) {
                        return Min;
                    } else {
                        return {};
                    }
                } else {
                    auto offset = clusters.at(pred_cluster)->Maximum();
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
            if (!clusters[high(x)]) {
                clusters[high(x)] = std::make_unique<RSVEBTree<t / 2>>();
            }
            if (!clusters[high(x)]->Minimum().has_value()) {
                if (!summary) {
                    summary = std::make_unique<RSVEBTree<t - t / 2>>();
                }
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

};

template <>
class RSVEBTree<1> {
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

};


int main() {
    RSVEBTree<12> tree;

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


}