#include <cassert>
#include <vector>
#include <iostream>
#include <bitset>

template <size_t N>
std::bitset<N + 1> add(const std::bitset<N>& A, const std::bitset<N>& B) {
    std::bitset<N + 1> C;
    int carry = 0;
    for (size_t j = 0; j < N; j++) {
        auto temp = A[j] + B[j] + carry;
        C[j] = temp % 2;
        carry = temp / 2;
    }
    C[N] = carry;
    return C;
}


int main() {
    size_t a = 37;
    size_t b = 26;
    std::bitset<8> A(a);
    std::bitset<8> B(b);
    auto C = add(A, B);
    assert(C.to_ulong() == a + b);
}