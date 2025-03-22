#include "application/application.h"
#include "remote_logger/remote_logger.h"

int main() {
    Application app{};

    while(!app.should_shutdown()) {
        app.run();
    }

    log_info() << "Application shutdowned" << std::endl;

    app.shutdown();
}
