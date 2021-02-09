#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <ranges>

namespace sr = std::ranges;

enum class Dir {
    upright,
    up,
    right,
};

std::string LongestPalindromicSubsequence(const std::string& str) {
    std::vector<std::vector<size_t>> len (str.length(), std::vector<size_t>(str.length()));
    for (size_t i = 0; i < str.length(); i++) {
        len[i][i] = 1;
    }

    std::vector<std::vector<Dir>> B (str.length(), std::vector<Dir>(str.length(), Dir::upright));

    for (size_t l = 2; l <= str.length(); l++) {
        for (size_t i = 0; i < str.length() - l + 1; i++) {
            size_t j = i + l - 1;
            if (str[i] == str[j]) {
                len[i][j] = len[i + 1][j - 1]  + 2;
                B[i][j] = Dir::upright;
            } else if (len[i][j - 1] > len[i + 1][j]) {
                len[i][j] = len[i][j - 1];
                B[i][j] = Dir::up;
            } else {
                len[i][j] = len[i + 1][j];
                B[i][j] = Dir::right;
            }
        }
    }
    size_t i = 0, j = str.length() - 1;
    std::string result;
    bool centered = false;
    while (len[i][j]) {
        if (len[i][j] == 1) {
            assert(i == j);
            centered = true;
            break;
        }
        if (B[i][j] == Dir::upright) {
            result += str[i];
            i++;
            j--;
        } else if (B[i][j] == Dir::up) {
            j--;
        } else {
            i++;
        }
    }
    auto rev = result;
    sr::reverse(rev);
    if (centered) {
        result = result + str[i] + rev;
    } else {
        result = result + rev;
    }
    return result;
}

int main() {
    std::cout << LongestPalindromicSubsequence("character");

}