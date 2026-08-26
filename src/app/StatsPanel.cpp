#include "app/StatsPanel.hpp"

#include "app/FrameMetricsCollector.hpp"

#include <QColor>
#include <QFontDatabase>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QSizePolicy>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>

#include <chrono>
#include <cstddef>

namespace {
QString formatMetricValue(const double value, const std::size_t sampleCount) {
    return sampleCount == 0 ? QStringLiteral("--") : QString::number(value, 'f', 2);
}

QString formatMetricRow(const QString& name, const MetricStatistics& statistics) {
    return QStringLiteral("%1 %2 %3 %4")
        .arg(name, -15)
        .arg(formatMetricValue(statistics.averageMilliseconds, statistics.sampleCount), 8)
        .arg(formatMetricValue(statistics.percentile95Milliseconds, statistics.sampleCount), 8)
        .arg(formatMetricValue(statistics.maximumMilliseconds, statistics.sampleCount), 8);
}

QString formatWindowSummary(const MetricStatistics& wakeInterval) {
    const double windowSeconds =
        wakeInterval.averageMilliseconds * static_cast<double>(wakeInterval.sampleCount) / 1000.0;
    return QStringLiteral("Window %1 s | %2 samples")
        .arg(windowSeconds, 0, 'f', 2)
        .arg(static_cast<qulonglong>(wakeInterval.sampleCount));
}
} // namespace

StatsPanel::StatsPanel(const FrameMetricsCollector& metricsCollector, QWidget* const parent)
    : QWidget(parent), metricsCollector(metricsCollector) {
    setObjectName(QStringLiteral("statsPanel"));
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QPalette panelPalette = palette();
    panelPalette.setColor(QPalette::Window, QColor{24, 24, 24});
    panelPalette.setColor(QPalette::WindowText, QColor{240, 240, 240});
    setPalette(panelPalette);

    auto* const panelLayout = new QVBoxLayout(this);
    panelLayout->setContentsMargins(12, 10, 12, 10);
    panelLayout->setSpacing(4);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QFont headingFont = fixedFont;
    headingFont.setBold(true);

    windowSummaryLabel = new QLabel(this);
    windowSummaryLabel->setObjectName(QStringLiteral("windowSummaryLabel"));
    windowSummaryLabel->setFont(headingFont);
    windowSummaryLabel->setTextFormat(Qt::PlainText);

    metricRowsLabel = new QLabel(this);
    metricRowsLabel->setObjectName(QStringLiteral("metricRowsLabel"));
    metricRowsLabel->setFont(fixedFont);
    metricRowsLabel->setTextFormat(Qt::PlainText);

    latestFrameLabel = new QLabel(this);
    latestFrameLabel->setObjectName(QStringLiteral("latestFrameLabel"));
    latestFrameLabel->setFont(fixedFont);
    latestFrameLabel->setTextFormat(Qt::PlainText);

    panelLayout->addWidget(windowSummaryLabel);
    panelLayout->addWidget(metricRowsLabel);
    panelLayout->addWidget(latestFrameLabel);

    refreshTimer = new QTimer(this);
    refreshTimer->setObjectName(QStringLiteral("statsRefreshTimer"));
    refreshTimer->setInterval(REFRESH_INTERVAL_MILLISECONDS);
    connect(refreshTimer, &QTimer::timeout, this, &StatsPanel::refresh);
    const auto applyEnabledState = [this](const bool enabled) {
        setVisible(enabled);
        updateRefreshTimer();
    };
    connect(&metricsCollector, &FrameMetricsCollector::enabledChanged, this, applyEnabledState);
    applyEnabledState(metricsCollector.isEnabled());
}

void StatsPanel::setSuspended(const bool suspended) {
    if (suspensionActive == suspended) {
        return;
    }

    suspensionActive = suspended;
    updateRefreshTimer();
}

void StatsPanel::paintEvent(QPaintEvent* const event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
}

void StatsPanel::refresh() {
    const FrameMetricsSnapshot snapshot = metricsCollector.snapshot();
    windowSummaryLabel->setText(formatWindowSummary(snapshot.wakeInterval));

    QStringList metricRows;
    metricRows.reserve(6);
    metricRows.append(QStringLiteral("%1 %2 %3 %4")
                          .arg(QStringLiteral("Metric"), -15)
                          .arg(QStringLiteral("avg"), 8)
                          .arg(QStringLiteral("p95"), 8)
                          .arg(QStringLiteral("max"), 8));
    metricRows.append(formatMetricRow(QStringLiteral("Simulate"), snapshot.simulateDuration));
    metricRows.append(formatMetricRow(QStringLiteral("Shade"), snapshot.shadeDuration));
    metricRows.append(formatMetricRow(QStringLiteral("Paint"), snapshot.paintDuration));
    metricRows.append(formatMetricRow(QStringLiteral("Paint interval"), snapshot.paintInterval));
    metricRows.append(formatMetricRow(QStringLiteral("Wake interval"), snapshot.wakeInterval));
    metricRowsLabel->setText(metricRows.join(QLatin1Char('\n')));

    if (!snapshot.latestFrame.has_value()) {
        latestFrameLabel->setText(QStringLiteral("Frame --"));
        return;
    }

    const FrameReport& frame = *snapshot.latestFrame;
    latestFrameLabel->setText(QStringLiteral("Frame %1").arg(static_cast<qulonglong>(frame.frameIndex)));
}

void StatsPanel::updateRefreshTimer() {
    const bool shouldRefresh = metricsCollector.isEnabled() && !suspensionActive;
    if (!shouldRefresh) {
        refreshTimer->stop();
        return;
    }

    refresh();
    refreshTimer->start();
}
