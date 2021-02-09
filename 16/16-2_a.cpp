#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

std::vector<size_t> minimizeAverageCompletion(std::vector<std::pair<size_t, size_t>>& tasks) {
    sr::sort(tasks);
    std::vector<size_t> taskOrder;
    taskOrder.reserve(tasks.size());
    for (const auto& [task, index] : tasks) {
        taskOrder.push_back(index);
    }
    return taskOrder;
}


int main() {
}
