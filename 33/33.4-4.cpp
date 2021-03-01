#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
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
    return std::max(std::abs(lhs.x - rhs.x), std::abs(lhs.y - rhs.y));
}

std::pair<Point2d, Point2d> ClosestPairOfPoints(const std::vector<Point2d>& X,
                                                const std::vector<Point2d>& Y) {
    if (X.size() == 2) {
        return {X[0], X[1]};
    } else if (X.size() == 3) {
        auto d1 = dist(X[0], X[1]);
        auto d2 = dist(X[0], X[2]);
        auto d3 = dist(X[1], X[2]);
        auto d = std::min({d1, d2, d3});
        if (isClose(d, d1)) {
            return {X[0], X[1]};
        } else if (isClose(d, d2)) {
            return {X[0], X[2]};
        } else {
            return {X[1], X[2]};
        }
    } else {
        std::vector<Point2d> X_L (X.begin(), X.begin() + X.size() / 2);
        std::vector<Point2d> X_R (X.begin() + X.size() / 2, X.end());
        double x_border = (X_L.back().x + X_R.front().x) / 2;
        std::vector<Point2d> Y_L;
        std::vector<Point2d> Y_R;
        for (const auto& y_pt : Y) {
            if (y_pt.x < x_border) {
                Y_L.push_back(y_pt);
            } else {
                Y_R.push_back(y_pt);
            }
        }
        auto [l_p1, l_p2] = ClosestPairOfPoints(X_L, Y_L);
        auto delta_L = dist(l_p1, l_p2);
        auto [r_p1, r_p2] = ClosestPairOfPoints(X_R, Y_R);
        auto delta_R = dist(r_p1, r_p2);
        double delta = std::min(delta_L, delta_R);
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
            return {l_p1, l_p2};
        } else if (isClose(delta_final, delta_R)) {
            return {r_p1, r_p2};
        } else {
            return {m_p1, m_p2};
        }
    }
}

std::pair<Point2d, Point2d> ClosestPairOfPoints(const std::vector<Point2d>& P) {
    assert(P.size() >= 2);
    auto X = P;
    auto Y = P;
    auto comp_by_x = [](const auto &p1, const auto &p2) {
        return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
    };
    auto comp_by_y = [](const auto &p1, const auto &p2) {
        return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
    };
    sr::sort(X, comp_by_x);
    sr::sort(Y, comp_by_y);
    return ClosestPairOfPoints(X, Y);
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
    for (const auto& [x, y] : Q) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    auto [p1, p2] = ClosestPairOfPoints(Q);
    std::cout << "Closest pair of points:\n";
    std::cout << '(' << p1.x << ", " << p1.y << "), ";
    std::cout << '(' << p2.x << ", " << p2.y << ")\n";

}