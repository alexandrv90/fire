#include "app/FireController.hpp"

#include <QCoreApplication>

#include <iostream>
#include <string_view>
#include <vector>

namespace {
int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

void testSuspensionPreservesRequestedRunState() {
    FireController controller;
    std::vector<bool> runningTransitions;
    QObject::connect(&controller,
                     &FireController::runningChanged,
                     &controller,
                     [&runningTransitions](const bool running) { runningTransitions.push_back(running); });

    check(!controller.isRunRequested(), "the controller starts logically paused");
    check(!controller.isRunning(), "the controller starts with its wake timer stopped");
    check(!controller.isSuspended(), "the controller starts unsuspended");

    controller.run();
    check(controller.isRunRequested(), "run records the requested running state");
    check(controller.isRunning(), "run starts frame advancement while unsuspended");
    check(runningTransitions == std::vector<bool>{true}, "run reports the logical state transition");

    controller.setSuspended(true);
    check(controller.isRunRequested(), "suspension preserves the requested running state");
    check(!controller.isRunning(), "suspension stops frame advancement");
    check(controller.isSuspended(), "suspension is observable");
    check(runningTransitions == std::vector<bool>{true}, "suspension does not report a user pause");

    controller.setSuspended(false);
    check(controller.isRunRequested(), "restore preserves the requested running state");
    check(controller.isRunning(), "restore resumes requested frame advancement");
    check(runningTransitions == std::vector<bool>{true}, "restore does not report a user resume");
}

void testPausedControllerStaysPausedAcrossSuspension() {
    FireController controller;
    controller.run();
    controller.pause();

    controller.setSuspended(true);
    controller.setSuspended(false);

    check(!controller.isRunRequested(), "restore preserves the requested paused state");
    check(!controller.isRunning(), "restore does not advance a user-paused controller");
    check(!controller.isSuspended(), "the controller leaves suspension after restore");
}

void testRunRequestWhileSuspendedWaitsForRestore() {
    FireController controller;
    controller.setSuspended(true);
    controller.run();

    check(controller.isRunRequested(), "run can be requested while suspended");
    check(!controller.isRunning(), "a suspended controller defers requested advancement");

    controller.setSuspended(false);
    check(controller.isRunning(), "restore starts advancement requested during suspension");
}
} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication application{argc, argv};

    testSuspensionPreservesRequestedRunState();
    testPausedControllerStaysPausedAcrossSuspension();
    testRunRequestWhileSuspendedWaitsForRestore();

    if (failureCount != 0) {
        std::cerr << failureCount << " fire controller test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All fire controller tests passed\n";
    return 0;
}
