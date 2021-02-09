#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <std::size_t k>
class TreeArray {
    std::array<std::size_t, (1u << (2 * k))> data = {0};
    std::array<std::size_t, (1u << k)> summary = {0};

public:
    void Insert(std::size_t value) {
        data[value]++;
        summary[value >> k] == 1;
    }

    void Delete(std::size_t value) {
        data[value]--;
        if (data[value]) {
            return;
        }
        summary[value >> k] == 0;
        for (std::size_t i = ((value >> k) << k); i < (((value >> k) + 1) << k); i++) {
            if (data[i]) {
                summary[value >> k] = 1;
            }
        }
    }

    [[nodiscard]] std::size_t Minimum() const {
        std::size_t index = std::distance(std::begin(summary), sr::find(summary, 1));
        return std::distance(std::begin(data), sr::find(data | srv::drop(index << k) | srv::take((index + 1) << k), 1));
    }

    [[nodiscard]] std::size_t Maximum() const {
        std::size_t index = std::distance(std::rbegin(summary), sr::find(summary | srv::reverse, 1));
        return (1u << (2 * k)) - std::distance(std::rbegin(data), sr::find(data | srv::reverse | srv::drop(index << k) | srv::take((index + 1) << k), 1)) - 1;
    }

    [[nodiscard]] std::optional<size_t> Successor(std::size_t value) const {
        std::size_t index = value >> k;
        auto cluster_end = std::begin(data) + ((index + 1) >> k);
        auto first_search = sr::find_if(data | srv::drop(value + 1) | srv::take((index + 1) >> k), [](auto& n){return n > 0;});
        if (first_search != cluster_end) {
            return std::distance(std::begin(data), first_search);
        } else {
            auto second_search = sr::find(summary | srv::drop(index + 1), 1);
            if (second_search != std::end(summary)) {
                auto dist = std::distance(std::begin(summary), second_search);
                return std::distance(std::begin(data),
                    sr::find_if(data | srv::drop(dist >> k) | srv::take((dist + 1) >> k), [](auto& n){return n > 0;}));
            } else {
                return {};
            }
        }
    }

    [[nodiscard]] std::optional<size_t> Predeccessor(std::size_t value) const {
        if (!value) {
            return {};
        }
        std::size_t cluster_begin = (value >> k) << k;
        for (std::size_t i = value - 1; i >= cluster_begin && i < (((value) >> k) + 1) << k; i--) {
            if (data[i]) {
                return i;
            }
        }
        std::size_t cluster_index = (value >> k);
        if (!cluster_index) {
            return {};
        }
        for (std::size_t i = cluster_index - 1; i >= 0 && i < (1u << k); i--) {
            if (summary[i]) {
                for (std::size_t j = ((i + 1) << k) - 1; j >= (i << k) && j < ((i + 1) << k); j--) {
                    if (data[j]) {
                        return j;
                    }
                }
            }
        }
        return {};
    }

};


int main() {

}