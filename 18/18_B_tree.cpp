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

template <std::semiregular T, std::size_t t, std::predicate<T, T> Comp = std::less<T>>
class BTree {
    static_assert(t >= 2);
    class Node {
        Node* parent = nullptr;
        std::size_t index = 0;
        std::size_t height = 1;
        std::vector<T> key;
        std::vector<std::unique_ptr<Node>> child;
    public:
        // modifying n must be done only with setN,
        // for the invariants key.size() == n and child.size() == n + 1 (if exists)
        void setN(std::size_t N) {
            key.resize(N);
            if (!child.empty()) {
                child.resize(N + 1);
            }
        }

        void validateChild() {
            if (child.empty()) {
                return;
            }
            for (std::size_t i = 0; i <= key.size(); i++) {
                child[i]->index = i;
                child[i]->parent = this;
            }
        }

        [[nodiscard]] std::size_t getN() const {
            return key.size();
        }

        [[nodiscard]] bool isFull() const {
            return key.size() == 2 * t - 1;
        }

        [[nodiscard]] bool canTakeKey() const {
            return key.size() > t - 1;
        }

        [[nodiscard]] bool hasMinimalKeys() const {
            return key.size() == t - 1;
        }

        [[nodiscard]] bool isEmpty() const {
            return key.empty();
        }

        friend class BTree;

        Node* RightmostLeaf() {
            Node* curr = this;
            while (!curr->child.empty()) {
                curr = curr->child.back().get();
            }
            return curr;
        }

        Node* LeftmostLeaf() {
            Node* curr = this;
            while (!curr->child.empty()) {
                curr = curr->child.front().get();
            }
            return curr;
        }

        // merge child[i + 1] and key[i] into child[i]
        void Merge(std::size_t i) noexcept {
            assert(!child.empty() && child[i]->hasMinimalKeys() && child[i + 1]->hasMinimalKeys());
            child[i]->setN(2 * t - 1);
            child[i]->key[t - 1] = key[i];
            // bring keys of child[i + 1]
            std::ranges::move(child[i + 1]->key, child[i]->key.begin() + t);
            // bring children of child[i + 1]
            if (!child[i]->child.empty()) {
                std::ranges::move(child[i + 1]->child, child[i]->child.begin() + t);
            }
            // shift children from i + 1 left by 1 (because child[i + 1] is merged)
            std::shift_left(child.begin() + i + 1, child.end(), 1);
            // shift keys from i left by 1 (because key[i] is merged)
            std::shift_left(key.begin() + i, key.end(), 1);
            setN(key.size() - 1);
            validateChild();
            child[i]->validateChild();
        }

        void LeftRotate() noexcept {
            assert(index + 1 < parent->child.size());
            auto sibling = parent->child[index + 1].get();
            // left rotation
            setN(key.size() + 1);
            key.back() = parent->key[index];
            parent->key[index] = sibling->key.front();
            // shift all keys of right sibling left by 1
            std::shift_left(sibling->key.begin(), sibling->key.end(), 1);
            if (!child.empty()) {
                child[key.size()] = std::move(sibling->child[0]);
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
            setN(key.size() + 1);
            // shift all keys of node right by 1
            std::shift_right(key.begin(), key.end(), 1);
            key.front() = parent->key[index - 1];
            parent->key[index - 1] = sibling->key.back();
            if (!child.empty()) {
                // shift all children of node right by 1
                std::shift_right(child.begin(), child.end(), 1);
                child[0] = std::move(sibling->child[sibling->getN()]);
            }
            sibling->setN(sibling->getN() - 1);
            validateChild();
            sibling->validateChild();
        }

    };


    template <bool Const>
    struct BTreeIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = std::conditional_t<Const, const T*, T*>;
        using reference = std::conditional_t<Const, const T&, T&>;
        using iterator_category = std::bidirectional_iterator_tag;

        Node* node = nullptr;
        std::size_t index;

        void Increment() {
            if (index == node->key.size()) {
                return;
            }
            if (node->child.empty()) {
                ++index;
                while (node->parent && index == node->key.size()) {
                    index = node->index;
                    node = node->parent;
                }
            } else {
                node = node->child[index + 1]->LeftmostLeaf();
                index = 0;
            }
        }

        void Decrement() {
            if (!node->leaf) {
                node = node->child[index]->RightmostLeaf();
                index = node->key.size() - 1;
            } else {
                if (index > 0) {
                    --index;
                } else {
                    while (node->parent && node->index == 0) {
                        node = node->parent;
                    }
                    if (node->index > 0) {
                        index = node->index - 1;
                        node = node->parent;
                    }
                }
            }
        }

        BTreeIterator() = default;

        BTreeIterator(Node* node, std::size_t i) : node {node}, index {i} {
            assert(node && i <= node->key.size());
        }

        reference operator*() const {
            return node->key[index];
        }

        pointer operator->() const {
            return node->key.begin() + index;
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

    };

    template <bool Const1, bool Const2>
    friend bool operator==(const BTreeIterator<Const1>& x, const BTreeIterator<Const2>& y) {
        return x.node == y.node && x.index == y.index;
    }

    template <bool Const1, bool Const2>
    friend bool operator!=(const BTreeIterator<Const1>& x, const BTreeIterator<Const2>& y) {
        return !(x == y);
    }

    std::unique_ptr<Node> root;

    using iterator = BTreeIterator<false>;
    using const_iterator = BTreeIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    std::pair<const Node*, std::size_t> Search(const Node* x, const T& k) const {
        std::size_t i = std::distance(x->key.begin(), std::ranges::lower_bound(x->key, k, Comp()));
        if (i < x->getN() && k == x->key[i]) { // equal? key found
            return {x, i};
        } else if (x->child.empty()) { // no child, key is not in the tree
            return {nullptr, 0};
        } else { // search on child between range
            return Search(x->child[i].get(), k);
        }
    }

    void InsertNonFull(Node* x, const T& k) {
        if (x->child.empty()) { // key should be inserted only at leaf
            InsertToLeaf(x, k);
        } else {
            auto i = std::distance(x->key.begin(), std::ranges::upper_bound(x->key, k, Comp()));
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
        assert(node->child.empty());
        auto i = std::distance(node->key.begin(), std::ranges::upper_bound(node->key, k, Comp()));
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
        z->height = y->height;
        z->setN(t - 1);
        // bring right half keys from y
        std::ranges::move(y->key | std::views::drop(t), z->key.begin());
        if (!y->child.empty()) {
            // bring right half children from y
            std::ranges::move(y->child |  std::views::drop(t), std::back_inserter(z->child));
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
        std::size_t i = std::distance(x->key.begin(), std::ranges::lower_bound(x->key, k, Comp()));
        if (i < x->getN() && k == x->key[i]) { // equal? key found
            Delete(x, k, i);
        } else if (x->child.empty()) { // no child, key is not in the tree
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
        if (x->child.empty()) {
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
        assert(node->child.empty());
        std::shift_left(node->key.begin() + i, node->key.end(), 1);
        node->setN(node->getN() - 1);
        assert(node == root.get() || node->getN() >= t - 1);
    }

    void ValidateIterators() {
        begin_ = iterator(root->LeftmostLeaf(), 0);
        cbegin_ = const_iterator(root->LeftmostLeaf(), 0);
        end_ = iterator(root.get(), root->getN());
        cend_ = const_iterator(root.get(), root->getN());
    }

    iterator begin_;
    const_iterator cbegin_;
    iterator end_;
    const_iterator cend_;

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
            s->child.resize(1);
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

    [[nodiscard]] iterator begin() {
        return begin_;
    }

    [[nodiscard]] const_iterator begin() const {
        return cbegin_;
    }

    [[nodiscard]] const_iterator cbegin() const {
        return cbegin_;
    }

    [[nodiscard]] iterator end() {
        return end_;
    }

    [[nodiscard]] const_iterator end() const {
        return cend_;
    }

    [[nodiscard]] const_iterator cend() const {
        return cend_;
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator(end_);
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend_);
    }

    [[nodiscard]] const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend_);
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator(begin_);
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin_);
    }

    [[nodiscard]] const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin_);
    }

    [[nodiscard]] std::size_t getHeight() const {
        return root->height;
    }

};

int main() {
    BTree<int, 2> tree;

    constexpr std::size_t N = 100;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    std::ranges::shuffle(v, gen);

    for (auto n : v) {
        tree.Insert(n);
        for (const auto& key : tree) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }

    assert(std::ranges::all_of(v, [&tree](auto n){return tree.Search(n).first != nullptr;}));


    // should output 1 2 3 ... N
    for (const auto& key : tree) {
        std::cout << key << ' ';
    }
    std::cout << '\n';

    std::ranges::shuffle(v, gen);

    for (auto n : v) {
        tree.Delete(n);
        for (const auto& key : tree) {
            std::cout << key << ' ';
        }
        std::cout << '\n';
    }

}
