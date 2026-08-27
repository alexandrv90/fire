#include "app/FireController.hpp"
#include "tests_common.h"

#include <QCoreApplication>

#include <vector>

namespace {
using fire_tests::check;

constexpr Dimensions TEST_DIMENSIONS{8, 6};

void testSuspensionPreservesRequestedRunState() {
    FireController controller{TEST_DIMENSIONS};
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
    FireController controller{TEST_DIMENSIONS};
    controller.run();
    controller.pause();

    controller.setSuspended(true);
    controller.setSuspended(false);

    check(!controller.isRunRequested(), "restore preserves the requested paused state");
    check(!controller.isRunning(), "restore does not advance a user-paused controller");
    check(!controller.isSuspended(), "the controller leaves suspension after restore");
}

void testRunRequestWhileSuspendedWaitsForRestore() {
    FireController controller{TEST_DIMENSIONS};
    controller.setSuspended(true);
    controller.run();

    check(controller.isRunRequested(), "run can be requested while suspended");
    check(!controller.isRunning(), "a suspended controller defers requested advancement");

    controller.setSuspended(false);
    check(controller.isRunning(), "restore starts advancement requested during suspension");
}

void testPaletteSelectionPublishesAnImmediateFrame() {
    FireController controller{TEST_DIMENSIONS};
    int frameReadyCount = 0;
    QObject::connect(&controller, &FireController::frameReady, &controller, [&frameReadyCount] { ++frameReadyCount; });

    controller.setPalettePreset(FirePalettePresetId::Ghostlight);
    check(controller.palettePreset() == FirePalettePresetId::Ghostlight,
          "the controller accepts a palette selection while paused");

    check(frameReadyCount == 1, "a palette selection publishes an immediately re-shaded frame");

    controller.reset();
    check(controller.palettePreset() == FirePalettePresetId::Ghostlight,
          "controller reset preserves the selected palette");
}
} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication application{argc, argv};

    testSuspensionPreservesRequestedRunState();
    testPausedControllerStaysPausedAcrossSuspension();
    testRunRequestWhileSuspendedWaitsForRestore();
    testPaletteSelectionPublishesAnImmediateFrame();

    return fire_tests::reportResults("fire controller");
}
