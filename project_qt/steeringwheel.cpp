#include "steeringwheel.h"

#include <QPainter>


SteeringWheel::SteeringWheel(QWidget *parent) : QLabel(parent)
{
    originalPixmap.load(":/icon/image/4.png");
    //originalPixmap.load(":/icon/image/handle1.png");

    setAlignment(Qt::AlignCenter);
    setScaledContents(false);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void SteeringWheel::setAngle(int angle)
{

    QPixmap scaled = originalPixmap.scaled(
        width(),
        height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
       );

    QPixmap result(size());
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.translate(width() / 2, height() / 2);
    painter.rotate(angle);

    painter.drawPixmap(
        -scaled.width() / 2,
        -scaled.height() / 2,
        scaled
    );

    painter.end();

    setPixmap(result);
}