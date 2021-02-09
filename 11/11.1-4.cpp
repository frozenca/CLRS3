#include <cassert>
#include <random>
#include <array>

struct Dictionary {
    std::array<size_t, 10'000'000> T;
    std::array<size_t, 1'000> S;
    size_t topS = 0;

    size_t Search(size_t k) {
        if (1 <= T[k] && T[k] <= topS && S[T[k]] == k) {
            return S[T[k]];
        } else {
            return -1;
        }
    }

    void Insert(size_t k) {
        S[topS] = k;
        T[k] = topS;
        topS++;
    }

    void Delete(size_t k) {
        S[T[k]] = S[topS];
        T[S[T[k]]] = T[k];
        T[k] = -1;
        topS--;
    }

};

int main() {

}