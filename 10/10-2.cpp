#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>
#include <list>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
struct MergeableHeap1 {
    std::list<T> data;

    void Insert(const T& key) {
        auto it = sr::find_if(data, [&key](auto& k) {return k > key;});
        data.insert(it, key);
    }

    T Minimum() {
        return data.front();
    }

    T ExtractMinimum() {
        T Min = data.front();
        data.pop_front();
        return Min;
    }

    MergeableHeap1<T> Merge(MergeableHeap1<T>& other) {
        data.merge(other);
        return *this;
    }
};

template <typename T>
struct MergeableHeap2 {
    std::list<T> data;

    void Insert(const T& key) {
        T curr_key = key;
        auto it = data.begin();
        while (true) {
            it = std::find_if(it, data.end(), [&curr_key](auto &k) { return k > curr_key; });
            if (it == data.end()) {
                data.insert(curr_key);
                return;
            }
            std::swap(*it, curr_key);
        }
    }

    T Minimum() {
        return data.front();
    }

    T ExtractMinimum() {
        T Min = data.front();
        T curr_key = Min;
        auto it = data.begin();
        while (true) {
            auto prev_it = it;
            it = std::find_if(it, data.end(), [&curr_key](auto &k) { return k < curr_key; });
            if (it == data.end()) {
                data.erase(prev_it);
                return;
            }
            std::swap(*it, curr_key);
        }
    }

    MergeableHeap2<T> Merge(MergeableHeap2<T>& other) {
        if (data.empty()) {
            return other;
        }
        auto it = data.begin();
        while (!other.empty()) {
            if (other.front() <= *it) {
                other.Insert(*it);
                *data.begin() = other.front();
                other.ExtractMinimum();
            }
            ++it;
        }
        data.splice(data.end(), other.data);
        return *this;
    }
};

int main() {

}