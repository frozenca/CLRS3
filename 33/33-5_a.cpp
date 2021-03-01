#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
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

double Dot(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

static const Point2d orig {0.0, 0.0};

double dist(const Point2d& lhs, const Point2d& rhs = orig) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

double Cos(const Point2d& p1, const Point2d& p2) {
    assert(p1 != orig && p2 != orig);
    auto dot = Dot(p1, p2);
    auto r = dist(p1) * dist(p2);
    return dot / r;
}

double Orientation(const Point2d& p0, const Point2d& p1, const Point2d& p2) {
    auto cross = Cross(p1 - p0, p2 - p0);
    return cross;
}

enum class Order {
    Before,
    EqualSrc,
    Between,
    EqualDst,
    After,
};

Order getOrder(const Point2d& p1, const Point2d& p2, const Point2d& q) {
    assert(p1 != p2);
    if (q == p1) {
        return Order::EqualSrc;
    } else if (q == p2) {
        return Order::EqualDst;
    }
    if (!isClose(p1.x, p2.x)) {
        if (p2.x - p1.x > tolerance) { // p1 - p2
            if (p1.x - q.x > tolerance) { // q - p1 - p2
                return Order::Before;
            } else if (p2.x - q.x > tolerance) { // p1 - q - p2
                return Order::Between;
            } else { // p1 - p2 - q
                return Order::After;
            }
        } else { // p2 - p1
            if (q.x - p1.x > tolerance) { // p2 - p1 - q
                return Order::Before;
            } else if (q.x - p2.x > tolerance) { // p2 - q - p1
                return Order::Between;
            } else { // q - p2 - p1
                return Order::After;
            }
        }
    } else {
        if (p2.y - p1.y > tolerance) { // p1 - p2
            if (p1.y - q.y > tolerance) { // q - p1 - p2
                return Order::Before;
            } else if (p2.y - q.y > tolerance) { // p1 - q - p2
                return Order::Between;
            } else { // p1 - p2 - q
                return Order::After;
            }
        } else { // p2 - p1
            if (q.y - p1.y > tolerance) { // p2 - p1 - q
                return Order::Before;
            } else if (q.y - p2.y > tolerance) { // p2 - q - p1
                return Order::Between;
            } else { // q - p2 - p1
                return Order::After;
            }
        }
    }
}
std::vector<Point2d> ConvexHullAdd(std::vector<Point2d>& CH, const Point2d& p) {
    if (CH.empty()) {
        CH.push_back(p);
        return CH;
    } else if (CH.size() == 1) {
        if (CH[0] != p) {
            CH.push_back(p);
        }
        return CH;
    } else if (CH.size() == 2) {
        const auto& p0 = CH[0];
        const auto& p1 = CH[1];
        auto cross = Cross(p1 - p0, p - p0);
        if (isClose(cross, 0.0)) { // colinear
            auto d1 = dist(p0, p);
            auto d2 = dist(p1, p);
            auto d3 = dist(p0, p1);
            auto d = std::max({d1, d2, d3});
            if (isClose(d, d1)) {
                return std::vector<Point2d>{p0, p};
            } else if (isClose(d, d2)) {
                return std::vector<Point2d>{p1, p};
            } else {
                return std::vector<Point2d>{p0, p1};
            }
        } else if (cross > tolerance) { // left turn
            return std::vector<Point2d>{p0, p, p1};
        } else { // right turn
            return std::vector<Point2d>{p0, p1, p};
        }
    }

    // make clockwise convex hull
    const std::size_t m = CH.size();
    std::size_t i = 0;
    // while right turn
    while (true) {
        if (i >= m) {
            break;
        }
        auto cross = Cross(p - CH[i % m], CH[(i + 1) % m] - CH[i % m]);
        if (isClose(cross, 0.0)) {
            // special case: colinear
            auto ord = getOrder(CH[i % m], CH[(i + 1) % m], p);
            if (ord == Order::Before) {
                CH[i % m] = p;
            } else if (ord == Order::After) {
                CH[(i + 1) % m] = p;
            }
            return CH;
        } else if (cross > tolerance) {
            i++;
        } else {
            break;
        }
    }
    if (i == m) { // interior point
        return CH;
    }
    if (i == 0) {
        // first turn is left, turn back to where left turn begins
        while (true) {
            auto cross = Cross(p - CH[i % m], CH[(i + 1) % m] - CH[i % m]);
            if (isClose(cross, 0.0)) {
                // special case: colinear
                auto ord = getOrder(CH[i % m], CH[(i + 1) % m], p);
                if (ord == Order::Before) {
                    CH[i % m] = p;
                } else if (ord == Order::After) {
                    CH[(i + 1) % m] = p;
                }
                return CH;
            } else if (cross < -tolerance) {
                i = (i + m - 1) % m;
            } else {
                break;
            }
        }
        i = (i + 1) % m;
    }
    std::size_t j = (i + 1) % m;
    // while left turn
    while (true) {
        auto cross = Cross(p - CH[j % m], CH[(j + 1) % m] - CH[j % m]);
        if (isClose(cross, 0.0)) {
            // special case: colinear
            auto ord = getOrder(CH[j % m], CH[(j + 1) % m], p);
            if (ord == Order::Before) {
                CH[j % m] = p;
            } else if (ord == Order::After) {
                CH[(j + 1) % m] = p;
            }
            return CH;
        } else if (cross < -tolerance) {
            j = (j + 1) % m;
        } else {
            break;
        }
    }

    // replace p_(i, j) with p
    if (j < i) {
        CH.erase(CH.begin() + i + 1, CH.end());
        CH.push_back(p);
        CH.erase(CH.begin(), CH.begin() + j);
    } else if (j > i) {
        CH.erase(CH.begin() + i + 1, CH.begin() + j);
        CH.insert(CH.begin() + i + 1, p);
    } else { // cannot happen
        assert(0);
    }
    return CH;
}

using ColinearPair = std::pair<std::optional<Point2d>, std::optional<Point2d>>;

void UpdateColinearPair(ColinearPair& curr_minmax, const Point2d& new_min, const Point2d& new_max) {
    auto& [curr_min, curr_max] = curr_minmax;
    if (curr_min.has_value()) {
        auto ord = getOrder(new_min, new_max, *curr_min);
        if (ord != Order::Before) {
            curr_min = new_min;
        }
    } else {
        curr_min = new_min;
    }
    if (curr_max.has_value()) {
        auto ord = getOrder(new_min, new_max, *curr_max);
        if (ord != Order::After) {
            curr_max = new_max;
        }
    } else {
        curr_max = new_max;
    }
}

void HandleColinearCase(ColinearPair& curr_minmax, const Point2d& p1, const Point2d& p2, const Point2d& q) {
    auto ord = getOrder(p1, p2, q);
    if (ord == Order::Before || ord == Order::EqualSrc) { // (q, p2)
        UpdateColinearPair(curr_minmax, q, p2);
    } else if (ord == Order::Between) { // (p1, p2)
        UpdateColinearPair(curr_minmax, p1, p2);
    } else if (ord == Order::EqualDst || ord == Order::After) { // (p1, q)
        UpdateColinearPair(curr_minmax, p1, q);
    } else { // cannot happen
        assert(0);
    }
}

std::vector<Point2d> MergeTwoConvexHulls(const std::vector<Point2d>& CH1, const std::vector<Point2d>& CH2) {
    if (std::min(CH1.size(), CH2.size()) <= 2) { // base case: just do online update.
        if (CH1.size() < CH2.size()) {
            auto CH = CH2;
            for (const auto& p : CH1) {
                CH = ConvexHullAdd(CH, p);
            }
            return CH;
        } else {
            auto CH = CH1;
            for (const auto& p : CH2) {
                CH = ConvexHullAdd(CH, p);
            }
            return CH;
        }
    }
    const std::size_t m = CH1.size();
    const std::size_t n = CH2.size();
    std::size_t start1 = std::distance(CH1.begin(), sr::min_element(CH1));
    std::size_t start2 = std::distance(CH2.begin(), sr::min_element(CH2));

    std::size_t i = start1;
    std::size_t j = start2;

    // compare angle with vertical axis to decide first edge vs vertex pair
    enum class Edge {
        CH1,
        CH2,
    };

    Edge currEdge = (Cos(Point2d{0.0, 1.0}, CH1[(i + 1) % m] - CH1[i % m])
            >= Cos(Point2d{0.0, 1.0}, CH2[(j + 1) % n] - CH2[j % n]))
                    ? Edge::CH1 : Edge::CH2;

    std::vector<Point2d> CH;

    ColinearPair colinear_minmax;

    while (i < start1 + m || j < start2 + n) {
        const auto& p_i = CH1[i % m];
        const auto& q_j = CH2[j % n];
        const auto& p_i1 = CH1[(i + 1) % m];
        const auto& q_j1 = CH2[(j + 1) % n];
        if (currEdge == Edge::CH1) {
            // compare p_i-p_(i+1) vs q_j
            auto dir = Orientation(p_i, p_i1, q_j);
            if (dir > tolerance) { // q_j is in the left side: add q_j
                CH.push_back(q_j);
            } else if (dir < -tolerance) { // q_j is in the right side: add p_i, p_(i+1)
                CH.push_back(p_i);
                CH.push_back(p_i1);
            } else { // colinear: determine the order
                HandleColinearCase(colinear_minmax, p_i, p_i1, q_j);
            }

            // choose one to advance
            auto curr_edge = p_i1 - p_i;
            const auto& p_i2 = CH1[(i + 2) % m];
            auto next_edge_p = p_i2 - p_i1;
            auto next_edge_q = q_j1 - q_j;

            auto cos_p = Cos(curr_edge, next_edge_p);
            auto cos_q = Cos(curr_edge, next_edge_q);
            if (cos_p - cos_q > 0.0) { // advance p
                i++;
            } else { // advance q
                currEdge = Edge::CH2;
                i++;
            }
            if (isClose(std::max(cos_p, cos_q), 1.0)
                && colinear_minmax.first.has_value()) { // next edge is parallel with curr edge, and in fact colinear
                // do nothing, keep colinear pairs
            } else if (colinear_minmax.first.has_value()) {
                // add this colinear pair to CH
                CH.push_back(*colinear_minmax.first);
                CH.push_back(*colinear_minmax.second);
                colinear_minmax = {{}, {}}; // discard colinear pair
            }
        } else if (currEdge == Edge::CH2) {
            // compare q_j-q_(j+1) vs p_i
            auto dir = Orientation(q_j, q_j1, p_i);
            if (dir > tolerance) { // p_i is in the left side: add p_i
                CH.push_back(p_i);
            } else if (dir < -tolerance) { // p_i is in the right side: add q_j, q_(j+1)
                CH.push_back(q_j);
                CH.push_back(q_j1);
            } else { // colinear: determine the order
                HandleColinearCase(colinear_minmax, q_j, q_j1, p_i);
            }

            // choose one to advance
            auto curr_edge = q_j1 - q_j;
            const auto& q_j2 = CH2[(j + 2) % n];
            auto next_edge_p = p_i1 - p_i;
            auto next_edge_q = q_j2 - q_j1;

            auto cos_p = Cos(curr_edge, next_edge_p);
            auto cos_q = Cos(curr_edge, next_edge_q);
            if (cos_q - cos_p > 0.0) { // advance q
                j++;
            } else { // advance p
                currEdge = Edge::CH1;
                j++;
            }
            if (isClose(std::max(cos_p, cos_q), 1.0)
                && colinear_minmax.first.has_value()) { // next edge is parallel with curr edge, and in fact colinear
                // do nothing, keep colinear pairs
            } else if (colinear_minmax.first.has_value()) {
                // add this colinear pair to CH
                CH.push_back(*colinear_minmax.first);
                CH.push_back(*colinear_minmax.second);
                colinear_minmax = {{}, {}}; // discard colinear pair
            }
        } else { // cannot happen
            assert(0);
        }
    }
    CH.erase(std::unique(CH.begin(), CH.end()), CH.end());
    if (CH.back() == CH.front()) {
        CH.pop_back();
    }
    return CH;
}

int main() {

    // Test case 1: ◇□
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {2.0, 2.0},
                                 {4.0, 0.0},
                                 {2.0, -2.0}};
        std::vector<Point2d> CH2{{5.0, -1.0},
                                 {5.0, 1.0},
                                 {7.0, 1.0},
                                 {7.0, -1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 2: □ □
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{4.0, 0.0},
                                 {4.0, 2.0},
                                 {6.0, 2.0},
                                 {6.0, 0.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 3: □◇ overlapping
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{-0.5, 1.0},
                                 {1.0, 2.5},
                                 {2.5, 1.0},
                                 {1.0, -0.5}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 4: □ inscribes ◇
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{-1.0, 1.0},
                                 {1.0, 3.0},
                                 {3.0, 1.0},
                                 {1.0, -1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 5: □□ overlapping
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{1.0, 0.0},
                                 {1.0, 2.0},
                                 {3.0, 2.0},
                                 {3.0, 0.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 6: □ □ meets at corner
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{2.0, 2.0},
                                 {2.0, 4.0},
                                 {4.0, 4.0},
                                 {4.0, 2.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 7: □ contains □, two □ intersects at edges
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{0.0, 0.0},
                                 {0.0, 1.0},
                                 {1.0, 1.0},
                                 {1.0, 0.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 8: □ contains □ with no intersection
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{0.5, 0.5},
                                 {0.5, 1.5},
                                 {1.5, 1.5},
                                 {1.5, 0.5}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 9: □ and interior point
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{1.0, 1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 10: □ and exterior point
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{3.0, 1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 11: □ and boundary point
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{2.0, 1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 12: □ and exterior point colinear
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{2.0, 3.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 13: □ and exterior point colinear 2
    {
        std::vector<Point2d> CH1{{0.0, 0.0},
                                 {0.0, 2.0},
                                 {2.0, 2.0},
                                 {2.0, 0.0}};
        std::vector<Point2d> CH2{{2.0, -1.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

    // Test case 14: regular octagon and ▷ that intersects at 4 points
    {
        auto sq2 = std::sqrt(2);
        std::vector<Point2d> CH1{{-2.0, 0.0},
                                 {-sq2, sq2},
                                 {0.0, 2.0},
                                 {sq2, sq2},
                                 {2.0, 0.0},
                                 {sq2, -sq2},
                                 {0.0, -2.0},
                                 {-sq2, -sq2}};
        std::vector<Point2d> CH2{{-2.0, 4.0 - 4.0 * sq2},
                                 {-2.0, -4.0 + 4.0 * sq2},
                                 {2.0, 0.0}};

        auto CH = MergeTwoConvexHulls(CH1, CH2);
        std::cout << "Convex hull:\n";
        for (const auto& p : CH) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

}