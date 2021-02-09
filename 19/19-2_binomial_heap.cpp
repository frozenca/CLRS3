#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
class BinomialHeap {
    static_assert(std::is_arithmetic_v<T>);
    struct Node {
        Node* parent = nullptr;
        T key;
        std::vector<std::unique_ptr<Node>> child;
        Node (const T& key) : key {key} {}

        void addSubtree(std::unique_ptr<Node> subtree) noexcept {
            assert(subtree->child.size() == child.size());
            subtree->parent = this;
            child.push_back(std::move(subtree));
        }
    };

    std::vector<std::unique_ptr<Node>> trees;

    std::unique_ptr<Node> MergeTrees(std::unique_ptr<Node> node1, std::unique_ptr<Node> node2) noexcept {
        assert(node1->child.size() == node2->child.size());
        if (node1->key > node2->key) {
            std::swap(node1, node2);
        }
        node1->addSubtree(std::move(node2));
        return std::move(node1);
    }

public:
    void Merge(BinomialHeap& other_heap) noexcept {
        if (other_heap.trees.size() > trees.size()) {
            std::swap(other_heap.trees, trees);
        }
        other_heap.trees.resize(trees.size());
        std::unique_ptr<Node> curr_node;
        for (std::size_t i = 0; i < trees.size(); i++) {
            if (curr_node) {
                if (trees[i]) {
                    if (other_heap.trees[i]) {
                        trees[i] = MergeTrees(std::move(trees[i]), std::move(other_heap.trees[i]));
                        std::swap(trees[i], curr_node);
                    } else {
                        curr_node = MergeTrees(std::move(curr_node), std::move(trees[i]));
                    }
                } else if (other_heap.trees[i]) {
                    curr_node = MergeTrees(std::move(curr_node), std::move(other_heap.trees[i]));
                } else {
                    trees[i] = std::move(curr_node);
                }
            } else if (trees[i] && other_heap.trees[i]) {
                curr_node = MergeTrees(std::move(trees[i]), std::move(other_heap.trees[i]));
            } else if (other_heap.trees[i]) {
                std::swap(trees[i], other_heap.trees[i]);
            }
        }
        if (curr_node) {
            trees.push_back(std::move(curr_node));
        }
    }

    void Insert(const T& key) noexcept {
        BinomialHeap temp;
        auto node = std::make_unique<Node>(key);
        temp.trees.push_back(std::move(node));
        Merge(temp);
    }

    [[nodiscard]] std::optional<T> FindMinimum() const {
        if (trees.empty()) {
            return {};
        }
        std::optional<T> res;
        for (auto&& root : trees) {
            if (root) {
                if (res.has_value()) {
                    res = std::min(root->key, res.value());
                } else {
                    res = root->key;
                }
            }
        }
        return res;
    }

    void DeleteMinimum() noexcept {
        if (trees.empty()) {
            return;
        }

        auto it = sr::find_if(trees, [](auto&& root) {return root != nullptr;});
        if (it == trees.end()) {
            return;
        }

        for (auto it2 = it; it2 != trees.end(); ++it2) {
            if (*it2 && (*it2)->key < (*it)->key) {
                it = it2;
            }
        }

        BinomialHeap temp;
        temp.trees = std::move((*it)->child);
        *it = nullptr;
        Merge(temp);
    }

    void DecreaseKey(Node* x, const T& key) {
        if (!x || x->key < key) {
            return;
        }
        x->key = key;
        while (x->parent && x->key < x->parent->key) {
            std::swap(x->key, x->parent->key);
            x = x->parent;
        }
    }

    void Delete(Node* x) {
        if (!x) {
            return;
        }
        DecreaseKey(x, std::numeric_limits<T>::lowest());
        DeleteMinimum();
    }

};


int main() {

    BinomialHeap<int> bheap;

    for (int i = 1; i <= 100; i++) {
        bheap.Insert(i);
    }
    for (int i = 1; i <= 100; i++) {
        bheap.DeleteMinimum();
    }


}