# Testing

This is a simple, custom-made testing "framework". It is used to "unit-test" different components of the application.

## Usage

For each component to be tested, there should be created a new test class. This class will be inherited from the "Test" class, an abstract class that contains the basic functionality of the test classes.

Each `testclass` must be then pushed to the back of the `tests` vector. Then, the driver program will run the tests, and at the end, will show how many tests have failed and will set the return code of the main function accordingly.