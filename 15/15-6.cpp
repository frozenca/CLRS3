#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

struct Company {
    struct Employee {
        double conviviality;
        size_t index;
        std::unique_ptr<Employee> first_subordinate;
        std::unique_ptr<Employee> next_coworker;

        Employee(double conviviality, size_t index) : conviviality {conviviality}, index {index} {}
    };

    std::unique_ptr<Employee> CEO;
};

double MaximumConviviality(const std::vector<typename Company::Employee*>& employees, size_t n) {
    std::vector<double> inclusive (n);
    std::vector<double> exclusive (n);

    for (size_t i = n - 1; i < n; i--) {
        double inc = 0;
        double exc = 0;
        auto curr_subordinate = employees[i]->first_subordinate.get();
        while (curr_subordinate) {
            exc += exclusive[curr_subordinate->index];
            inc += inclusive[curr_subordinate->index];
            curr_subordinate = curr_subordinate->next_coworker.get();
        }
        exclusive[i] = inc;
        inclusive[i] = std::max(exc + employees[i]->conviviality, inc);
    }
    return inclusive[0];
}

int main() {
    std::vector<typename Company::Employee*> emps;
    Company c;
    c.CEO = std::make_unique<typename Company::Employee>(-2, 0);
    emps.push_back(c.CEO.get());
    c.CEO->first_subordinate = std::make_unique<typename Company::Employee>(1, 1);
    emps.push_back(c.CEO->first_subordinate.get());
    c.CEO->first_subordinate->next_coworker = std::make_unique<typename Company::Employee>(1, 2);
    emps.push_back(c.CEO->first_subordinate->next_coworker.get());
    c.CEO->first_subordinate->first_subordinate = std::make_unique<typename Company::Employee>(4, 3);
    emps.push_back(c.CEO->first_subordinate->first_subordinate.get());
    std::cout << MaximumConviviality(emps, 4);

}