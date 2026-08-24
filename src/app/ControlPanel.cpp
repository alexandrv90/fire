#include "app/ControlPanel.hpp"

#include "sim/FireParameters.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

namespace {
QLabel* makeValueLabel(QWidget* const parent, const int initialValue) {
    auto* const label = new QLabel(QString::number(initialValue), parent);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMinimumWidth(28);
    return label;
}

QSlider* addSlider(QHBoxLayout& layout,
                   QWidget* const parent,
                   const QString& title,
                   const int minimum,
                   const int maximum,
                   const int initialValue) {
    auto* const titleLabel = new QLabel(title, parent);
    auto* const slider = new QSlider(Qt::Horizontal, parent);
    auto* const valueLabel = makeValueLabel(parent, initialValue);
    slider->setRange(minimum, maximum);
    slider->setValue(initialValue);
    slider->setMinimumWidth(90);

    QObject::connect(
        slider, &QSlider::valueChanged, valueLabel, [valueLabel](const int value) { valueLabel->setNum(value); });

    layout.addWidget(titleLabel);
    layout.addWidget(slider, 1);
    layout.addWidget(valueLabel);
    return slider;
}
} // namespace

ControlPanel::ControlPanel(const FireParameters& parameters, QWidget* const parent) : QWidget(parent) {
    auto* const controlsLayout = new QHBoxLayout(this);
    controlsLayout->setContentsMargins(12, 8, 12, 8);

    pauseButton = new QPushButton(QStringLiteral("Pause"), this);
    auto* const resetButton = new QPushButton(QStringLiteral("Reset"), this);
    pauseButton->setToolTip(QStringLiteral("Pause or resume (Space)"));
    resetButton->setToolTip(QStringLiteral("Restart the simulation (R)"));
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(resetButton);
    controlsLayout->addSpacing(8);

    auto* const sourceHeatSlider = addSlider(*controlsLayout,
                                             this,
                                             QStringLiteral("Heat"),
                                             FireParameters::MINIMUM_SOURCE_HEAT,
                                             FireParameters::MAXIMUM_SOURCE_HEAT,
                                             parameters.sourceHeat());
    controlsLayout->addSpacing(8);
    auto* const coolingSlider = addSlider(*controlsLayout,
                                          this,
                                          QStringLiteral("Cooling"),
                                          FireParameters::MINIMUM_COOLING,
                                          FireParameters::MAXIMUM_COOLING,
                                          parameters.cooling());

    connect(pauseButton, &QPushButton::clicked, this, &ControlPanel::toggleRequested);
    connect(resetButton, &QPushButton::clicked, this, &ControlPanel::resetRequested);
    connect(sourceHeatSlider, &QSlider::valueChanged, this, &ControlPanel::sourceHeatChanged);
    connect(coolingSlider, &QSlider::valueChanged, this, &ControlPanel::coolingChanged);
}

void ControlPanel::setPaused(const bool paused) {
    pauseButton->setText(paused ? QStringLiteral("Resume") : QStringLiteral("Pause"));
}
