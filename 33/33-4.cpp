#include <algorithm>
#include <cassert>
#include <compare>
#include <cmath>
#include <iostream>
#include <list>
#include <random>
#include <optional>
#include <stack>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

constexpr double tolerance = 1e-6;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
}

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
    return isClose(lhs.x, rhs.x) && isClose(lhs.y, rhs.y);
}

bool operator!=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs == rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

struct Point3d {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    Point3d() = default;
    Point3d(double x, double y, double z) : x {x}, y {y}, z{z} {}
};

std::ostream& operator<<(std::ostream& os, const Point3d& point) {
    os << '{' << point.x << ", " << point.y << ", " << point.z << '}';
    return os;
}

Point3d operator+(const Point3d& lhs, const Point3d& rhs) {
    return Point3d(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Point3d operator-(const Point3d& lhs, const Point3d& rhs) {
    return Point3d(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

bool operator==(const Point3d& lhs, const Point3d& rhs) {
    return isClose(lhs.x, rhs.x) && isClose(lhs.y, rhs.y) && isClose(lhs.z, rhs.z);
}

bool operator!=(const Point3d& lhs, const Point3d& rhs) {
    return !(lhs == rhs);
}

double dist(const Point3d& lhs, const Point3d& rhs) {
    return std::hypot(std::hypot(lhs.x - rhs.x, lhs.y - rhs.y), lhs.z - rhs.z);
}

struct Segment2d {
    Point2d src;
    Point2d dst;
    Segment2d(const Point2d& src, const Point2d& dst)
            : src{src}, dst{dst} {
        assert(src != dst);
    }
};

std::ostream& operator<<(std::ostream& os, const Segment2d& segment) {
    os << '{' << segment.src << ',' << segment.dst << '}';
    return os;
}

bool operator==(const Segment2d& lhs, const Segment2d& rhs) {
    if (lhs.src == rhs.src) {
        return lhs.dst == rhs.dst;
    } else if (lhs.src == rhs.dst) {
        return lhs.dst == rhs.src;
    } else {
        return false;
    }
}

bool operator!=(const Segment2d& lhs, const Segment2d& rhs) {
    return !(lhs == rhs);
}

double Direction(const Point2d& p1, const Point2d& p2, const Point2d& p3) {
    return Cross(p3 - p1, p2 - p1);
}

bool OnSegment(const Point2d& p1, const Point2d& p2, const Point2d& p3) {
    if ((std::min(p1.x, p2.x) <= p3.x && p3.x <= std::max(p1.x, p2.x))
        && (std::min(p1.y, p2.y) <= p3.y && p3.y <= std::max(p1.y, p2.y))) {
        return true;
    } else {
        return false;
    }
}

Point2d FindIntersection(const Segment2d& s1, const Segment2d& s2) {
    const auto& p1 = s1.src;
    const auto& p2 = s1.dst;
    const auto& p3 = s2.src;
    const auto& p4 = s2.dst;
    auto dp1 = p2 - p1;
    auto dp2 = p4 - p3;

    if (isClose(dp1.x, 0.0)) {
        assert(!isClose(dp2.x, 0.0));
        double x_pos = p1.x;
        double y_pos = (dp2.y / dp2.x) * (x_pos - p3.x) + p3.y;
        return Point2d{x_pos, y_pos};
    } else if (isClose(dp2.x, 0.0)) {
        double x_pos = p3.x;
        double y_pos = (dp1.y / dp1.x) * (x_pos - p1.x) + p1.y;
        return Point2d{x_pos, y_pos};
    } else {
        double x_pos = (p3.x * dp2.y / dp2.x - p1.x * dp1.y / dp1.x - p3.y + p1.y) / (dp2.y / dp2.x - dp1.y / dp1.x);
        double y_pos = (dp2.y / dp2.x) * (x_pos - p3.x) + p3.y;
        return Point2d{x_pos, y_pos};
    }
}

std::optional<Point2d> SegmentsIntersect(const Segment2d& s1, const Segment2d& s2) {
    const auto& p1 = s1.src;
    const auto& p2 = s1.dst;
    const auto& p3 = s2.src;
    const auto& p4 = s2.dst;
    auto d1 = Direction(p3, p4, p1);
    auto d2 = Direction(p3, p4, p2);
    auto d3 = Direction(p1, p2, p3);
    auto d4 = Direction(p1, p2, p4);
    if (((d1 > tolerance && d2 < -tolerance) || (d1 < -tolerance && d2 > tolerance))
        && ((d3 > tolerance && d4 < -tolerance) || (d3 < -tolerance && d4 > tolerance))) {
        return FindIntersection(s1, s2);
    } else if (isClose(d1, 0.0) && OnSegment(p3, p4, p1)) {
        return p1;
    } else if (isClose(d2, 0.0) && OnSegment(p3, p4, p2)) {
        return p2;
    } else if (isClose(d3, 0.0) && OnSegment(p1, p2, p3)) {
        return p3;
    } else if (isClose(d4, 0.0) && OnSegment(p1, p2, p4)) {
        return p4;
    }
    return {};
}

struct Segment3d {
    Point3d src;
    Point3d dst;
    Segment3d(const Point3d& src, const Point3d& dst)
            : src{src}, dst{dst} {
        assert(src != dst);
    }
};

std::ostream& operator<<(std::ostream& os, const Segment3d& segment) {
    os << '{' << segment.src << ',' << segment.dst << '}';
    return os;
}

bool operator==(const Segment3d& lhs, const Segment3d& rhs) {
    if (lhs.src == rhs.src) {
        return lhs.dst == rhs.dst;
    } else if (lhs.src == rhs.dst) {
        return lhs.dst == rhs.src;
    } else {
        return false;
    }
}

bool operator!=(const Segment3d& lhs, const Segment3d& rhs) {
    return !(lhs == rhs);
}

auto isAbove(const Segment3d& lhs, const Segment3d& rhs) {
    Segment2d lhs_xy {Point2d{lhs.src.x, lhs.src.y},
                      Point2d{lhs.dst.x, lhs.dst.y}};
    Segment2d rhs_xy {Point2d{rhs.src.x, rhs.src.y},
                      Point2d{rhs.dst.x, rhs.dst.y}};
    auto intersect_xy = SegmentsIntersect(lhs_xy, rhs_xy);
    if (!intersect_xy.has_value()) {
        return std::partial_ordering::equivalent;
    }
    const auto& [x, y] = *intersect_xy;

    double alpha = 0.0;
    if (!isClose(lhs.dst.x, lhs.src.x)) {
        alpha = (x - lhs.src.x) / (lhs.dst.x - lhs.src.x);
    } else {
        alpha = (y - lhs.src.y) / (lhs.dst.y - lhs.src.y);
    }
    double beta = 0.0;
    if (!isClose(rhs.dst.x, rhs.src.x)) {
        beta = (x - rhs.src.x) / (rhs.dst.x - rhs.src.x);
    } else {
        beta = (y - rhs.src.y) / (rhs.dst.y - rhs.src.y);
    }
    double lhs_z = lhs.src.z + alpha * (lhs.dst.z - lhs.src.z);
    double rhs_z = rhs.src.z + beta * (rhs.dst.z - rhs.src.z);
    if (lhs_z - rhs_z > tolerance) {
        return std::partial_ordering::greater;
    } else if (rhs_z - lhs_z > tolerance) {
        return std::partial_ordering::less;
    } else {
        return std::partial_ordering::equivalent;
    }
}

class Graph {
    const std::size_t n;
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph(std::size_t n) : n {n}, adj(n) {}

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].push_back(dst);
    }

    enum class Color {
        White,
        Gray,
        Black,
    };

    bool DFSVisit(std::size_t u, std::vector<Color>& visited, std::list<std::size_t>& topSort) {
        if (visited[u] == Color::Black) {
            return false;
        } else if (visited[u] == Color::Gray) {
            return true;
        }
        visited[u] = Color::Gray;
        for (auto v : adj[u]) {
            auto res = DFSVisit(v, visited, topSort);
            if (res) {
                return true;
            }
        }
        visited[u] = Color::Black;
        topSort.push_front(u);
        return false;
    }

    std::list<std::size_t> TopologicalSort() {
        std::stack<std::size_t> S;
        std::list<std::size_t> topSort;
        std::vector<Color> visited (n, Color::White);

        for (std::size_t u = 0; u < n; u++) {
            if (visited[u] == Color::White) {
                auto res = DFSVisit(u, visited, topSort);
                if (res) {
                    return {};
                }
            }
        }
        return topSort;
    }
};

std::list<Segment3d> ComputePickingOrder(const std::vector<Segment3d>& sticks) {
    const std::size_t n = sticks.size();
    Graph g(n);
    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = i + 1; j < n; j++) {
            auto res = isAbove(sticks[i], sticks[j]);
            if (res == std::partial_ordering::greater) {
                g.addEdge(i, j);
            } else if (res == std::partial_ordering::less) {
                g.addEdge(j, i);
            }
        }
    }
    auto top_sort = g.TopologicalSort();
    std::list<Segment3d> picking_order;
    for (auto idx : top_sort) {
        picking_order.push_back(sticks[idx]);
    }
    return picking_order;
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::vector<Segment3d> sticks {
            {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
            {{1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}},
            {{1.0, 1.0, 2.0}, {0.0, 1.0, 2.0}},
            {{0.0, 1.0, 3.0}, {0.0, 0.0, 3.0}},
            {{0.0, 0.0, 4.0}, {1.0, 0.0, 4.0}},
    };
    sr::shuffle(sticks, gen);

    std::cout << "Sticks :\n";
    for (const auto& stick : sticks) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';

    auto picking_order = ComputePickingOrder(sticks);

    std::cout << "Picking order :\n";
    for (const auto& stick : picking_order) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';

    sticks.clear();
    sticks = std::vector<Segment3d> {
            {{0.0, 0.0, 0.0}, {1.0, 1.0, 3.0}},
            {{1.0, 1.0, 2.0}, {1.0, 0.0, 2.0}},
            {{1.0, 0.0, 1.0}, {0.0, 0.0, 1.0}},
    };

    std::cout << "Sticks :\n";
    for (const auto& stick : sticks) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';

    picking_order = ComputePickingOrder(sticks);

    std::cout << "Picking order :\n";
    for (const auto& stick : picking_order) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';

    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_segments = 10;

    sticks.clear();
    for (std::size_t i = 0; i < num_segments; i++) {
        sticks.emplace_back(Point3d{dist(gen), dist(gen), dist(gen)},
                            Point3d{dist(gen), dist(gen), dist(gen)});
    }

    std::cout << "Sticks :\n";
    for (const auto& stick : sticks) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';

    picking_order = ComputePickingOrder(sticks);

    std::cout << "Picking order :\n";
    for (const auto& stick : picking_order) {
        std::cout << stick << '\n';
    }
    std::cout << '\n';


}