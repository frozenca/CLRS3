#include <cassert>
#include <random>
#include <vector>

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t compactListSearch(const std::vector<T>& A, const T& k) {
    size_t i = 0;
    std::uniform_int_distribution<> dist(0, A.size() - 1);
    while (i < A.size() && A[i] < k) {
        size_t j = dist(gen);
        if (A[i] < A[j] && A[j] <= k) {
            i = j;
            if (A[i] == k) {
                return i;
            }
        }
        i++;
    }
    if (i >= A.size() || A[i] > k) {
        return -1;
    } else {
        return i;
    }
}

int main() {

}