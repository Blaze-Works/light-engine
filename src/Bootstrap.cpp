#include <Bootstrap.hpp>
#include <RunArgs.hpp>
#include <LightEngine.hpp>

namespace blaze::lightEngine {

int bootstrapMain(int argc, char** argv) {
    (void)argc;
    (void)argv;

    RunArgs runArgs(
        WindowSettings{800, 600, 1366, 768, false},
        DirectorySet{".", "./resources/", "/resources/"}
    );

    LightEngine engine(std::move(runArgs));
    engine.run();
    return 0;
}

} // namespace blaze::lightEngine

