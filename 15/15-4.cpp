#include <cassert>
#include <iostream>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

size_t GiveLines(const std::vector<size_t>& p, const std::vector<std::string>& tokens, size_t j) {
    size_t i = p[j];
    size_t k = 0;
    if (i == 1) {
        k = 1;
    } else if (i == 0) {
        k = 0;
    } else {
        k = GiveLines(p, tokens, i - 1) + 1;
    }
    for (size_t idx = i; idx <= j; idx++) {
        std::cout << tokens[idx] << ' ';
    }
    std::cout << '\n';
    return k;
}

void PrintNeatly(const std::string& data, size_t M) {
    std::vector<std::string> tokens;
    std::stringstream ss (data);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    size_t n = tokens.size();
    std::vector<std::vector<int>> extras(n, std::vector<int> (n));
    for (size_t i = 0; i < n; i++) {
        assert(M >= tokens[i].length());
        extras[i][i] = M - tokens[i].length();
        for (size_t j = i + 1; j < n; j++) {
            extras[i][j] = extras[i][j - 1] - tokens[j].length() - 1;
        }
    }
    std::vector<std::vector<int>> cost(n, std::vector<int> (n));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i; j < n; j++) {
            if (extras[i][j] < 0) {
                cost[i][j] = std::numeric_limits<int>::max();
            } else if (j == n - 1 && extras[i][j] >= 0) {
                cost[i][j] = 0;
            } else {
                cost[i][j] = std::pow(extras[i][j], 3);
            }
        }
    }
    std::vector<int> c (n + 1);
    std::vector<size_t> p (n);
    for (size_t j = 0; j < n; j++) {
        c[j + 1] = std::numeric_limits<int>::max();
        for (size_t i = 0; i <= j; i++) {
            if (cost[i][j] != std::numeric_limits<int>::max() && c[i] + cost[i][j] < c[j + 1]) {
                c[j + 1] = c[i] + cost[i][j];
                p[j] = i;
            }
        }
    }
    GiveLines(p, tokens, n - 1);
}

int main() {
    std::string data = "The algorithm has found universal application in decoding the convolutional codes used in both CDMA and GSM digital cellular, dial-up modems, satellite, deep-space communications, and 802.11 wireless LANs.";

    PrintNeatly(data, 30);


}