#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <utility>
#include <numeric>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

double weightedMedian1(const std::vector<std::pair<double, double>>& A) {
    auto B = A;
    sr::sort(B);
    double w = 0.0;
    size_t i = 0;
    for (; i < B.size(); i++) {
        w += B[i].second;
        if (w > 0.5) {
            break;
        }
    }
    return B[i].first;
}

double weightedMedian2(const std::vector<std::pair<double, double>>& A) {
    if (A.size() == 1) {
        return A[0].first;
    } else if (A.size() == 2) {
        if (A[0].second >= A[1].second) {
            return A[0].first;
        } else {
            return A[1].first;
        }
    } else {
        auto B = A;
        sr::nth_element(B, B.begin() + B.size() / 2);
        double median = B[B.size() / 2].first;
        double w_L = std::accumulate(B.begin(), B.begin() + B.size() / 2, 0.0, [](double a, auto& p) {return a + p.second;});
        double w_R = std::accumulate(B.begin() + 1 + B.size() / 2 , B.end(), 0.0, [](double a, auto& p) {return a + p.second;});
        if (w_L < 0.5 && w_R < 0.5) {
            return median;
        } else if (w_L > 0.5) {
            B[B.size() / 2].second += w_R;
            std::vector<std::pair<double, double>> C (std::make_move_iterator(B.begin()), std::make_move_iterator(B.begin() + 1 + B.size() / 2));
            return weightedMedian2(C);
        } else {
            B[B.size() / 2].second += w_L;
            std::vector<std::pair<double, double>> C (std::make_move_iterator(B.begin() + B.size() / 2), std::make_move_iterator(B.end()));
            return weightedMedian2(C);
        }
    }
}

std::pair<double, double> weightedMedian2d(const std::vector<std::tuple<double, double, double>>& A) {
    std::vector<std::pair<double, double>> X, Y;
    for (const auto& [x, y, w] : A) {
        X.emplace_back(x, w);
        Y.emplace_back(y, w);
    }
    auto x_m = weightedMedian2(X);
    auto y_m = weightedMedian2(Y);
    return {x_m, y_m};
}


int main() {
    std::vector<std::pair<double, double>> vw {{1, 0.1}, {3, 0.2}, {5, 0.3}, {7, 0.4}};
    std::cout << weightedMedian1(vw) << '\n';
    std::cout << weightedMedian2(vw) << '\n';

    std::vector<std::tuple<double, double, double>> xyw {{1, 7, 0.1},
                                                         {3, 5, 0.2},
                                                         {5, 3, 0.3},
                                                         {7, 1, 0.4}};
    auto [x_m, y_m] = weightedMedian2d(xyw);
    std::cout << x_m << ' ' << y_m;

}