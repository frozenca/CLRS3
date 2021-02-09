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

using data_t = std::optional<std::pair<std::size_t, std::size_t>>;

template <std::size_t t>
class VEBTree {
    static_assert(t > 1 && t < 31);
    std::unique_ptr<VEBTree<t - t / 2>> summary;
    std::array<std::unique_ptr<VEBTree<t / 2>>, (1u << (t - t / 2))> clusters;

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
    VEBTree() : summary {std::make_unique<VEBTree<t - t / 2>>()} {
        for (std::size_t i = 0; i < (1u << (t - t / 2)); i++) {
            clusters[i] = std::make_unique<VEBTree<t / 2>>();
        }
    }

    void EmptyInsert(std::size_t x, std::size_t freq = 1) {
        assert(!Min.has_value());
        Min = {x, freq};
        Max = {x, freq};
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
            if (min_low.has_value() && low(x) > min_low) {
                auto offset = clusters[high(x)]->Predecessor(low(x));
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
                    auto offset = clusters[pred_cluster->first]->Maximum();
                    return std::make_pair(index(pred_cluster->first, offset->first), offset->second);
                }
            }
        }
    }

    void Insert(std::size_t x, std::size_t freq = 1) {
        if (!Min.has_value()) {
            EmptyInsert(x, freq);
        } else {
            std::pair<std::size_t, std::size_t> y = {x, freq};
            if (y.first < Min->first) {
                std::swap(y, Min.value());
            }
            x = y.first;
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Insert(high(x));
                clusters[high(x)]->EmptyInsert(low(x), freq);
            } else {
                clusters[high(x)]->Insert(low(x), freq);
            }
            if (y.first > Max->first) {
                Max = y;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            if (Min.has_value()) {
                Min->second--;
                Max->second--;
                if (Min->second == 0) {
                    Min = {};
                    Max = {};
                }
            }
        } else {
            if (x == Min->first) {
                Min->second--;
                if (Min->second == 0) {
                    auto first_cluster = summary->Minimum();
                    auto next = clusters[first_cluster->first]->Minimum();
                    x = index(first_cluster->first, next->first);
                    Min = {x, next->second};
                }
            }
            clusters[high(x)]->Delete(low(x));
            if (!clusters[high(x)]->Minimum().has_value()) {
                summary->Delete(high(x));
                if (x == Max->first) {
                    Max->second--;
                    if (Max->second == 0) {
                        auto summary_max = summary->Maximum();
                        if (!summary_max.has_value()) {
                            Max = Min;
                        } else {
                            auto prev = clusters[summary_max->first]->Maximum();
                            Max = {index(summary_max->first, prev->first), prev->second};
                        }
                    }
                }
            } else if (x == Max->first) {
                Max->second--;
                if (Max->second == 0) {
                    auto prev = clusters[high(x)]->Maximum();
                    Max = {index(high(x), prev->first), prev->second};
                }
            }
        }
    }

};

template <>
class VEBTree<1> {
    data_t Min;
    data_t Max;

public:
    void EmptyInsert(std::size_t x, std::size_t freq = 1) {
        assert(!Min.has_value());
        Min = {x, freq};
        Max = {x, freq};
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

    void Insert(std::size_t x, std::size_t freq = 1) {
        if (!Min.has_value()) {
            EmptyInsert(x, freq);
        } else if (x) {
            if (Min == Max) {
                if (Min->first == 0) {
                    Max = {x, freq};
                } else {
                    Min->second += freq;
                    Max->second += freq;
                }
            } else {
                Max->second += freq;
            }
        } else {
            if (Min == Max) {
                if (Min->first == 0) {
                    Min->second += freq;
                    Max->second += freq;
                } else {
                    Min = {x, freq};
                }
            } else {
                Min->second += freq;
            }
        }
    }

    void Delete(std::size_t x) {
        if (Min == Max) {
            if (Min.has_value()) {
                Min->second--;
                Max->second--;
                if (Min->second == 0) {
                    Min = {};
                    Max = {};
                }
            }
        } else {
            if (!x) {
                Min->second--;
                if (Min->second == 0) {
                    Min = Max;
                }
            } else {
                Max->second--;
                if (Max->second == 0) {
                    Max = Min;
                }
            }
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