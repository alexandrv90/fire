#include "app/StatsPanel.hpp"

#include "app/FrameMetricsCollector.hpp"

#include <QApplication>
#include <QLabel>
#include <QPalette>
#include <QTimer>
#include <QWidget>

#include <chrono>
#include <iostream>
#include <string_view>

namespace {
using namespace std::chrono_literals;

int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

void testInitialSnapshotPresentation() {
    FrameMetricsCollector collector;
    collector.setEnabled(true);

    const auto start = MetricsClock::time_point{};
    collector.observeWake(start);
    collector.observeWake(start + 10ms);
    collector.observeFrame(FrameReport{2, 20ms, 1ms, 7, FrameStageTimings{3ms, 4ms}});
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
    const auto* const latestFrameLabel = panel.findChild<QLabel*>(QStringLiteral("latestFrameLabel"));
    check(windowSummaryLabel != nullptr, "the stats panel exposes its rolling window summary");
    check(metricRowsLabel != nullptr, "the stats panel exposes fixed metric rows");
    check(latestFrameLabel != nullptr, "the stats panel exposes the latest frame report");
    if (windowSummaryLabel == nullptr || metricRowsLabel == nullptr || latestFrameLabel == nullptr) {
        return;
    }

    check(windowSummaryLabel->text() == QStringLiteral("Window 0.01 s | 1 samples"),
          "the first row reports the retained wake window and sample count");

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

    const QString latestFrame = latestFrameLabel->text();
    check(latestFrame.contains(QStringLiteral("Frame 7")), "the panel displays the produced frame index");
    check(latestFrame.contains(QStringLiteral("Ticks 2")), "the panel displays ticks executed for the latest frame");
    check(latestFrame.contains(QStringLiteral("Elapsed 20.00 ms")), "the panel displays latest-frame elapsed time");
    check(latestFrame.contains(QStringLiteral("Discarded 1.00 ms")), "the panel displays latest-frame discarded time");
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
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testInitialSnapshotPresentation();
    testDisabledCollectionHidesPanel();

    if (failureCount != 0) {
        std::cerr << failureCount << " stats panel test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All stats panel tests passed\n";
    return 0;
}
