#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <ranges>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <std::regular T, std::size_t t, std::predicate<T, T> Comp = std::less<T>>
class BTree {
    static_assert(t >= 2);
    class Node {
        std::size_t n = 0;
        bool leaf = true;
        Node* parent = nullptr;
        std::size_t index = 0;
        std::size_t height = 1;
        std::vector<T> key;
        std::vector<std::unique_ptr<Node>> child;
    public:
        // modifying n must be done only with setN,
        // for the invariants key.size() == n and child.size() == n + 1 (if exists)
        void setN(std::size_t N) {
            n = N;
            key.resize(n);
            if (!leaf) {
                child.resize(n + 1);
            }
        }

        void validateChild() {
            if (leaf) {
                return;
            }
            for (std::size_t i = 0; i <= n; i++) {
                child[i]->index = i;
                child[i]->parent = this;
            }
        }

        [[nodiscard]] std::size_t getN() const {
            return n;
        }

        [[nodiscard]] bool isFull() const {
            return n == 2 * t - 1;
        }

        [[nodiscard]] bool canTakeKey() const {
            return n > t - 1;
        }

        [[nodiscard]] bool hasMinimalKeys() const {
            return n == t - 1;
        }

        [[nodiscard]] bool isEmpty() const {
            return n == 0;
        }

        friend class BTree;

        Node* RightmostLeaf() {
            Node* curr = this;
            while (!curr->leaf) {
                curr = curr->child.back().get();
            }
            return curr;
        }

        Node* LeftmostLeaf() {
            Node* curr = this;
            while (!curr->leaf) {
                curr = curr->child.front().get();
            }
            return curr;
        }

        // merge child[i + 1] and key[i] into child[i]
        void Merge(std::size_t i) noexcept {
            assert(!leaf && child[i]->hasMinimalKeys() && child[i + 1]->hasMinimalKeys());
            child[i]->setN(2 * t - 1);
            child[i]->key[t - 1] = key[i];
            // bring keys of child[i + 1]
            sr::move(child[i + 1]->key, child[i]->key.begin() + t);
            // bring children of child[i + 1]
            if (!child[i]->leaf) {
                sr::move(child[i + 1]->child, child[i]->child.begin() + t);
            }
            // shift children from i + 1 left by 1 (because child[i + 1] is merged)
            std::shift_left(child.begin() + i + 1, child.end(), 1);
            // shift keys from i left by 1 (because key[i] is merged)
            std::shift_left(key.begin() + i, key.end(), 1);
            setN(n - 1);
            validateChild();
            child[i]->validateChild();
        }

        void LeftRotate() noexcept {
            assert(index + 1 < parent->child.size());
            auto sibling = parent->child[index + 1].get();
            // left rotation
            setN(n + 1);
            key.back() = parent->key[index];
            parent->key[index] = sibling->key.front();
            // shift all keys of right sibling left by 1
            std::shift_left(sibling->key.begin(), sibling->key.end(), 1);
            if (!leaf) {
                child[n] = std::move(sibling->child[0]);
                // shift all children of right sibling left by 1
                std::shift_left(sibling->child.begin(), sibling->child.end(), 1);
            }
            sibling->setN(sibling->getN() - 1);
            validateChild();
            sibling->validateChild();
        }

        void RightRotate() noexcept {
            assert(index - 1 < parent->child.size());
            auto sibling = parent->child[index - 1].get();
            // right rotation
            setN(n + 1);
            // shift all keys of node right by 1
            std::shift_right(key.begin(), key.end(), 1);
            key.front() = parent->key[index - 1];
            parent->key[index - 1] = sibling->key.back();
            if (!leaf) {
                // shift all children of node right by 1
                std::shift_right(child.begin(), child.end(), 1);
                child[0] = std::move(sibling->child[sibling->getN()]);
            }
            sibling->setN(sibling->getN() - 1);
            validateChild();
            sibling->validateChild();
        }

        friend BTree<T, t, Comp> Join(BTree<T, t, Comp>& tree1, const T& x, BTree<T, t, Comp>& tree2);
        friend std::pair<BTree<T, t, Comp>, BTree<T, t, Comp>> Split(BTree<T, t, Comp>& tree, const T& k);
    };

    struct BTreeIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        Node* node = nullptr;
        std::vector<T>::iterator it;

        void Increment() {
            if (it == node->key.end()) {
                return;
            }
            if (node->leaf) {
                ++it;
                while (node->parent && it == node->key.end()) {
                    it = node->parent->key.begin() + node->index;
                    node = node->parent;
                }
            } else {
                auto i = std::distance(node->key.begin(), it);
                node = node->child[i + 1]->LeftmostLeaf();
                it = node->key.begin();
            }
        }

        void Decrement() {
            auto i = std::distance(node->key.begin(), it);
            if (!node->leaf) {
                node = node->child[i]->RightmostLeaf();
                it = node->key.begin() + node->key.size() - 1;
            } else {
                if (i > 0) {
                    --it;
                } else {
                    while (node->parent && node->index == 0) {
                        node = node->parent;
                    }
                    if (node->index > 0) {
                        it = node->parent->key.begin() + node->index - 1;
                        node = node->parent;
                    }
                }
            }

        }

        BTreeIterator() = default;

        BTreeIterator(Node* node, std::size_t i) : node {node} {
            assert(node && i <= node->key.size());
            it = node->key.begin() + i;
        }

        reference operator*() const {
            return *it;
        }

        pointer operator->() const {
            return it;
        }

        BTreeIterator& operator++() {
            Increment();
            return *this;
        }

        BTreeIterator operator++(int) {
            BTreeIterator temp = *this;
            Increment();
            return temp;
        }

        BTreeIterator& operator--() {
            Decrement();
            return *this;
        }

        BTreeIterator operator--(int) {
            BTreeIterator temp = *this;
            Decrement();
            return temp;
        }

        friend bool operator==(const BTreeIterator& x, const BTreeIterator& y) {
            return x.node == y.node && x.it == y.it;
        }

        friend bool operator!=(const BTreeIterator& x, const BTreeIterator& y) {
            return !(x == y);
        }
    };

    struct BTreeConstIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::bidirectional_iterator_tag;

        const Node* node = nullptr;
        std::vector<T>::const_iterator it;

        void Increment() {
            if (it == node->key.cend()) {
                return;
            }
            if (node->leaf) {
                ++it;
                while (node->parent && it == node->key.cend()) {
                    it = node->parent->key.cbegin() + node->index;
                    node = node->parent;
                }
            } else {
                auto i = std::distance(node->key.cbegin(), it);
                node = node->child[i + 1]->LeftmostLeaf();
                it = node->key.cbegin();
            }
        }

        void Decrement() {
            auto i = std::distance(node->key.cbegin(), it);
            if (!node->leaf) {
                node = node->child[i]->RightmostLeaf();
                it = node->key.cbegin() + node->key.size() - 1;
            } else {
                if (i > 0) {
                    --it;
                } else {
                    while (node->parent && node->index == 0) {
                        node = node->parent;
                    }
                    if (node->index > 0) {
                        it = node->parent->key.cbegin() + node->index - 1;
                        node = node->parent;
                    }
                }
            }

        }

        BTreeConstIterator() = default;

        BTreeConstIterator(const Node* node, std::size_t i) : node {node} {
            assert(node && i <= node->key.size());
            it = node->key.cbegin() + i;
        }

        reference operator*() const {
            return *it;
        }

        pointer operator->() const {
            return it;
        }

        BTreeConstIterator& operator++() {
            Increment();
            return *this;
        }

        BTreeConstIterator operator++(int) {
            BTreeConstIterator temp = *this;
            Increment();
            return temp;
        }

        BTreeConstIterator& operator--() {
            Decrement();
            return *this;
        }

        BTreeConstIterator operator--(int) {
            BTreeConstIterator temp = *this;
            Decrement();
            return temp;
        }

        friend bool operator==(const BTreeConstIterator& x, const BTreeConstIterator& y) {
            return x.node == y.node && x.it == y.it;
        }

        friend bool operator!=(const BTreeConstIterator& x, const BTreeConstIterator& y) {
            return !(x == y);
        }
    };

    std::unique_ptr<Node> root;

    using Iterator = BTreeIterator;
    using ConstIterator = BTreeConstIterator;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    std::pair<const Node*, std::size_t> Search(const Node* x, const T& k) const {
        std::size_t i = std::distance(x->key.begin(), sr::lower_bound(x->key, k, Comp()));
        if (i < x->getN() && k == x->key[i]) { // equal? key found
            return {x, i};
        } else if (x->leaf) { // no child, key is not in the tree
            return {nullptr, 0};
        } else { // search on child between range
            return Search(x->child[i].get(), k);
        }
    }

    void InsertNonFull(Node* x, const T& k) {
        if (x->leaf) { // key should be inserted only at leaf
            InsertToLeaf(x, k);
        } else {
            auto i = std::distance(x->key.begin(), sr::upper_bound(x->key, k, Comp()));
            if (x->child[i]->isFull()) { // is full? then split
                SplitChild(x, i);
                if (Comp()(x->key[i], k)) {
                    i++;
                }
            }
            InsertNonFull(x->child[i].get(), k); // recursively insert
        }
    }

    void InsertToLeaf(Node* node, const T& k) {
        assert(node->leaf);
        auto i = std::distance(node->key.begin(), sr::upper_bound(node->key, k, Comp()));
        node->setN(node->getN() + 1);
        std::shift_right(node->key.begin() + i, node->key.end(), 1);
        node->key[i] = k;
        // n cannot exceeds 2t - 1, because in that case
        // its parent should've called SplitChild on x before
        assert(node->getN() < 2 * t);
    }

    // split x->child[i]
    static void SplitChild(Node* x, std::size_t i) noexcept {
        if (!x) {
            return;
        }
        auto y = x->child[i].get();
        if (!y) {
            return;
        }
        // x cannot be full, because in that case its parent should've called SplitChild on x before
        assert(!x->isFull() && y->isFull());
        auto z = std::make_unique<Node>(); // will be y's right sibling
        z->leaf = y->leaf;
        z->height = y->height;
        z->setN(t - 1);
        // bring right half keys from y
        sr::move(y->key | srv::drop(t), z->key.begin());
        if (!y->leaf) {
            // bring right half children from y
            sr::move(y->child |  srv::drop(t), z->child.begin());
            z->validateChild();
        }
        x->setN(x->getN() + 1);
        // shift children of x right by 1 from i + 1
        std::shift_right(x->child.begin() + i + 1, x->child.end(), 1);
        x->child[i + 1] = std::move(z);
        // shift keys of x right by 1 from i
        std::shift_right(x->key.begin() + i, x->key.end(), 1);
        x->key[i] = y->key[t - 1];
        y->setN(t - 1);
        x->validateChild();
        y->validateChild();
    }

    void Delete(Node* x, const T& k) noexcept {
        std::size_t i = std::distance(x->key.begin(), sr::lower_bound(x->key, k, Comp()));
        if (i < x->getN() && k == x->key[i]) { // equal? key found
            Delete(x, k, i);
        } else if (x->leaf) { // no child, key is not in the tree
            return;
        } else { // search on child between range
            Node* next = x->child[i].get();
            if (x->child[i]->hasMinimalKeys()) {
                if (i + 1 < x->child.size() && x->child[i + 1]->canTakeKey()) {
                    x->child[i]->LeftRotate();
                } else if (i - 1 < x->child.size() && x->child[i - 1]->canTakeKey()) {
                    x->child[i]->RightRotate();
                } else if (i + 1 < x->child.size()) {
                    x->Merge(i);
                    next = x->child[i].get();
                    if (x == root.get() && x->isEmpty()) {
                        // shrink tree in height, merged child should be a new root
                        root = std::move(x->child[i]);
                        root->parent = nullptr;
                    }
                } else if (i - 1 < x->child.size()) {
                    x->Merge(i - 1);
                    next = x->child[i - 1].get();
                    if (x == root.get() && x->isEmpty()) {
                        // shrink tree in height, merged child should be a new root
                        root = std::move(x->child[i - 1]);
                        root->parent = nullptr;
                    }
                }
            }
            Delete(next, k);
        }
    }

    void Delete(Node* x, const T& k, std::size_t i) noexcept {
        assert(x->key[i] == k);
        if (x->leaf) {
            // directly erase from leaf
            DeleteToLeaf(x, i);
        } else if (x->child[i]->canTakeKey()) {
            // find predecessor and swap keys
            auto predLeaf = x->child[i]->RightmostLeaf();
            std::swap(predLeaf->key.back(), x->key[i]);
            // now k is in left child, search there
            Delete(x->child[i].get(), k);
        } else if (x->child[i + 1]->canTakeKey()) {
            // find successor and swap keys
            auto succLeaf = x->child[i + 1]->LeftmostLeaf();
            std::swap(succLeaf->key.front(), x->key[i]);
            // now k is in right child, search there
            Delete(x->child[i + 1].get(), k);
        } else {
            // merge two children
            x->Merge(i);
            Node* next = x->child[i].get();
            if (x == root.get() && x->getN() == 0) {
                // shrink tree in height, merged child should be a new root
                root = std::move(x->child[i]);
                root->parent = nullptr;
            }
            Delete(next, k);
        }
    }

    void DeleteToLeaf(Node* node, std::size_t i) {
        assert(node->leaf);
        std::shift_left(node->key.begin() + i, node->key.end(), 1);
        node->setN(node->getN() - 1);
        assert(node == root.get() || node->getN() >= t - 1);
    }

    void ValidateIterators() {
        begin_ = Iterator(root->LeftmostLeaf(), 0);
        cbegin_ = ConstIterator(root->LeftmostLeaf(), 0);
        end_ = Iterator(root.get(), root->getN());
        cend_ = ConstIterator(root.get(), root->getN());
    }

    Iterator begin_;
    ConstIterator cbegin_;
    Iterator end_;
    ConstIterator cend_;

public:
    BTree() : root {std::make_unique<Node>()},
              begin_ {root.get(), 0}, cbegin_ {root.get(), 0},
              end_ {root.get(), 0}, cend_ {root.get(), 0} {
    }

    BTree(std::unique_ptr<Node> other) : root {std::move(other)} {
        root->parent = nullptr;
        ValidateIterators();
    }

    [[nodiscard]] std::pair<const Node*, std::size_t> Search(const T& k) const {
        return Search(root.get(), k);
    }

    void Insert(const T& k) noexcept {
        if (root->isFull()) { // if root is full then make it as a child of new root - and split
            auto s = std::make_unique<Node>();
            s->leaf = false;
            s->setN(0);
            s->height = root->height + 1;
            s->child[0] = std::move(root);
            root = std::move(s);
            SplitChild(root.get(), 0);
            InsertNonFull(root.get(), k);
        } else {
            InsertNonFull(root.get(), k);
        }
        ValidateIterators();
    }

    void Delete(const T& k) {
        Delete(root.get(), k);
        ValidateIterators();
    }

    [[nodiscard]] Iterator begin() {
        return begin_;
    }

    [[nodiscard]] ConstIterator begin() const {
        return cbegin_;
    }

    [[nodiscard]] ConstIterator cbegin() const {
        return cbegin_;
    }

    [[nodiscard]] Iterator end() {
        return end_;
    }

    [[nodiscard]] ConstIterator end() const {
        return cend_;
    }

    [[nodiscard]] ConstIterator cend() const {
        return cend_;
    }

    [[nodiscard]] ReverseIterator rbegin() {
        return ReverseIterator(end_);
    }

    [[nodiscard]] ConstReverseIterator rbegin() const {
        return ConstReverseIterator(cend_);
    }

    [[nodiscard]] ConstReverseIterator crbegin() const {
        return ConstReverseIterator(cend_);
    }

    [[nodiscard]] ReverseIterator rend() {
        return ReverseIterator(begin_);
    }

    [[nodiscard]] ConstReverseIterator rend() const {
        return ConstReverseIterator(cbegin_);
    }

    [[nodiscard]] ConstReverseIterator crend() const {
        return ConstReverseIterator(cbegin_);
    }

    [[nodiscard]] std::size_t getHeight() const {
        return root->height;
    }

    // too many duplicated code, works but must be refactored
    friend BTree<T, t, Comp> Join(BTree<T, t, Comp>& tree1, const T& x, BTree<T, t, Comp>& tree2) {
        assert((tree1.root->getN() == 0 || Comp()(*tree1.rbegin(), x)) &&
               (tree2.root->getN() == 0 || Comp()(x, *tree2.begin())));
        if (tree1.getHeight() == tree2.getHeight()) { // equal height
            std::size_t n1 = tree1.root->getN();
            std::size_t n2 = tree2.root->getN();
            tree1.root->setN(n1 + n2 + 1);
            tree1.root->key[n1] = x;
            sr::move(tree2.root->key, tree1.root->key.begin() + n1 + 1);
            if (!tree1.root->leaf) {
                sr::move(tree2.root->child, tree1.root->child.begin() + n1 + 1);
            }
            tree1.root->validateChild();
            std::size_t curN = tree1.root->getN();
            if (curN > 2 * t - 1) {
                auto s = std::make_unique<Node>();
                s->leaf = false;
                s->setN(0);
                s->height = tree1.root->height + 1;
                s->child[0] = std::move(tree1.root);
                tree1.root = std::move(s);
                std::size_t rightN = (curN - 1) / 2;
                auto z = std::make_unique<Node>(); // will be y's right sibling
                Node* y = tree1.root->child[0].get();
                z->leaf = y->leaf;
                z->height = y->height;
                z->setN(rightN);
                // bring right half keys from y
                sr::move(y->key | srv::drop(curN - rightN), z->key.begin());
                if (!y->leaf) {
                    // bring right half children from y
                    sr::move(y->child | srv::drop(curN - rightN), z->child.begin());
                    z->validateChild();
                }
                Node* p = tree1.root.get();
                p->setN(1);
                p->child[1] = std::move(z);
                p->key[0] = y->key[curN - rightN - 1];
                y->setN(curN - rightN - 1);
                p->validateChild();
                y->validateChild();
            }
            tree1.ValidateIterators();
            return std::move(tree1);
        } else if (tree1.getHeight() > tree2.getHeight()) {
            Node* r1 = tree1.root.get();
            for (std::size_t i = 0; i < tree1.getHeight() - tree2.getHeight(); i++) {
                r1 = r1->child.back().get();
            }
            Node* r2 = tree2.root.get();
            std::size_t n1 = r1->getN();
            std::size_t n2 = r2->getN();
            r1->setN(n1 + n2 + 1);
            r1->key[n1] = x;
            sr::move(r2->key, r1->key.begin() + n1 + 1);
            if (!r1->leaf) {
                sr::move(r2->child, r1->child.begin() + n1 + 1);
            }
            r1->validateChild();
            while (r1->parent && r1->getN() > 2 * t - 1) {
                std::size_t curN = r1->getN();
                auto z = std::make_unique<Node>(); // will be y's right sibling
                z->leaf = r1->leaf;
                z->height = r1->height;
                std::size_t rightN = (curN - 1) / 2;
                z->setN(rightN);
                // bring right half keys from r1
                sr::move(r1->key | srv::drop(curN - rightN), z->key.begin());
                if (!r1->leaf) {
                    // bring right half children from r1
                    sr::move(r1->child | srv::drop(curN - rightN), z->child.begin());
                    z->validateChild();
                }
                Node* p = r1->parent;
                std::size_t i = r1->index;
                p->setN(p->getN() + 1);
                // shift children of x right by 1 from i + 1
                std::shift_right(p->child.begin() + i + 1, p->child.end(), 1);
                p->child[i + 1] = std::move(z);
                // shift keys of x right by 1 from i
                std::shift_right(p->key.begin() + i, p->key.end(), 1);
                p->key[i] = r1->key[curN - rightN - 1];
                r1->setN(curN - rightN - 1);
                p->validateChild();
                r1->validateChild();
                r1 = p;
            }
            std::size_t curN = tree1.root->getN();
            if (curN > 2 * t - 1) {
                auto s = std::make_unique<Node>();
                s->leaf = false;
                s->setN(0);
                s->height = tree1.root->height + 1;
                s->child[0] = std::move(tree1.root);
                tree1.root = std::move(s);
                std::size_t rightN = (curN - 1) / 2;
                auto z = std::make_unique<Node>(); // will be y's right sibling
                Node* y = tree1.root->child[0].get();
                z->leaf = y->leaf;
                z->height = y->height;
                z->setN(rightN);
                // bring right half keys from y
                sr::move(y->key | srv::drop(curN - rightN), z->key.begin());
                if (!y->leaf) {
                    // bring right half children from y
                    sr::move(y->child | srv::drop(curN - rightN), z->child.begin());
                    z->validateChild();
                }
                Node* p = tree1.root.get();
                p->setN(1);
                p->child[1] = std::move(z);
                p->key[0] = y->key[curN - rightN - 1];
                y->setN(curN - rightN - 1);
                p->validateChild();
                y->validateChild();
            }
            tree1.ValidateIterators();
            return std::move(tree1);
        } else {
            Node* r2 = tree2.root.get();
            for (std::size_t i = 0; i < tree2.getHeight() - tree1.getHeight(); i++) {
                r2 = r2->child.front().get();
            }
            Node* r1 = tree1.root.get();
            std::size_t n1 = r1->getN();
            std::size_t n2 = r2->getN();
            r2->setN(n1 + n2 + 1);
            std::shift_right(r2->key.begin(), r2->key.end(), n1 + 1);
            sr::move(r1->key, r2->key.begin());
            r2->key[n1] = x;
            if (!r1->leaf) {
                std::shift_right(r2->child.begin(), r2->child.end(), n1 + 1);
                sr::move(r1->child, r2->child.begin());
            }
            r2->validateChild();
            while (r2->parent && r2->getN() > 2 * t - 1) {
                std::size_t curN = r2->getN();
                auto z = std::make_unique<Node>(); // will be y's right sibling
                z->leaf = r2->leaf;
                z->height = r2->height;
                std::size_t rightN = (curN - 1) / 2;
                z->setN(rightN);
                // bring right half keys from r2
                sr::move(r2->key | srv::drop(curN - rightN), z->key.begin());
                if (!r2->leaf) {
                    // bring right half children from r2
                    sr::move(r2->child | srv::drop(curN - rightN), z->child.begin());
                    z->validateChild();
                }
                Node* p = r2->parent;
                std::size_t i = r2->index;
                p->setN(p->getN() + 1);
                // shift children of x right by 1 from i + 1
                std::shift_right(p->child.begin() + i + 1, p->child.end(), 1);
                p->child[i + 1] = std::move(z);
                // shift keys of x right by 1 from i
                std::shift_right(p->key.begin() + i, p->key.end(), 1);
                p->key[i] = r2->key[curN - rightN - 1];
                r2->setN(curN - rightN - 1);
                p->validateChild();
                r2->validateChild();
                r2 = p;
            }
            std::size_t curN = tree2.root->getN();
            if (curN > 2 * t - 1) {
                auto s = std::make_unique<Node>();
                s->leaf = false;
                s->setN(0);
                s->height = tree2.root->height + 1;
                s->child[0] = std::move(tree2.root);
                tree2.root = std::move(s);
                std::size_t rightN = (curN - 1) / 2;
                auto z = std::make_unique<Node>(); // will be y's right sibling
                Node* y = tree2.root->child[0].get();
                z->leaf = y->leaf;
                z->height = y->height;
                z->setN(rightN);
                // bring right half keys from y
                sr::move(y->key | srv::drop(curN - rightN), z->key.begin());
                if (!y->leaf) {
                    // bring right half children from y
                    sr::move(y->child | srv::drop(curN - rightN), z->child.begin());
                    z->validateChild();
                }
                Node* p = tree2.root.get();
                p->setN(1);
                p->child[1] = std::move(z);
                p->key[0] = y->key[curN - rightN - 1];
                y->setN(curN - rightN - 1);
                p->validateChild();
                y->validateChild();
            }
            tree2.ValidateIterators();
            return std::move(tree2);
        }
    }


    friend std::pair<BTree<T, t, Comp>, BTree<T, t, Comp>> Split(BTree<T, t, Comp>& tree, const T& x) {
        assert(tree.Search(x).first != nullptr);
        BTree<T, t, Comp> tree1, tree2;

        Node* r = tree.root.get();
        std::stack<Node*> path;
        while (true) {
            std::size_t i = std::distance(r->key.begin(), sr::lower_bound(r->key, x, Comp()));
            if (i < r->getN() && x == r->key[i]) {
                if (r->leaf) {
                    for (std::size_t j = 0; j < i; j++) {
                        tree1.Insert(r->key[j]);
                    }
                    for (std::size_t j = i + 1; j < r->getN(); j++) {
                        tree2.Insert(r->key[j]);
                    }
                } else {
                    for (std::size_t j = i; j < r->getN(); j--) {
                        BTree<T, t, Comp> tempTree (std::move(r->child[j]));
                        tree1 = Join(tempTree, r->key[j], tree1);
                    }
                    tree1.Delete(x);
                    for (std::size_t j = i + 1; j <= r->getN(); j++) {
                        BTree<T, t, Comp> tempTree (std::move(r->child[j]));
                        tree2 = Join(tree2, r->key[j - 1], tempTree);
                    }
                    tree2.Delete(x);
                }
                break;
            } else if (r->leaf) {
                throw std::invalid_argument("key is not in the tree");
            } else {
                r = r->child[i].get();
                path.push(r);
            }
        }
        while (!path.empty()) {
            r = path.top();
            path.pop();
            std::size_t i = r->index;
            for (std::size_t j = i - 1; j < r->parent->getN(); j--) {
                BTree<T, t, Comp> tempTree (std::move(r->parent->child[j]));
                tree1 = Join(tempTree, r->parent->key[j], tree1);
            }
            for (std::size_t j = i + 1; j <= r->parent->getN(); j++) {
                BTree<T, t, Comp> tempTree (std::move(r->parent->child[j]));
                tree2 = Join(tree2, r->parent->key[j - 1], tempTree);
            }
        }
        tree1.ValidateIterators();
        tree2.ValidateIterators();
        return {std::move(tree1), std::move(tree2)};
    }
};

int main() {
    BTree<int, 2> tree;

    constexpr std::size_t N = 100;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Insert(n);
        for (const auto& key : tree) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }

    assert(sr::all_of(v, [&tree](auto n){return tree.Search(n).first != nullptr;}));


    // should output 1 2 3 ... N
    for (const auto& key : tree) {
        std::cout << key << ' ';
    }
    std::cout << '\n';

    sr::shuffle(v, gen);

    for (auto n : v) {
        tree.Delete(n);
        for (const auto& key : tree) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }

    for (int i = 1; i <= N; i++) {
        BTree<int, 2> tree1, tree2;
        for (int j = 1; j < i; j++) {
            tree1.Insert(j);
        }
        for (int j = i + 1; j <= N; j++) {
            tree2.Insert(j);
        }
        auto tree3 = Join(tree1, i, tree2);
        for (const auto& key : tree3) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }

    for (int i = 1; i <= N; i++) {
        BTree<int, 2> tree1;
        for (int j = 1; j <= N; j++) {
            tree1.Insert(j);
        }
        auto [tree2, tree3] = Split(tree1, i);
        for (const auto& key : tree2) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
        for (const auto& key : tree3) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }
}