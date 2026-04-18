#include "core/Application.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        HillExplorer::Application app(argc, argv);

        if (!app.Initialize()) {
            return 0;
        }

        app.Run();
        app.Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}