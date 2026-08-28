#include "app/ControlPanel.hpp"

#include "sim/FireParameters.hpp"
#include "tests_common.h"

#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QObject>
#include <QPushButton>
#include <QSlider>

namespace {
using fire_tests::check;

QSlider* findSlider(ControlPanel& panel, const int minimum, const int maximum) {
    const auto sliders = panel.findChildren<QSlider*>();
    for (QSlider* const slider : sliders) {
        if (slider->minimum() == minimum && slider->maximum() == maximum) {
            return slider;
        }
    }

    return nullptr;
}

QLabel* valueLabelFor(const QSlider& slider) {
    const int sliderIndex = slider.parentWidget()->layout()->indexOf(const_cast<QSlider*>(&slider));
    return qobject_cast<QLabel*>(slider.parentWidget()->layout()->itemAt(sliderIndex + 1)->widget());
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

    QLabel* const sourceHeatValueLabel = valueLabelFor(*sourceHeatSlider);
    QLabel* const coolingValueLabel = valueLabelFor(*coolingSlider);
    check(sourceHeatValueLabel != nullptr && sourceHeatValueLabel->text() == QStringLiteral("43%"),
          "the source heat label shows its initial value as a percentage of the slider range");
    check(coolingValueLabel != nullptr && coolingValueLabel->text() == QStringLiteral("29%"),
          "the cooling label shows its initial value as a percentage of the slider range");

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
    check(sourceHeatValueLabel != nullptr && sourceHeatValueLabel->text() == QStringLiteral("57%"),
          "the source heat percentage follows slider edits");

    coolingSlider->setValue(5);
    check(changeCount == 2, "a cooling edit emits one whole-value change");
    check(reportedParameters.sourceHeat() == 160 && reportedParameters.cooling() == 5,
          "a cooling edit preserves the current source heat value");
    check(coolingValueLabel != nullptr && coolingValueLabel->text() == QStringLiteral("57%"),
          "the cooling percentage follows slider edits");

    FireParameters acceptedParameters;
    acceptedParameters.setSourceHeat(208);
    acceptedParameters.setCooling(7);
    panel.setParameters(acceptedParameters);

    check(changeCount == 2, "applying accepted parameters does not feed changes back to the controller");
    check(sourceHeatSlider->value() == 208 && coolingSlider->value() == 7,
          "applying accepted parameters updates both sliders");
    check(sourceHeatValueLabel != nullptr && sourceHeatValueLabel->text() == QStringLiteral("79%") &&
              coolingValueLabel != nullptr && coolingValueLabel->text() == QStringLiteral("86%"),
          "applying accepted parameters updates both percentage labels");

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

void testPalettePresetBinding() {
    ControlPanel panel{FireParameters{}, FirePalettePresetId::ArcaneBloom};
    auto* const paletteComboBox = panel.findChild<QComboBox*>(QStringLiteral("paletteComboBox"));
    check(paletteComboBox != nullptr, "the panel exposes a palette dropdown");
    if (paletteComboBox == nullptr) {
        return;
    }

    check(paletteComboBox->count() == 3, "the palette dropdown contains every built-in preset");
    check(paletteComboBox->currentText() == QStringLiteral("Arcane Bloom"),
          "the palette dropdown reflects the initial preset");

    for (const QLabel* const label : panel.findChildren<QLabel*>()) {
        check(label->text() != QStringLiteral("Palette"), "the palette dropdown has no separate text label");
    }

    int changeCount = 0;
    FirePalettePresetId reportedPreset = FirePalettePresetId::Classic;
    QObject::connect(
        &panel, &ControlPanel::palettePresetChanged, [&changeCount, &reportedPreset](const FirePalettePresetId preset) {
            ++changeCount;
            reportedPreset = preset;
        });

    paletteComboBox->setCurrentIndex(paletteComboBox->findData(static_cast<int>(FirePalettePresetId::Ghostlight)));
    check(changeCount == 1 && reportedPreset == FirePalettePresetId::Ghostlight,
          "selecting a dropdown item emits its stable palette ID");

    panel.setPalettePreset(FirePalettePresetId::Classic);
    check(paletteComboBox->currentText() == QStringLiteral("Classic"),
          "applying an accepted preset updates the dropdown");
    check(changeCount == 1, "synchronizing the accepted preset does not feed back to the controller");
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication application{argc, argv};
    application.setQuitOnLastWindowClosed(false);

    testParameterValueBinding();
    testMetricsToggleBinding();
    testPalettePresetBinding();

    return fire_tests::reportResults("control panel");
}
