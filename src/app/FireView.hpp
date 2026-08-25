#pragma once

#include "render/PixelBuffer.hpp"

#include <QImage>
#include <QWidget>

class FireView final : public QWidget {
public:
    explicit FireView(QWidget* parent = nullptr);

    void present(const PixelBuffer& pixels);

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QImage frame;
};
