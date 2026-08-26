#include "app/StatsPanel.hpp"

#include "app/FrameMetricsCollector.hpp"
#include "app/Utils.hpp"
#include "tests_common.h"

#include <QApplication>
#include <QLabel>
#include <QPalette>
#include <QTimer>
#include <QWidget>

#include <chrono>

namespace {
using namespace std::chrono_literals;

using fire_tests::check;

void testInitialSnapshotPresentation() {
    FrameMetricsCollector collector;
    collector.setEnabled(true);

    const auto start = MetricsClock::time_point{};
    collector.observeWake(start);
    collector.observeWake(start + 10ms);
    collector.observeAdvance(FrameReport{2, 20ms, 1ms, 7, FrameStageTimings{3ms, 4ms}});
    collector.observeAdvance(FrameReport{0, 10ms, 2ms, 7, std::nullopt});
    collector.observePaint(start + 1ms, 5ms);
    collector.observePaint(start + 17ms, 7ms);

    QWidget renderOverlay;
    StatsPanel panel{collector, &renderOverlay};

    check(!panel.isHidden(), "the stats panel starts visible");
    check(panel.testAttribute(Qt::WA_OpaquePaintEvent), "the stats panel declares an opaque paint area");
    check(panel.palette().color(QPalette::Window).alpha() == 255, "the stats panel background is fully opaque");

    const auto* const refreshTimer = panel.findChild<QTimer*>(QStringLiteral("statsRefreshTimer"));
    check(refreshTimer != nullptr && refreshTimer->isActive() && refreshTimer->interval() == 250,
          "the stats panel refreshes independently at 4 Hz");

    const auto* const windowSummaryLabel = panel.findChild<QLabel*>(QStringLiteral("windowSummaryLabel"));
    const auto* const metricRowsLabel = panel.findChild<QLabel*>(QStringLiteral("metricRowsLabel"));
    const auto* const frameSummaryLabel = panel.findChild<QLabel*>(QStringLiteral("frameSummaryLabel"));
    check(windowSummaryLabel != nullptr, "the stats panel exposes its rolling window summary");
    check(metricRowsLabel != nullptr, "the stats panel exposes fixed metric rows");
    check(frameSummaryLabel != nullptr, "the stats panel exposes frame activity");
    if (windowSummaryLabel == nullptr || metricRowsLabel == nullptr || frameSummaryLabel == nullptr) {
        return;
    }

    check(windowSummaryLabel->text() == QStringLiteral("Window 0.03 s | 2 samples"),
          "the first row reports the activity window and sample count");

    const QString rows = metricRowsLabel->text();
    check(!rows.section(QLatin1Char('\n'), 0, 0).contains(QLatin1Char('n')),
          "the metric table drops the sample column");
    check(rows.contains(QStringLiteral("Simulate")) && rows.contains(QStringLiteral("3.00")),
          "the simulation row displays its statistics");
    check(rows.contains(QStringLiteral("Shade")) && rows.contains(QStringLiteral("4.00")),
          "the shade row displays its statistics");
    check(rows.contains(QStringLiteral("Wake interval")) && rows.contains(QStringLiteral("10.00")),
          "the wake interval row displays its statistics");
    check(rows.contains(QStringLiteral("Paint")) && rows.contains(QStringLiteral("6.00")),
          "the paint row displays its average statistics");
    check(rows.contains(QStringLiteral("Paint interval")) && rows.contains(QStringLiteral("16.00")),
          "the paint interval row displays its statistics");

    check(frameSummaryLabel->text() == QStringLiteral("Frame 7 | Dropped 3.00 ms | Idle wakes 1"),
          "the final row displays frame count and accumulated wake activity");
}

void testDisabledCollectionHidesPanel() {
    FrameMetricsCollector collector;
    QWidget renderOverlay;
    StatsPanel panel{collector, &renderOverlay};

    auto* const refreshTimer = panel.findChild<QTimer*>(QStringLiteral("statsRefreshTimer"));
    if (refreshTimer == nullptr) {
        check(false, "the stats panel owns its refresh timer");
        return;
    }

    check(panel.isHidden(), "the stats panel is hidden while collection is disabled");
    check(!refreshTimer->isActive(), "the stats panel does not refresh while collection is disabled");

    collector.setEnabled(true);
    check(!panel.isHidden(), "enabling collection shows the stats panel");
    check(refreshTimer->isActive(), "enabling collection starts stats refreshes");

    collector.setEnabled(false);
    check(panel.isHidden(), "disabling collection hides the stats panel");
    check(!refreshTimer->isActive(), "disabling collection stops stats refreshes");
}

void testSuspensionStopsRefreshesWithoutChangingEnablement() {
    FrameMetricsCollector collector;
    collector.setEnabled(true);
    QWidget renderOverlay;
    StatsPanel panel{collector, &renderOverlay};

    auto* const refreshTimer = panel.findChild<QTimer*>(QStringLiteral("statsRefreshTimer"));
    if (refreshTimer == nullptr) {
        check(false, "the stats panel owns its refresh timer");
        return;
    }

    panel.setSuspended(true);
    check(collector.isEnabled(), "suspending stats preserves metrics enablement");
    check(!panel.isHidden(), "suspending stats preserves panel visibility preference");
    check(!refreshTimer->isActive(), "suspending stats stops refreshes");

    collector.setEnabled(false);
    collector.setEnabled(true);
    check(!refreshTimer->isActive(), "enabling metrics while suspended does not start refreshes");

    panel.setSuspended(false);
    check(refreshTimer->isActive(), "restoring an enabled stats panel restarts refreshes");

    collector.setEnabled(false);
    panel.setSuspended(true);
    panel.setSuspended(false);
    check(!refreshTimer->isActive(), "restoring a disabled stats panel leaves refreshes stopped");
}

void checkRect(const FitRect& actual, const FitRect& expected, const std::string_view message) {
    check(actual.x == expected.x && actual.y == expected.y && actual.width == expected.width &&
              actual.height == expected.height,
          message);
}

void testViewport() {
    checkRect(fitPreservingAspect(800, 600, 640, 480), {0, 0, 800, 600}, "viewport preserves a matching aspect");
    checkRect(fitPreservingAspect(100, 100, 16, 9), {0, 22, 100, 56}, "viewport letterboxes a wide source");
    checkRect(fitPreservingAspect(100, 100, 9, 16), {22, 0, 56, 100}, "viewport pillarboxes a tall source");
    checkRect(fitPreservingAspect(101, 100, 2, 1), {0, 25, 101, 50}, "viewport centres an odd-sized fit");
    checkRect(fitPreservingAspect(0, 100, 16, 9), {}, "viewport rejects an empty available width");
    checkRect(fitPreservingAspect(100, 100, 0, 9), {}, "viewport rejects an empty source width");
    checkRect(fitPreservingAspect(-1, 100, 16, 9), {}, "viewport rejects negative dimensions");
    checkRect(fitPreservingAspect(std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max(),
                                  std::numeric_limits<int>::max()),
              {0, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
              "viewport handles maximum dimensions without overflow");
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testViewport();
    testInitialSnapshotPresentation();
    testDisabledCollectionHidesPanel();
    testSuspensionStopsRefreshesWithoutChangingEnablement();

    return fire_tests::reportResults("stats panel");
}
