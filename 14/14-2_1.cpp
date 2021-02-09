#include <cassert>
#include <vector>

std::vector<size_t> Josephus(size_t n, size_t m) {
    assert(m && n && n >= m);
    std::vector<size_t> res;
    for (size_t i = 0; i < n; i++) {
        res.push_back(m * (i + 1) % n);
    }
    return res;
}

int main() {

}