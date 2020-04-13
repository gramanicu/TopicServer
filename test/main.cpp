#include <iostream>
#include <vector>
#include "DatabaseTest.hpp"

/**
 * @brief Runs the tests
 * Will print how many tests passed.
 * @return int 0 if no tests failed, -1 if any test failed
 */
int main() {
    std::vector<testing::Test*> tests;

    // Add tests to be run
    tests.push_back(new testing::DatabaseTest());


    // Do not change code from here
    unsigned int cTest = 0;
    for (auto& i : tests) {
        if (i->run_tests()) {
            cTest++;
        }
    }

    std::cout << "Passed " << cTest << "/" << tests.size() << " tests!\n";
    if (cTest == tests.size()) {
        return 0;
    } else {
        return -1;
    }
}