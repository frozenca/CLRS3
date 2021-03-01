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

constexpr double tolerance = 1e-6;

std::ostream& operator<<(std::ostream& os, const Point2d& point) {
    os << '{' << point.x << ", " << point.y << '}';
    return os;
}

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
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

bool operator<(const Point2d& lhs, const Point2d& rhs) {
    return rhs.x - lhs.x > tolerance ||
           (isClose(lhs.x, rhs.x) && rhs.y - lhs.y > tolerance);
}

bool operator>=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs < rhs);
}

bool operator>(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x - rhs.x > tolerance ||
           (isClose(lhs.x, rhs.x) && lhs.y - rhs.y > tolerance);
}

bool operator<=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs > rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

std::vector<Point2d> JarvisMarch(std::vector<Point2d>& Q) {
    if (Q.size() <= 2) {
        auto Q_ = Q;
        Q.clear();
        return Q_;
    }
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
    // we don't need to separately run building the right chain and the left chain of the convex hull.
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
    std::vector<Point2d> Q_;
    for (std::size_t i = 0; i < Q.size(); i++) {
        if (!contained[i]) {
            Q_.push_back(Q[i]);
        }
    }
    std::swap(Q, Q_);
    return S;
}

std::vector<std::vector<Point2d>> ConvexLayers(const std::vector<Point2d>& Q_) {
    std::vector<std::vector<Point2d>> layers;
    auto Q = Q_;
    while (true) {
        auto JM = JarvisMarch(Q);
        if (JM.empty()) {
            break;
        } else {
            layers.emplace_back(JM);
        }
    }
    return layers;
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

    auto CL = ConvexLayers(Q);
    std::cout << "Convex layers:\n";
    for (const auto& layer : CL) {
        std::cout << "Points:\n";
        for (const auto& p : layer) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }



}