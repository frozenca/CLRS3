#include <cassert>
#include <random>
#include <utility>
#include <vector>

std::mt19937 gen(std::random_device{}());
std::bernoulli_distribution d(0.5);

std::vector<int> chips;

std::pair<bool, bool> report(size_t A, size_t B) {
    std::pair<bool, bool> res = {false, false};
    if (chips[A]) {
        res.first = chips[B];
    } else {
        res.first = d(gen);
    }
    if (chips[B]) {
        res.second = chips[A];
    } else {
        res.second = d(gen);
    }
    return res;
}


int main() {
    constexpr size_t NUM_CHIPS = 30;
    chips.resize(NUM_CHIPS);
    for (size_t i = 0; i < 1 + NUM_CHIPS / 2; i++) {
        chips[i] = 1;
    }
    std::shuffle(chips.begin(), chips.end(), gen);
    std::vector<int> predictions(chips.size());
    for (size_t i = 0; i < NUM_CHIPS - 1; i++) {
        auto [resA, resB] = report(i, i + 1);
        if (resA && resB) { // report say that both is good.
            predictions[i] = 1; // take the first, discard the second
        } else {
            i++; // discard both of them
        }
    }
    size_t last_good = 0;
    for (size_t i = NUM_CHIPS - 1; i < NUM_CHIPS; i--) {
        if (predictions[i]) {
            last_good = i;
            break;
        }
    }
    for (size_t i = 0; i < NUM_CHIPS; i++) {
        if (i == last_good) {
            continue;
        }
        bool resI = report(last_good, i).first;
        predictions[i] = resI;
    }
    assert(predictions == chips);

}