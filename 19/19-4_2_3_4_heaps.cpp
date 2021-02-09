#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
concept Key = std::is_arithmetic_v<T>;

template <Key T>
class BHeap {
    class Node {
        Node* parent = nullptr;
        std::size_t index = 0;
        std::size_t height = 1;
        T small = std::numeric_limits<T>::max();
        std::optional<T> key;
        std::vector<std::unique_ptr<Node>> child;

        friend class BHeap;

        void validateChild() {
            if (child.empty()) {
                assert(key.has_value());
                curr->small = key;
            } else {
                curr->small = child[0]->small;
                for (std::size_t i = 0; i < child.size(); i++) {
                    child[i]->index = i;
                    child[i]->parent = this;
                    curr->small = std::min(child[i]->small, curr->small);
                }
            }
        }

        void Merge(std::size_t i) noexcept {
            child[i]->child.resize(3);
            sr::move(child[i + 1]->child, child[i]->child.begin() + 1);
            std::shift_left(child.begin() + i + 1, child.end(), 1);
            child.pop_back();
            validateChild();
            child[i]->validateChild();
        }

        void LeftRotate() noexcept {
            assert(index + 1 < parent->child.size());
            auto sibling = parent->child[index + 1].get();
            child.push_back(std::move(sibling->child[0]));
            std::shift_left(sibling->child.begin(), sibling->child.end(), 1);
            sibling->child.pop_back();
            validateChild();
            sibling->validateChild();
        }

        void RightRotate() noexcept {
            assert(index - 1 < parent->child.size());
            auto sibling = parent->child[index - 1].get();
            child.resize(child.size() + 1);
            std::shift_right(child.begin(), child.end(), 1);
            child[0] = std::move(sibling->child[sibling->child.size() - 1]);
            sibling->child.pop_back();
            validateChild();
            sibling->validateChild();
        }
    };

    std::unique_ptr<Node> root;

    [[nodiscard]] const Node* Minimum(const Node* node) const {
        if (node->child.empty()) {
            return node;
        } else {
            Node* next = node->child[0].get();
            for (auto&& child : node->child) {
                if (child->small < next->small) {
                    next = child.get();
                }
            }
            return Minimum(next);
        }
    }

    static void SplitChild(Node* x, std::size_t i) noexcept {
        if (!x) {
            return;
        }
        auto y = x->child[i].get();
        if (!y) {
            return;
        }
        // x cannot be full, because in that case its parent should've called SplitChild on x before
        assert(x->child.size() != 4 && y->child.size() == 4);
        auto z = std::make_unique<Node>(); // will be y's right sibling
        z->height = y->height;
        z->child.resize(2);
        sr::move(y->child | srv::drop(2), z->child.begin());
        z->validateChild();
        y->child.resize(2);
        x->child.resize(x->child.size() + 1);
        // shift children of x right by 1 from i + 1
        std::shift_right(x->child.begin() + i + 1, x->child.end(), 1);
        x->child[i + 1] = std::move(z);
        x->validateChild();
        y->validateChild();
    }

    void InsertNonFull(Node* x, const T& k) {
        if (x->child[0]->child.empty()) {
            x->child.push_back(std::make_unique<Node>());
            x->child.back()->key = k;
            auto curr = x->child.back().get();
            while (curr) {
                curr->small = std::min(curr->small, k);
                curr = curr->parent;
            }
        } else {
            if (x->child[0]->child.size() == 4) { // is full? then split
                SplitChild(x, 0);
            }
            InsertNonFull(x->child[0].get(), k); // recursively insert
        }
    }

public:
    BHeap() : root {std::make_unique<Node>()} {}

    [[nodiscard]] const Node* Minimum() const {
        return Minimum(root.get());
    }

    void DecreaseKey(Node* x, const T& k) {
        if (!x->key.has_value() || x->key < k) {
            return;
        }
        x->key = k;
        while (x) {
            x->small = std::min(x->small, k);
            x = x->parent;
        }
    }

    void Insert(const T& k) noexcept {
        if (root->child.size() == 4) {
            auto s = std::make_unique<Node>();
            s->height = root->height + 1;
            s->child.push_back(std::move(root));
            root = std::move(s);
            SplitChild(root.get(), 0);
            InsertNonFull(root.get(), k);
        } else if (root->child.empty()) {
            root->child.push_back(std::make_unique<Node>());
            root->child[0]->key = k;
        } else {
            InsertNonFull(root.get(), k);
        }
    }

    void Delete(Node* x) noexcept {
        Node* curr = x->parent;
        std::shift_left(x->parent->child.begin() + x->index, x->parent->child.end(), 1);
        curr->child.pop_back();
        curr->validateChild();
        while (curr && curr->parent && curr->child.size() == 1) {
            std::size_t i = curr->index;
            if (i + 1 < curr->parent->child.size() && curr->parent->child[i + 1]->child.size() > 2) {
                curr->LeftRotate();
            } else if (i - 1 < curr->parent->child.size() && curr->parent->child[i - 1]->child.size() > 2) {
                curr->RightRotate();
            } else if (i + 1 < curr->parent->child.size()) {
                curr->parent->Merge(i);
                if (curr->parent == root.get() && curr->parent->child.size() == 1) {
                    root = std::move(curr->parent->child[i]);
                    root->parent = nullptr;
                }
            } else if (i - 1 < curr->parent->child.size()) {
                curr->parent->Merge(i - 1);
                if (curr->parent == root.get() && curr->parent->child.size() == 1) {
                    root = std::move(curr->parent->child[i - 1]);
                    root->parent = nullptr;
                }
            }
            curr = curr->parent;
        }
    }

    void ExtractMin() {
        auto x = const_cast<Node*>(Minimum());
        Delete(x);
    }

    [[nodiscard]] std::size_t getHeight() const {
        return root->height;
    }

    void Union(BHeap& other_heap) {
        if (getHeight() < other_heap.getHeight()) {
            std::swap(root, other_heap.root);
        }
        if (getHeight() == other_heap.getHeight()) {
            sr::move(other_heap.root->child, std::back_inserter(root->child));
            root->validateChild();
            std::size_t curN = root->child.size();
            if (curN > 4) {
                auto s = std::make_unique<Node>();
                s->height = tree1.root->height + 1;
                s->child.push_back(std::move(root));
                root = std::move(s);
                std::size_t rightN = (curN + 1) / 2;
                auto z = std::make_unique<Node>(); // will be y's right sibling
                Node* y = root->child[0].get();
                z->height = y->height;
                sr::move(y->child | srv::drop(curN - rightN), std::back_inserter(z->child));
                z->validateChild();
                Node* p = root.get();
                p->child.push_back(std::move(z));
                y->child.resize(curN - rightN);
                p->validateChild();
                y->validateChild();
            }
        } else {
            Node* r1 = root.get();
            for (std::size_t i = 0; i < getHeight() - other_heap.getHeight(); i++) {
                r1 = r1->child.back().get();
            }
            Node* r2 = other_heap.root.get();
            sr::move(r2->child, std::back_inserter(r1->child));
            r1->validateChild();
            std::size_t curN = r1->child.size();
            while (r1->parent && r1->child.size() > 4) {
                std::size_t curN = r1->child.size();
                auto z = std::make_unique<Node>(); // will be y's right sibling
                z->height = r1->height;
                std::size_t rightN = (curN + 1) / 2;
                Node* y = root->child[0].get();
                z->height = y->height;
                sr::move(y->child | srv::drop(curN - rightN), std::back_inserter(z->child));
                z->validateChild();
                Node* p = r1->parent;
                p->child.push_back(std::move(z));
                y->child.resize(curN - rightN);
                p->validateChild();
                y->validateChild();
                r1->validateChild();
                r1 = p;
            }
            std::size_t curN = root->child.size();
            if (curN > 4) {
                auto s = std::make_unique<Node>();
                s->height = tree1.root->height + 1;
                s->child.push_back(std::move(root));
                root = std::move(s);
                std::size_t rightN = (curN + 1) / 2;
                auto z = std::make_unique<Node>(); // will be y's right sibling
                Node* y = root->child[0].get();
                z->height = y->height;
                sr::move(y->child | srv::drop(curN - rightN), std::back_inserter(z->child));
                z->validateChild();
                Node* p = root.get();
                p->child.push_back(std::move(z));
                y->child.resize(curN - rightN);
                p->validateChild();
                y->validateChild();
            }
        }
    }

};


int main() {
    BHeap<int> bheap;

    constexpr std::size_t N = 100;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (auto n : v) {
        bheap.Insert(n);
    }

    for (auto n : v) {
        bheap.ExtractMin();
    }


}