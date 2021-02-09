#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

template <typename T>
std::pair<T, T> minmax(const std::vector<T>& A) {
    std::vector<T> MaxMin, Max, Min, None;
    MaxMin = A;
    while (true) {
        if (MaxMin.size() >= 2) {
            T a = MaxMin[MaxMin.size() - 1];
            T b = MaxMin[MaxMin.size() - 2];
            if (a > b) {
                Max.push_back(a);
                Min.push_back(b);
            } else {
                Max.push_back(b);
                Min.push_back(a);
            }
            MaxMin.pop_back();
            MaxMin.pop_back();
        } else if (!MaxMin.empty() && !None.empty()) {
            T a = MaxMin.back();
            Max.push_back(a);
            MaxMin.pop_back();
        } else if (Min.size() >= 2) {
            T a = Min[Min.size() - 1];
            T b = Min[Min.size() - 2];
            Min.pop_back();
            Min.pop_back();
            if (a > b) {
                None.push_back(a);
                Min.push_back(b);
            } else {
                None.push_back(b);
                Min.push_back(a);
            }
        } else if (Max.size() >= 2) {
            T a = Max[Max.size() - 1];
            T b = Max[Max.size() - 2];
            Max.pop_back();
            Max.pop_back();
            if (a < b) {
                None.push_back(a);
                Max.push_back(b);
            } else {
                None.push_back(b);
                Max.push_back(a);
            }
        } else {
            break;
        }
    }
    assert(Min.size() == 1 && Max.size() == 1 && MaxMin.empty());
    return {Min[0], Max[0]};
}

int main() {
    std::vector<int> v{5, 1, 3, 6, 2};
    auto [Min, Max] = minmax(v);
    std::cout << Min << ' ' << Max;

}