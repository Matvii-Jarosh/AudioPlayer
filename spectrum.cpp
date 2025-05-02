#include "spectrum.h"

Spectrum::Spectrum(QWidget *parent)
    : QWidget{parent}
{
    image = QImage(620, 250, QImage::Format_RGB32);
    image.fill(Qt::black);

    QPainter painter(&image);
    painter.setPen(Qt::blue);
    painter.drawRect(50, 50, 100, 100);
    painter.end();

    setFixedSize(image.size());
}

void Spectrum::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawImage(0, 0, image);
}
