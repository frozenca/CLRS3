#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

Point2d operator+(const Point2d& lhs, const Point2d& rhs) {
    return Point2d(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point2d operator-(const Point2d& lhs, const Point2d& rhs) {
    return Point2d(lhs.x - rhs.x, lhs.y - rhs.y);
}

bool operator==(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs == rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

std::vector<Point2d> GrahamScan(const std::vector<Point2d>& Q) {
    assert(Q.size() >= 3);
    auto find_leftlowest = [](const auto& p1, const auto& p2) {
        return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
    };
    const auto& p0 = *sr::min_element(Q, find_leftlowest);
    auto angle_comparator = [&p0](const auto& p1, const auto& p2) {
        return Cross(p1 - p0, p2 - p0) > 0.0;
    };
    std::set<Point2d, decltype(angle_comparator)> P(angle_comparator);
    for (const auto& point : Q) {
        if (point == p0) {
            continue;
        }
        auto it = P.find(point); // does equivalent point exist
        if (it == P.end()) {
            P.insert(point);
        } else {
            auto dist_diff = dist(p0, point) - dist(p0, *it);
            if (dist_diff > 0.0) {
                P.erase(it);
                P.insert(point);
            }
        }
    }
    std::vector<Point2d> S;
    S.push_back(p0);
    S.push_back(*P.begin());
    P.erase(P.begin());
    S.push_back(*P.begin());
    P.erase(P.begin());
    for (const auto& p_i : P) {
        while (Cross(S[S.size() - 1] - S[S.size() - 2], p_i - S[S.size() - 2]) < 0.0) {
            S.pop_back();
        }
        if (isClose(Cross(S[S.size() - 1] - S[S.size() - 2], p_i - S[S.size() - 2]), 0.0)) {
            S.pop_back();
        }
        S.push_back(p_i);
    }
    return S;
}

std::vector<Point2d> JarvisMarch(const std::vector<Point2d>& Q) {
    assert(Q.size() >= 3);
    std::vector<int> contained(Q.size());
    auto find_leftlowest = [](const auto& p1, const auto& p2) {
        return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
    };
    auto p0_ptr = sr::min_element(Q, find_leftlowest);
    const auto& p_0 = *p0_ptr;
    std::vector<Point2d> S;
    S.push_back(p_0);
    contained[std::distance(Q.begin(), p0_ptr)] = 1;

    // in contrast to the approach in CLRS,
    // we don't ne ed to separately run building the right chain and the left chain of the convex hull.
    // a single pass is enough to guarantee constructing the correct convex hull.

    while (true) {
        const auto& p_i = S.back();
        auto point_selector = [&p_i](const auto& p1, const auto& p2) {
            auto cross = Cross(p1 - p_i, p2 - p_i);
            return cross > 0.0 || (isClose(cross, 0.0) && dist(p1, p_i) > dist(p2, p_i));
        };
        auto p_j_ptr = sr::min_element(Q, point_selector);
        const auto& p_j = *p_j_ptr;
        std::size_t j = std::distance(Q.begin(), p_j_ptr);
        if (contained[j]) {
            break;
        }
        S.push_back(p_j);
        contained[j] = 1;
    }
    return S;
}


int main() {
    std::vector<Point2d> Q {{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0},
                            {0.0, 1.0}, {1.0, 1.0}, {2.0, 1.0},
                            {0.0, 2.0}, {1.0, 2.0}, {2.0, 2.0}};

    std::cout << "Points:\n";
    for (const auto& [x, y] : Q) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    auto GS = GrahamScan(Q);

    std::cout << "Convex hull:\n";
    for (const auto& [x, y] : GS) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    auto JM = JarvisMarch(Q);

    std::cout << "Convex hull2:\n";
    for (const auto& [x, y] : JM) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_points = 30;

    Q.clear();

    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(dist(gen), dist(gen));
    }

    std::cout << "Points:\n";
    for (const auto& [x, y] : Q) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    GS = GrahamScan(Q);

    std::cout << "Convex hull:\n";
    for (const auto& [x, y] : GS) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    JM = JarvisMarch(Q);

    std::cout << "Convex hull2:\n";
    for (const auto& [x, y] : JM) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';



}