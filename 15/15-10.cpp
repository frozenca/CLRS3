#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

double Invest(double d, size_t yr, size_t n, const std::vector<std::vector<double>>& r, double f_1, double f_2) {
    std::vector<size_t> I (yr + 1);
    std::vector<double> R (yr + 1);
    for (size_t k = yr - 1; k < yr; k--) {
        size_t q = 0;
        for (size_t i = 0; i < n; i++) {
            if (r[i][k] > r[q][k]) {
                q = i;
            }
        }
        if (R[k + 1] + d * r[I[k + 1]][k] - f_1 > R[k + 1] + d * r[q][k] - f_2) {
            R[k] = R[k + 1] + d * r[I[k + 1]][k] - f_1;
            I[k] = I[k + 1];
        } else {
            R[k] = R[k + 1] + d * r[q][k] - f_2;
            I[k] = q;
        }
    }
    return R[0];
}

int main() {

}