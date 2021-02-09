#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <numeric>
#include <utility>
#include <vector>
#include <algorithm>

namespace sr = std::ranges;

constexpr int INF = std::numeric_limits<int>::max();

int extractMin(std::vector<std::vector<int>>& tableau) {
    int val = tableau[0][0];
    tableau[0][0] = INF;
    std::pair<size_t, size_t> curPos = {0, 0};
    size_t R = tableau.size();
    size_t C = tableau[0].size();
    while (true) {
        auto [r, c] = curPos;
        int minVal = tableau[r][c];
        size_t minR = r;
        size_t minC = c;
        if (r < R - 1 && minVal > tableau[r + 1][c]) {
            minVal = tableau[r + 1][c];
            minR = r + 1;
        }
        if (c < C - 1 && minVal > tableau[r][c + 1]) {
            minVal = tableau[r][c + 1];
            minR = r;
            minC = c + 1;
        }
        if (minR == r && minC == c) {
            break;
        } else {
            std::swap(tableau[r][c], tableau[minR][minC]);
            curPos = {minR, minC};
        }
    }
    return val;
}

void insertKey(std::vector<std::vector<int>>& tableau, int key) {
    size_t R = tableau.size();
    size_t C = tableau[0].size();
    tableau[R - 1][C - 1] = key;
    std::pair<size_t, size_t> curPos = {R - 1, C - 1};
    while (true) {
        auto [r, c] = curPos;
        int maxVal = tableau[r][c];
        size_t maxR = r;
        size_t maxC = c;
        if (r > 0 && maxVal < tableau[r - 1][c]) {
            maxVal = tableau[r - 1][c];
            maxR = r - 1;
        }
        if (c > 0 && maxVal < tableau[r][c - 1]) {
            maxVal = tableau[r][c - 1];
            maxR = r;
            maxC = c - 1;
        }
        if (maxR == r && maxC == c) {
            break;
        } else {
            std::swap(tableau[r][c], tableau[maxR][maxC]);
            curPos = {maxR, maxC};
        }
    }
}

bool findElement(const std::vector<std::vector<int>>& tableau, int key) {
    size_t R = tableau.size();
    size_t C = tableau[0].size();
    std::pair<size_t, size_t> curPos = {0, C - 1};
    while (curPos.second < C && curPos.first < R) {
        int curKey = tableau[curPos.first][curPos.second];
        if (curKey == key) {
            return true;
        } else if (curKey < key) {
            curPos.first++;
        } else {
            curPos.second--;
        }
    }
    return false;
}

int main() {
    std::vector<std::vector<int>> tableau {{2, 3, 12, 14},
                                     {4, 8, 16, INF},
                                     {5, 9, INF, INF},
                                     {INF, INF, INF, INF}};

    extractMin(tableau);

    for (const auto& row : tableau) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }

    insertKey(tableau, 2);

    for (const auto& row : tableau) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }

    std::vector<std::vector<int>> emptytableau {{INF, INF, INF, INF},
                                           {INF, INF, INF, INF},
                                           {INF, INF, INF, INF},
                                           {INF, INF, INF, INF}};

    std::vector<int> v (emptytableau.size() * emptytableau[0].size());
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (auto n : v) {
        insertKey(emptytableau, n);
    }

    for (const auto& row : emptytableau) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }

    v.clear();

    for (size_t i = 0; i < emptytableau.size() * emptytableau[0].size(); i++) {
        v.push_back(extractMin(emptytableau));
    }

    for (auto n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    assert(findElement(tableau, 9));
    assert(!findElement(tableau, 17));
}