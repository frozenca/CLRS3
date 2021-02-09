#include <array>
#include <vector>
#include <utility>

template <std::size_t n>
class GraphMatrix {
    std::array<std::size_t, n * n> adj;
public:
    [[nodiscard]] bool hasUniversalSink() const {
        std::size_t i = 0;
        std::size_t j = 0;
        while (i < n && j < n) {
            if (adj[i * n + j] == 0) {
                j++;
            } else {
                i++;
            }
        }
        if (i == n) {
            return false;
        } else {
            for (std::size_t k = 0; k < n; k++) {
                if (k != i && adj[i * n + k] == 0) {
                    return false;
                }
            }
        }
        return true;
    }
};

int main() {

}
