/**
 * A simple greeter function, but this time in C++
 */

#include <string>

std::string greet(std::string name) {
    return "Hello, " + name + ", and welcome to C++!";
}

int factorial(int n) {
    int result{1};
    while(n > 1) {
        result *= n;
        n--;
    }
    return result;
}
