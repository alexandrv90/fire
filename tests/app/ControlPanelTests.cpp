#include "app/ControlPanel.hpp"

#include "sim/FireParameters.hpp"

#include <QApplication>
#include <QObject>
#include <QPushButton>
#include <QSlider>

#include <iostream>
#include <string_view>

namespace {
int failureCount = 0;

void check(const bool condition, const std::string_view message) {
    if (condition) {
        return;
    }

    std::cerr << "FAILED: " << message << '\n';
    ++failureCount;
}

QSlider* findSlider(ControlPanel& panel, const int minimum, const int maximum) {
    const auto sliders = panel.findChildren<QSlider*>();
    for (QSlider* const slider : sliders) {
        if (slider->minimum() == minimum && slider->maximum() == maximum) {
            return slider;
        }
    }

    return nullptr;
}

void testParameterValueBinding() {
    FireParameters initialParameters;
    initialParameters.setSourceHeat(128);
    initialParameters.setCooling(3);
    ControlPanel panel{initialParameters};

    QSlider* const sourceHeatSlider =
        findSlider(panel, FireParameters::MINIMUM_SOURCE_HEAT, FireParameters::MAXIMUM_SOURCE_HEAT);
    QSlider* const coolingSlider = findSlider(panel, FireParameters::MINIMUM_COOLING, FireParameters::MAXIMUM_COOLING);
    check(sourceHeatSlider != nullptr, "the panel exposes the source heat parameter range");
    check(coolingSlider != nullptr, "the panel exposes the cooling parameter range");
    if (sourceHeatSlider == nullptr || coolingSlider == nullptr) {
        return;
    }

    check(sourceHeatSlider->value() == 128 && coolingSlider->value() == 3,
          "the panel initializes both sliders from one parameter value");

    int changeCount = 0;
    FireParameters reportedParameters;
    QObject::connect(
        &panel, &ControlPanel::parametersChanged, [&changeCount, &reportedParameters](FireParameters value) {
            ++changeCount;
            reportedParameters = value;
        });

    sourceHeatSlider->setValue(160);
    check(changeCount == 1, "a source heat edit emits one whole-value change");
    check(reportedParameters.sourceHeat() == 160 && reportedParameters.cooling() == 3,
          "a source heat edit preserves the current cooling value");

    coolingSlider->setValue(5);
    check(changeCount == 2, "a cooling edit emits one whole-value change");
    check(reportedParameters.sourceHeat() == 160 && reportedParameters.cooling() == 5,
          "a cooling edit preserves the current source heat value");

    FireParameters acceptedParameters;
    acceptedParameters.setSourceHeat(208);
    acceptedParameters.setCooling(7);
    panel.setParameters(acceptedParameters);

    check(changeCount == 2, "applying accepted parameters does not feed changes back to the controller");
    check(sourceHeatSlider->value() == 208 && coolingSlider->value() == 7,
          "applying accepted parameters updates both sliders");

    sourceHeatSlider->setValue(192);
    check(changeCount == 3, "editing after read-back emits one new change");
    check(reportedParameters.sourceHeat() == 192 && reportedParameters.cooling() == 7,
          "editing after read-back preserves the other accepted parameter");
}

void testMetricsToggleBinding() {
    ControlPanel panel{FireParameters{}};
    auto* const metricsButton = panel.findChild<QPushButton*>(QStringLiteral("metricsButton"));
    check(metricsButton != nullptr, "the panel exposes a metrics button next to its controls");
    if (metricsButton == nullptr) {
        return;
    }

    int changeCount = 0;
    bool reportedEnabledState = false;
    QObject::connect(
        &panel, &ControlPanel::metricsEnabledChanged, [&changeCount, &reportedEnabledState](const bool enabled) {
            ++changeCount;
            reportedEnabledState = enabled;
        });

    panel.setMetricsEnabled(true);
    check(metricsButton->isChecked(), "the metrics button reflects enabled collection");
    check(changeCount == 0, "synchronizing enabled metrics does not feed back into the collector");

    metricsButton->click();
    check(changeCount == 1 && !reportedEnabledState, "clicking enabled metrics requests collection to stop");

    panel.setMetricsEnabled(false);
    check(!metricsButton->isChecked(), "the metrics button reflects disabled collection");
    check(changeCount == 1, "synchronizing disabled metrics does not feed back into the collector");

    metricsButton->click();
    check(changeCount == 2 && reportedEnabledState, "clicking disabled metrics requests collection to start");
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testParameterValueBinding();
    testMetricsToggleBinding();

    if (failureCount != 0) {
        std::cerr << failureCount << " control panel test assertion(s) failed\n";
        return 1;
    }

    std::cout << "All control panel tests passed\n";
    return 0;
}
