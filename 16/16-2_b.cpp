#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <tuple>
#include <queue>
#include <functional>
#include <ranges>

namespace sr = std::ranges;

std::vector<size_t> minimizeAverageCompletion(std::vector<std::tuple<int, size_t, size_t>>& tasks) {
    sr::sort(tasks); // sort by release time - processing time - index
    auto [start_time, firstTime, firstIndex] = tasks.front();
    int end_time = std::get<0>(tasks.back());

    std::vector<size_t> taskOrder;
    std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<>> remainingTimes;
    remainingTimes.emplace(firstTime, firstIndex);
    for (int time = start_time; time <= end_time; time++) {
        auto [currProcess, currIndex] = remainingTimes.top();
        // process a task
        currProcess--;
        taskOrder.push_back(currIndex);
        remainingTimes.pop();
        if (currProcess) {
            remainingTimes.emplace(currProcess, currIndex);
        }
    }
    return taskOrder;
}

int main() {
}
