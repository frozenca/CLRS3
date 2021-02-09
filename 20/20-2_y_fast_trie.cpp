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
#include <set>
#include <unordered_map>
#include <vector>

namespace sr = std::ranges;

template <std::size_t u>
class YFastTrie;

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

    friend class YFastTrie<u>;

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

    [[nodiscard]] bool Find(std::size_t k) const {
        return lss[0].contains(k);
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
                lss[i].erase(child_value);
            }
            curr = parent;
            to_delete = to_delete && !lss[i].contains(sibling_value);
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

template <std::size_t u>
class YFastTrie {
    static_assert(u > 0 && u < 32);

    XFastTrie<u> xfast;
    std::unordered_map<std::size_t, std::set<std::size_t>> data;

    void SplitIfNecessary(std::size_t repr, std::set<std::size_t>& subtree) {
        if (subtree.size() <= 2 * u) {
            return;
        }
        std::set<std::size_t> ltree;
        std::set<std::size_t> rtree;
        auto it = subtree.begin();
        for (std::size_t i = 0; i < u; i++) {
            ltree.insert(*it);
            ++it;
        }
        std::size_t lrepr = *ltree.rbegin();
        for (std::size_t i = u; i < subtree.size(); i++) {
            rtree.insert(*it);
            ++it;
        }
        std::size_t rrepr = *rtree.rbegin();
        xfast.Delete(repr);
        data.erase(repr);
        xfast.Insert(lrepr);
        data[lrepr] = ltree;
        xfast.Insert(rrepr);
        data[rrepr] = rtree;
    }

    void MergeIfNecessary(std::size_t repr, std::set<std::size_t>& subtree) {
        if (subtree.size() >= u / 2) {
            return;
        }
        auto right_opt = xfast.Successor(repr);
        auto left_opt = xfast.Predecessor(repr);
        if (right_opt.has_value()) {
            std::size_t right_repr = *right_opt;
            auto& right_tree = data[right_repr];
            std::set<std::size_t> merged_tree;
            for (auto n : subtree) {
                merged_tree.insert(n);
            }
            for (auto n : right_tree) {
                merged_tree.insert(n);
            }
            std::size_t merged_repr = *merged_tree.rbegin();
            xfast.Delete(right_repr);
            data.erase(right_repr);
            xfast.Delete(repr);
            data.erase(repr);
            xfast.Insert(merged_repr);
            data[merged_repr] = merged_tree;
            SplitIfNecessary(merged_repr, merged_tree);
        } else if (left_opt.has_value()) {
            std::size_t left_repr = *left_opt;
            auto& left_tree = data[left_repr];
            std::set<std::size_t> merged_tree;
            for (auto n : subtree) {
                merged_tree.insert(n);
            }
            for (auto n : left_tree) {
                merged_tree.insert(n);
            }
            std::size_t merged_repr = *merged_tree.rbegin();
            xfast.Delete(left_repr);
            data.erase(left_repr);
            xfast.Delete(repr);
            data.erase(repr);
            xfast.Insert(merged_repr);
            data[merged_repr] = merged_tree;
            SplitIfNecessary(merged_repr, merged_tree);
        }
    }

public:
    [[nodiscard]] bool Find(std::size_t k) const {
        auto succ_opt = xfast.Successor(k);
        auto pred_opt = xfast.Predecessor(k);
        if (succ_opt.has_value()) {
            pred_opt = xfast.Predecessor(*succ_opt);
            if (data.at(*succ_opt).contains(k)) {
                return true;
            }
            if (pred_opt.has_value() && data.at(*pred_opt).contains(k)) {
                return true;
            }
        }
        if (pred_opt.has_value()) {
            succ_opt = xfast.Successor(*pred_opt);
            if (data.at(*pred_opt).contains(k)) {
                return true;
            }
            if (succ_opt.has_value() && data.at(*succ_opt).contains(k)) {
                return true;
            }
        }
        if (xfast.Find(k)) {
            auto& exact_tree = data.at(k);
            if (exact_tree.contains(k)) {
                return true;
            }
            succ_opt = xfast.Successor(k);
            if (succ_opt.has_value() && data.at(*succ_opt).contains(k)) {
                return true;
            }
            pred_opt = xfast.Predecessor(k);
            if (pred_opt.has_value() && data.at(*pred_opt).contains(k)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::optional<std::size_t> Minimum() const {
        auto min_opt = xfast.Minimum();
        if (min_opt.has_value()) {
            return *data.at(*min_opt).begin();
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Maximum() const {
        auto max_opt = xfast.Maximum();
        if (max_opt.has_value()) {
            return *data.at(*max_opt).rbegin();
        } else {
            return {};
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor(std::size_t k) const {
        auto succ_opt = xfast.Successor(k);
        auto pred_opt = xfast.Predecessor(k);
        if (succ_opt.has_value()) {
            pred_opt = xfast.Predecessor(*succ_opt);
            auto& succ_tree = data.at(*succ_opt);
            if (pred_opt.has_value()) {
                auto& pred_tree = data.at(*pred_opt);
                auto pred_it = pred_tree.upper_bound(k);
                if (pred_it != pred_tree.end()) {
                    return *pred_it;
                }
            }
            auto succ_it = succ_tree.upper_bound(k);
            if (succ_it != succ_tree.end()) {
                return *succ_it;
            }
            auto next_opt = xfast.Successor(*succ_opt);
            if (next_opt.has_value()) {
                auto& next_tree = data.at(*next_opt);
                return *next_tree.begin();
            }
        }
        if (pred_opt.has_value()) {
            auto& pred_tree = data.at(*pred_opt);
            auto pred_it = pred_tree.upper_bound(k);
            if (pred_it != pred_tree.end()) {
                return *pred_it;
            }
            succ_opt = xfast.Successor(*pred_opt);
            if (succ_opt.has_value()) {
                auto& succ_tree = data.at(*succ_opt);
                auto succ_it = succ_tree.upper_bound(k);
                if (succ_it != succ_tree.end()) {
                    return *succ_it;
                }
            }
        }
        if (xfast.Find(k)) {
            auto& exact_tree = data.at(k);
            auto exact_it = exact_tree.upper_bound(k);
            if (exact_it != exact_tree.end()) {
                return *exact_it;
            }
            succ_opt = xfast.Successor(k);
            if (succ_opt.has_value()) {
                auto& succ_tree = data.at(*succ_opt);
                auto succ_it = succ_tree.upper_bound(k);
                if (succ_it != succ_tree.end()) {
                    return *succ_it;
                }
            }
        }
        return {};
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor(std::size_t k) const {
        auto pred_opt = xfast.Predecessor(k);
        auto succ_opt = xfast.Successor(k);
        if (pred_opt.has_value()) {
            succ_opt = xfast.Successor(*pred_opt);
            if (succ_opt.has_value()) {
                auto& succ_tree = data.at(*succ_opt);
                auto succ_it = succ_tree.lower_bound(k);
                if (succ_it != succ_tree.begin()) {
                    --succ_it;
                    return *succ_it;
                }
            }
            auto& pred_tree = data.at(*pred_opt);
            auto pred_it = pred_tree.lower_bound(k);
            if (pred_it != pred_tree.begin()) {
                --pred_it;
                return *pred_it;
            }
            auto before_opt = xfast.Predecessor(*pred_opt);
            if (before_opt.has_value()) {
                auto& before_tree = data.at(*before_opt);
                return *before_tree.rbegin();
            }
        }
        if (succ_opt.has_value()) {
            auto& succ_tree = data.at(*succ_opt);
            if (succ_opt.has_value()) {
                auto succ_it = succ_tree.lower_bound(k);
                if (succ_it != succ_tree.begin()) {
                    --succ_it;
                    return *succ_it;
                }
            }
            pred_opt = xfast.Predecessor(*succ_opt);
            if (pred_opt.has_value()) {
                auto& pred_tree = data.at(*pred_opt);
                auto pred_it = pred_tree.lower_bound(k);
                if (pred_it != pred_tree.begin()) {
                    --pred_it;
                    return *pred_it;
                }
            }
        }
        if (xfast.Find(k)) {
            auto& exact_tree = data.at(k);
            auto exact_it = exact_tree.lower_bound(k);
            if (exact_it != exact_tree.begin()) {
                --exact_it;
                return *exact_it;
            }
            pred_opt = xfast.Predecessor(k);
            auto& pred_tree = data.at(*pred_opt);
            auto pred_it = pred_tree.lower_bound(k);
            if (pred_it != pred_tree.begin()) {
                --pred_it;
                return *pred_it;
            }
        }
        return {};
    }

    void Insert (std::size_t k) {
        auto succ_opt = xfast.Successor(k);
        auto pred_opt = xfast.Predecessor(k);
        if (succ_opt.has_value()) {
            pred_opt = xfast.Predecessor(*succ_opt);
            if (pred_opt.has_value()) {
                auto& pred_tree = data.at(*pred_opt);
                auto pred_it = pred_tree.upper_bound(k);
                if (pred_it != pred_tree.end()) {
                    pred_tree.insert(k);
                    SplitIfNecessary(*pred_opt, pred_tree);
                    return;
                }
            }
            auto& succ_tree = data.at(*succ_opt);
            succ_tree.insert(k);
            SplitIfNecessary(*succ_opt, succ_tree);
            return;
        }
        if (pred_opt.has_value()) {
            auto& pred_tree = data.at(*pred_opt);
            pred_tree.insert(k);
            SplitIfNecessary(*pred_opt, pred_tree);
            return;
        }

        // trie is empty
        xfast.Insert(k);
        data[k].insert(k);
    }

    void Delete (std::size_t k) {
        auto succ_opt = xfast.Successor(k);
        auto pred_opt = xfast.Predecessor(k);
        if (succ_opt.has_value()) {
            pred_opt = xfast.Predecessor(*succ_opt);
            if (pred_opt.has_value()) {
                auto& pred_tree = data.at(*pred_opt);
                if (pred_tree.contains(k)) {
                    pred_tree.erase(k);
                    MergeIfNecessary(*pred_opt, pred_tree);
                    return;
                }
            }
            auto& succ_tree = data.at(*succ_opt);
            if (succ_tree.contains(k)) {
                succ_tree.erase(k);
                MergeIfNecessary(*succ_opt, succ_tree);
                return;
            }
        }
        if (pred_opt.has_value()) {
            auto& pred_tree = data.at(*pred_opt);
            if (pred_tree.contains(k)) {
                pred_tree.erase(k);
                MergeIfNecessary(*pred_opt, pred_tree);
                return;
            }
            succ_opt = xfast.Successor(*pred_opt);
            if (succ_opt.has_value()) {
                auto& succ_tree = data.at(*succ_opt);
                if (succ_tree.contains(k)) {
                    succ_tree.erase(k);
                    MergeIfNecessary(*succ_opt, succ_tree);
                    return;
                }
            }
        }
        // k is not in trie
    }

};

int main() {
    YFastTrie<7> trie;

    constexpr std::size_t N = 100;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 1);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (std::size_t i = 0; i < N; i++) {
        trie.Insert(v[i]);
        std::cout << v[i] << ' ';
    }
    std::cout << '\n';

    auto curr = trie.Minimum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Successor(curr.value());
    }
    std::cout << '\n';

    curr = trie.Maximum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Predecessor(curr.value());
    }
    std::cout << '\n';

    sr::shuffle(v, gen);

    for (std::size_t i = 0; i < N - 10; i++) {
        trie.Delete(v[i]);
        std::cout << v[i] << ' ';
    }
    std::cout << '\n';

    curr = trie.Minimum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Successor(curr.value());
    }
    std::cout << '\n';

    curr = trie.Maximum();
    while (curr.has_value()) {
        std::cout << curr.value() << ' ';
        curr = trie.Predecessor(curr.value());
    }
    std::cout << '\n';

}