#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>

template <std::size_t t>
class ProtoVEB {
    static_assert(t > 0 && t < 6);
    std::unique_ptr<ProtoVEB<t - 1>> summary;
    std::array<std::unique_ptr<ProtoVEB<t - 1>>, (1u << (1u << (t - 1)))> clusters;

    static constexpr std::size_t v = (1u << (1u << (t - 1)));

public:
    ProtoVEB() : summary {std::make_unique<ProtoVEB<t - 1>>()} {
        for (std::size_t i = 0; i < clusters.size(); i++) {
            clusters[i] = std::make_unique<ProtoVEB<t - 1>>();
        }
    }

    [[nodiscard]] bool Member(std::size_t x) const {
        return clusters[high(x)]->Member(low(x));
    }

    [[nodiscard]] std::size_t high (std::size_t x) const {
        return x / v;
    }

    [[nodiscard]] std::size_t low (std::size_t x) const {
        return x % v;
    }

    [[nodiscard]] std::size_t index (std::size_t x, std::size_t y) const {
        return x * v + y;
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        auto min_cluster = summary->Minimum();
        if (!min_cluster.has_value()) {
            return {};
        } else {
            auto offset = clusters[min_cluster]->Minimum();
            return index(min_cluster, offset);
        }
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        auto max_cluster = summary->Maximum();
        if (!max_cluster.has_value()) {
            return {};
        } else {
            auto offset = clusters[max_cluster]->Maximum();
            return index(max_cluster, offset);
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t x) const {
        auto offset = clusters[high(x)]->Successor(low(x));
        if (offset.has_value()) {
            return index(high(x), offset);
        } else {
            auto succ_cluster = summary->Successor(high(x));
            if (!succ_cluster.has_value()) {
                return {};
            } else {
                auto offset2 = clusters[succ_cluster]->Minimum();
                return index(succ_cluster, offset2);
            }
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t x) const {
        auto offset = clusters[high(x)]->Predecessor(low(x));
        if (offset.has_value()) {
            return index(high(x), offset);
        } else {
            auto pred_cluster = summary->Predecessor(high(x));
            if (!pred_cluster.has_value()) {
                return {};
            } else {
                auto offset2 = clusters[pred_cluster]->Maximum();
                return index(pred_cluster, offset2);
            }
        }
    }

    void Insert(std::size_t x) {
        clusters[high(x)]->Insert(low(x));
        summary->Insert(high(x));
    }
};

template <>
class ProtoVEB<0> {
    std::array<bool, 2> data = {false};

public:

    [[nodiscard]] bool Member(std::size_t x) const {
        assert(x < 2);
        return data[x];
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        if (data[0]) {
            return 0;
        } else if (data[1]) {
            return 1;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        if (data[1]) {
            return 1;
        } else if (data[0]) {
            return 0;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t x) const {
        if (!x && data[1]) {
            return 1;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t x) const {
        if (x == 1 && data[0]) {
            return 0;
        } else {
            return {};
        }
    }

    void Insert(std::size_t x) {
        assert(x < 2);
        data[x] = true;
    }
};


int main() {
    ProtoVEB<4> veb;
    veb.Insert(6);

}