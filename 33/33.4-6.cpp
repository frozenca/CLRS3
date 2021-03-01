#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

std::ostream& operator<<(std::ostream& os, const Point2d& point) {
    os << '{' << point.x << ", " << point.y << '}';
    return os;
}

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

auto comp_by_y = [](const auto &p1, const auto &p2) {
    return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
};

std::tuple<Point2d, Point2d, std::vector<Point2d>> ClosestPairOfPoints(const std::vector<Point2d>& X) {
    if (X.size() == 2) {
        auto Y = X;
        sr::sort(Y, comp_by_y);
        return {X[0], X[1], Y};
    } else if (X.size() == 3) {
        auto d1 = dist(X[0], X[1]);
        auto d2 = dist(X[0], X[2]);
        auto d3 = dist(X[1], X[2]);
        auto d = std::min({d1, d2, d3});
        auto Y = X;

        sr::sort(Y, comp_by_y);
        if (isClose(d, d1)) {
            return {X[0], X[1], Y};
        } else if (isClose(d, d2)) {
            return {X[0], X[2], Y};
        } else {
            return {X[1], X[2], Y};
        }
    } else {
        std::vector<Point2d> X_L (X.begin(), X.begin() + X.size() / 2);
        std::vector<Point2d> X_R (X.begin() + X.size() / 2, X.end());
        double x_border = (X_L.back().x + X_R.front().x) / 2;
        auto [l_p1, l_p2, Y_L] = ClosestPairOfPoints(X_L);
        auto delta_L = dist(l_p1, l_p2);
        auto [r_p1, r_p2, Y_R] = ClosestPairOfPoints(X_R);
        auto delta_R = dist(r_p1, r_p2);
        double delta = std::min(delta_L, delta_R);

        std::vector<Point2d> Y;
        Y.reserve(Y_L.size() + Y_R.size());
        sr::merge(Y_L, Y_R, std::back_inserter(Y), comp_by_y);

        std::vector<Point2d> Mid;
        for (const auto& y_pt : Y) {
            if (y_pt.x >= x_border - delta && y_pt.x <= x_border + delta) {
                Mid.push_back(y_pt);
            }
        }
        double delta_mid = delta;

        Point2d m_p1;
        Point2d m_p2;
        for (std::size_t i = 0; i < Mid.size(); i++) {
            for (std::size_t j = i + 1; j < std::min(i + 8, Mid.size()) && Mid[j].y <= Mid[i].y + delta; j++) {
                auto curr_dist = dist(Mid[i], Mid[j]);
                if (curr_dist < delta_mid) {
                    m_p1 = Mid[i];
                    m_p2 = Mid[j];
                    delta_mid = curr_dist;
                }
            }
        }
        double delta_final = std::min(delta_mid, delta);
        if (isClose(delta_final, delta_L)) {
            return {l_p1, l_p2, Y};
        } else if (isClose(delta_final, delta_R)) {
            return {r_p1, r_p2, Y};
        } else {
            return {m_p1, m_p2, Y};
        }
    }
}

std::pair<Point2d, Point2d> FindClosestPairOfPoints(const std::vector<Point2d>& P) {
    assert(P.size() >= 2);
    auto X = P;
    auto comp_by_x = [](const auto &p1, const auto &p2) {
        return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
    };
    sr::sort(X, comp_by_x);
    auto [p1, p2, _] = ClosestPairOfPoints(X);
    return {p1, p2};
}

int main() {
    std::vector<Point2d> Q;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_points = 30;

    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(dist(gen), dist(gen));
    }

    std::cout << "Points:\n";
    for (const auto& p : Q) {
        std::cout << p << '\n';
    }
    std::cout << '\n';

    auto [p1, p2] = FindClosestPairOfPoints(Q);
    std::cout << "Closest pair of points:\n";
    std::cout << p1 << ' ' << p2 << '\n';

}