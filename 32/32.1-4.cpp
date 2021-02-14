#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

bool GapMatching(const std::string& T, const std::string& P,
                 const char delimeter = ' ', std::size_t t = 0, std::size_t p = 0) {
    if (p == P.length()) {
        return t <= T.length();
    }
    if (t == T.length()) {
        return false;
    }
    if (T[t] == P[p]) {
        if (p + 1 < P.length() && P[p + 1] == delimeter) {
            return GapMatching(T, P, delimeter, t + 1, p + 2);
        } else {
            return GapMatching(T, P, delimeter, t + 1, p + 1);
        }
    } else {
        return GapMatching(T, P, delimeter, t + 1, p);
    }
}

int main() {
    assert(GapMatching("cabccbacbacab", "ab ba c"));
}