#include <list>
#include <utility>

template <typename T>
struct ListSet {
    std::list<std::pair<T, ListSet&>> l;
};

template <typename T>
using Element = std::pair<T, ListSet<T>&>;

template <typename T>
ListSet<T> MakeSet(const T& key) {
    ListSet<T> ls;
    ls.l.emplace_back(key, ls);
    return ls;
}

template <typename T>
ListSet<T> FindSet(const Element<T>& elm) {
    return elm.second;
}

template <typename T>
ListSet<T> Union(ListSet<T>& ls1, ListSet<T>& ls2) {
    if (ls1.l.size() < ls2.l.size()) {
        std::swap(ls1.l, ls2.l);
    }
    for (auto& [elm, refr] : ls2.l) {
        refr = ls1;
    }
    ls1.l.splice(ls1.l.end(), ls2.l);
    return ls1;
}

int main() {

}