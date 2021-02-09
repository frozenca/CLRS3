#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace sr = std::ranges;

template <std::size_t u>
class XFastTrie {
    static_assert(u > 0 && u < 32);
    class Node {
        std::size_t value = 0;
        Node* left = nullptr;
        Node* right = nullptr;

        friend class XFastTrie;

        Node() = default;
        explicit Node(std::size_t value) : value {value} {}
    };

    std::vector<std::unordered_map<std::size_t, Node*>> lss;
    Node* root = nullptr;
    Node* leaf_head = nullptr;
    Node* leaf_tail = nullptr;

    [[nodiscard]] std::size_t getLCA(std::size_t k) const {
        std::size_t l = 0;
        std::size_t r = u;
        while (l < r && r < u + 1) {
            std::size_t m = (l + r) / 2;
            if (lss[m].contains(k >> m)) {
                r = m;
            } else {
                l = m + 1;
            }
        }
        return l;
    }

    [[nodiscard]] Node* MinimumOfSubtree(Node* node, std::size_t level) const {
        Node* curr = node;
        for (std::size_t i = level - 1; i < u; i--) {
            std::size_t curr_val = curr->value;
            if (lss[i].contains(curr_val * 2)) {
                curr = curr->left;
            } else {
                curr = curr->right;
            }
        }
        return curr;
    }

    [[nodiscard]] Node* MaximumOfSubtree(Node* node, std::size_t level) const {
        Node* curr = node;
        for (std::size_t i = level - 1; i < u; i--) {
            std::size_t curr_val = curr->value;
            if (lss[i].contains(curr_val * 2 + 1)) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }
        }
        return curr;
    }

    void UpdateDescendants(std::size_t k) {
        Node* curr = root;
        std::bitset<u> bits(k);
        for (std::size_t i = u - 1; i < u; i--) {
            std::size_t child_value = k >> i;
            std::size_t sibling_value = child_value ^ 1u;
            if (bits[i]) {
                if (!lss[i].contains(sibling_value)) {
                    curr->left = MinimumOfSubtree(curr->right, i);
                }
                curr = curr->right;
            } else {
                if (!lss[i].contains(sibling_value)) {
                    curr->right = MaximumOfSubtree(curr->left, i);
                }
                curr = curr->left;
            }
        }
    }

public:
    XFastTrie() : lss (u + 1), root {new Node()} {
        lss.back()[0] = root;
    }
    ~XFastTrie() {
        for (auto& lss_map : lss) {
            for (auto& [key, ptr] : lss_map) {
                delete ptr;
            }
        }
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        if (leaf_head) {
            return leaf_head->value;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        if (leaf_tail) {
            return leaf_tail->value;
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t k) const {
        assert(k < (1u << u));
        // find the longest common prefix
        std::size_t l = getLCA(k);
        if (l == 0) { // leaf
            Node* curr = lss[0].at(k);
            if (curr->right) {
                return curr->right->value;
            } else {
                return {};
            }
        } else {
            std::size_t prefix = k >> l;
            Node* curr = nullptr;
            if (lss[l - 1].contains(prefix * 2)) { // right is descendant
                curr = lss[l].at(prefix)->right;
            } else if (lss[l - 1].contains(prefix * 2 + 1)) { // left is descendant
                curr = lss[l].at(prefix)->left;
            }
            if (!curr) {
                if (leaf_tail && leaf_tail->value > k) {
                    return leaf_tail->value;
                } else {
                    return {};
                }
            } else if (curr->value > k) {
                return curr->value;
            } else if (curr->right) {
                return curr->right->value;
            } else {
                return {};
            }
        }
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t k) const {
        assert(k < (1u << u));
        // find the longest common prefix
        std::size_t l = getLCA(k);
        if (l == 0) { // leaf
            Node* curr = lss[0].at(k);
            if (curr->left) {
                return curr->left->value;
            } else {
                return {};
            }
        } else {
            std::size_t prefix = k >> l;
            Node* curr = nullptr;
            if (lss[l - 1].contains(prefix * 2)) { // right is descendant
                curr = lss[l].at(prefix)->right;
            } else if (lss[l - 1].contains(prefix * 2 + 1)) { // left is descendant
                curr = lss[l].at(prefix)->left;
            }
            if (!curr) {
                if (leaf_head && leaf_head->value < k) {
                    return leaf_head->value;
                } else {
                    return {};
                }
            } else if (curr->value < k) {
                return curr->value;
            } else if (curr->left) {
                return curr->left->value;
            } else {
                return {};
            }
        }
    }

    void Insert(std::size_t k) {
        assert(k < (1u << u));
        std::bitset<u> bits(k);
        Node* curr = root;

        Node* pred = nullptr;
        auto pred_opt = Predecessor(k);
        if (pred_opt.has_value()) {
            pred = lss[0][*pred_opt];
        }
        Node* succ = nullptr;
        auto succ_opt = Successor(k);
        if (succ_opt.has_value()) {
            succ = lss[0][*succ_opt];
        }

        // insert
        for (std::size_t i = u - 1; i < u; i--) {
            std::size_t child_value = k >> i;
            if (bits[i]) {
                if (!lss[i].contains(child_value)) {
                    curr->right = new Node(child_value);
                    lss[i][child_value] = curr->right;
                }
                curr = curr->right;
            } else {
                if (!lss[i].contains(child_value)) {
                    curr->left = new Node(child_value);
                    lss[i][child_value] = curr->left;
                }
                curr = curr->left;
            }
        }

        if (pred) {
            pred->right = curr;
            curr->left = pred;
        } else {
            if (leaf_head) {
                curr->right = leaf_head;
                leaf_head->left = curr;
            }
            leaf_head = curr;
        }
        if (succ) {
            succ->left = curr;
            curr->right = succ;
        } else {
            if (leaf_tail) {
                curr->left = leaf_tail;
                leaf_tail->right = curr;
            }
            leaf_tail = curr;
        }

        UpdateDescendants(k);
        if (pred_opt.has_value()) {
            UpdateDescendants(*pred_opt);
        }
        if (succ_opt.has_value()) {
            UpdateDescendants(*succ_opt);
        }
    }

    void Delete(std::size_t k) {
        assert(k < (1u << u));
        if (!lss[0].contains(k)) {
            return;
        }

        std::bitset<u> bits(k);

        Node* pred = nullptr;
        auto pred_opt = Predecessor(k);
        if (pred_opt.has_value()) {
            pred = lss[0][*pred_opt];
        }
        Node* succ = nullptr;
        auto succ_opt = Successor(k);
        if (succ_opt.has_value()) {
            succ = lss[0][*succ_opt];
        }

        Node* curr = lss[0][k];
        // delete
        bool to_delete = true;
        for (std::size_t i = 0; i < u; i++) {
            std::size_t child_value = k >> i;
            std::size_t sibling_value = child_value ^ 1u;
            Node* parent = lss[i + 1][child_value / 2];
            if (to_delete) {
                delete curr;
            }
            curr = parent;
            to_delete = to_delete && lss[i].contains(sibling_value);
        }

        if (pred) {
            pred->right = succ;
        } else {
            leaf_head = succ;
        }
        if (succ) {
            succ->left = pred;
        } else {
            leaf_tail = pred;
        }

        if (pred_opt.has_value()) {
            UpdateDescendants(*pred_opt);
        }
        if (succ_opt.has_value()) {
            UpdateDescendants(*succ_opt);
        }
    }

};

int main() {
    XFastTrie<7> trie;

    constexpr std::size_t N = 100;

     std::vector<int> v (N);
     std::iota(v.begin(), v.end(), 1);
     std::mt19937 gen(std::random_device{}());
     sr::shuffle(v, gen);

    for (std::size_t i = 0; i < 100; i++) {
        trie.Insert(v[i]);
    }

    auto curr = trie.Minimum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Successor(curr.value());
    }
    std::cout << '\n';

    sr::shuffle(v, gen);

    for (std::size_t i = 0; i < 90; i++) {
        trie.Delete(v[i]);
    }

    curr = trie.Minimum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Successor(curr.value());
    }

}