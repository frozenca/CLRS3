#include <cassert>
#include <complex>
using namespace std::complex_literals;

int main() {
    std::complex<int> z1 = 1. + 2i;
    std::complex<int> z2 = 3. + 4i;
    int a = z1.real();
    int b = z1.imag();
    int c = z2.real();
    int d = z2.imag();
    int ac = a * c;
    int bd = b * d;
    int a_bc_d = (a + b) * (c + d);
    std::complex<int> z3 = (ac - bd) * 1.0 + (a_bc_d - (ac - bd)) * 1.0 * 1i;
    assert(z1 * z2 == z3);

}