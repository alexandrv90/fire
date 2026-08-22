
#include <iostream>
#include <span>

int main(int argc, char* argv[]) {
    const std::span args(argv, static_cast<std::size_t>(argc));

    std::cout << "Argument count: " << args.size() << std::endl;
    for (std::size_t i = 0; i < args.size(); ++i) {
        std::cout << "  [" << i << "]\t" << args[i] << std::endl;
    }

    return 0;
}
