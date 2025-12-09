#include <print>
#include <algorithm>
#include <ranges>
#include <string>

int main() {
    std::string hello = "Hello world!";
    std::ranges::for_each(hello, [](char c){ std::println("{}", c); });
};
