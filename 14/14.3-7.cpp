#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <tuple>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

struct IntervalTree {
private:
    enum class Color {
        Red,
        Black
    };

    struct Node {
        double begin;
        double end;
        Color color;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent;
        double maximum;

        Node(double begin, double end) : begin {begin}, end {end}, color {Color::Red}, parent {nullptr}, maximum {end} {}
    };

public:
    std::unique_ptr<Node> root;

private:
    void LeftRotate(std::unique_ptr<Node>&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        if (x->right) {
            x->right->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        Node* px = nullptr;
        Node* py = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node>(px);
            root->left->parent = root.get();
            py = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node>(px);
            xp->left->left->parent = xp->left.get();
            py = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->left = std::unique_ptr<Node>(px);
            xp->right->left->parent = xp->right.get();
            py = xp->right.get();
        }
        px->maximum = std::max({px->end, px->left ? px->left->maximum : std::numeric_limits<double>::lowest(),
                                px->right ? px->right->maximum : std::numeric_limits<double>::lowest()});
        py->maximum = std::max({py->end, py->left ? py->left->maximum : std::numeric_limits<double>::lowest(),
                                py->right ? py->right->maximum : std::numeric_limits<double>::lowest()});
    }

    void RightRotate(std::unique_ptr<Node>&& x) {
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        if (x->left) {
            x->left->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        Node* px = nullptr;
        Node* py = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node>(px);
            root->right->parent = root.get();
            py = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node>(px);
            xp->left->right->parent = xp->left.get();
            py = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->right = std::unique_ptr<Node>(px);
            xp->right->right->parent = xp->right.get();
            py = xp->right.get();
        }
        px->maximum = std::max({px->end, px->left ? px->left->maximum : std::numeric_limits<double>::lowest(),
                                px->right ? px->right->maximum : std::numeric_limits<double>::lowest()});
        py->maximum = std::max({py->end, py->left ? py->left->maximum : std::numeric_limits<double>::lowest(),
                                py->right ? py->right->maximum : std::numeric_limits<double>::lowest()});
    }

    void LeftRotate(Node* x) {
        auto xp = x->parent;
        if (!xp) {
            LeftRotate(std::move(root));
        } else if (x == xp->left.get()) {
            LeftRotate(std::move(xp->left));
        } else {
            LeftRotate(std::move(xp->right));
        }
    }

    void RightRotate(Node* x) {
        auto xp = x->parent;
        if (!xp) {
            RightRotate(std::move(root));
        } else if (x == xp->left.get()) {
            RightRotate(std::move(xp->left));
        } else {
            RightRotate(std::move(xp->right));
        }
    }

public:
    [[nodiscard]] Node* Search(double begin, double end) const {
        assert(begin <= end);
        auto x = root.get();
        while (x && (x->begin > end || x->end < begin)) {
            if (x->left && x->left->end >= begin) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        return x;
    }

    void Insert(double begin, double end) {
        assert(begin <= end);
        auto z = std::make_unique<Node>(begin, end);
        Insert(std::move(z));
    }

    void Delete(double begin, double end) {
        assert(begin <= end);
        auto z = Search(begin, end);
        Delete(z);
    }

private:
    [[nodiscard]] Node* Minimum(Node* x) const {
        if (!x) {
            return x;
        }
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    void Insert(std::unique_ptr<Node> z) {
        Node* y = nullptr;
        Node* x = root.get();
        while (x) {
            y = x;
            if (z->begin < x->begin || (z->begin == x->begin && z->end < x->end)) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        z->parent = y;
        if (!y) {
            root = std::move(z);
            InsertFixup(root.get());
        } else if (z->begin < y->begin || (z->begin == y->begin && z->end < y->end)) {
            y->maximum = std::max(y->maximum, z->end);
            y->left = std::move(z);
            InsertFixup(y->left.get());
        } else {
            y->maximum = std::max(y->maximum, z->end);
            y->right = std::move(z);
            InsertFixup(y->right.get());
        }
    }

    void InsertFixup(Node* z) {
        while (z->parent && z->parent->color == Color::Red) {
            if (z->parent == z->parent->parent->left.get()) {
                auto y = z->parent->parent->right.get();
                if (y && y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right.get()) {
                        z = z->parent;
                        LeftRotate(z);
                    }
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    RightRotate(z->parent->parent);
                }
            } else {
                auto y = z->parent->parent->left.get();
                if (y && y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left.get()) {
                        z = z->parent;
                        RightRotate(z);
                    }
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    LeftRotate(z->parent->parent);
                }
            }
        }
        root->color = Color::Black;
    }

    Node* Transplant(Node* u, std::unique_ptr<Node>&& v) {
        if (v) {
            v->parent = u->parent;
        }
        Node* w = nullptr;
        if (!u->parent) {
            w = root.release();
            root = std::move(v);
        } else if (u == u->parent->left.get()) {
            w = u->parent->left.release();
            u->parent->left = std::move(v);
        } else {
            w = u->parent->right.release();
            u->parent->right = std::move(v);
        }
        return w;
    }

    void Delete(Node* z) {
        if (!z) {
            return;
        }
        Color orig_color = z->color;
        Node* x = nullptr;
        Node* xp = nullptr;
        if (!z->left) {
            x = z->right.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->right));
            auto upz = std::unique_ptr<Node>(pz);
        } else if (!z->right) {
            x = z->left.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->left));
            auto upz = std::unique_ptr<Node>(pz);
        } else {
            auto y = Minimum(z->right.get());
            orig_color = y->color;
            x = y->right.get();
            if (y->parent == z) {
                if (x) {
                    x->parent = y;
                }
                xp = y;
                auto pz = Transplant(z, std::move(z->right));
                y->left = std::move(pz->left);
                y->left->parent = y;
                y->color = pz->color;
                auto upz = std::unique_ptr<Node>(pz);
            } else {
                xp = y->parent;
                auto py = Transplant(y, std::move(y->right));
                py->right = std::move(z->right);
                py->right->parent = py;
                auto upy = std::unique_ptr<Node>(py);
                auto pz = Transplant(z, std::move(upy));
                py->left = std::move(pz->left);
                py->left->parent = py;
                py->color = pz->color;
                auto upz = std::unique_ptr<Node>(pz);
            }
        }
        if (orig_color == Color::Black) {
            DeleteFixup(x, xp);
        }

    }

    void DeleteFixup(Node* x, Node* xp) {
        while (x != root.get() && (!x || x->color == Color::Black)) {
            if (x == xp->left.get()) {
                Node* w = xp->right.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    LeftRotate(xp);
                    w = xp->right.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->right || w->right->color == Color::Black) {
                        w->left->color = Color::Black;
                        w->color = Color::Red;
                        RightRotate(w);
                        w = xp->right.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->right->color = Color::Black;
                    LeftRotate(xp);
                    x = root.get();
                } else {
                    x = root.get();
                }
            } else {
                Node* w = xp->left.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    RightRotate(xp);
                    w = xp->left.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->left || w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color = Color::Red;
                        LeftRotate(w);
                        w = xp->left.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->left->color = Color::Black;
                    RightRotate(xp);
                    x = root.get();
                } else {
                    x = root.get();
                }
            }
        }
        if (x) {
            x->color = Color::Black;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, Node* node) {
        if (node) {
            os << node->left.get();
            os << '(' << node->begin << ", " << node->end << "):" << node->maximum;
            if (node->color == Color::Black) {
                os << "● ";
            } else {
                os << "○ ";
            }
            os << node->right.get();
        }
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const IntervalTree& tree) {
        os << std::setprecision(3) << tree.root.get();
        return os;
    }
};

using Rectangle = std::tuple<double, double, double, double>;

bool overlappingExist(std::vector<Rectangle>& rectangles) {
    std::vector<std::tuple<double, double, double, bool>> y_intervals;
    for (const auto& [x_b, x_e, y_b, y_e] : rectangles) {
        y_intervals.emplace_back(x_b, y_b, y_e, true);
        y_intervals.emplace_back(x_e, y_b, y_e, false);
    }
    sr::sort(y_intervals);

    IntervalTree tree;
    for (const auto& [x, y_b, y_e, left] : y_intervals) {
        if (left) {
            if (tree.Search(y_b, y_e)) {
                return true;
            }
            tree.Insert(y_b, y_e);
        } else {
            tree.Delete(y_b, y_e);
        }
    }
    return false;
}

int main() {
    constexpr size_t SIZE = 100;
    std::uniform_real_distribution<> dist(0.0, 1.0);
    std::vector<Rectangle> rectangles;
    for (size_t i = 0; i < SIZE; i++) {
        auto x_begin = dist(gen);
        auto x_end = dist(gen);
        auto y_begin = dist(gen);
        auto y_end = dist(gen);
        if (x_begin > x_end) std::swap(x_begin, x_end);
        if (y_begin > y_end) std::swap(y_begin, y_end);
        rectangles.emplace_back(x_begin, x_end, y_begin, y_end);
    }
    std::cout << overlappingExist(rectangles);

}