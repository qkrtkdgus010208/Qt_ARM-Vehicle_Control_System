
#include "speeddial.h"

#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

SpeedDial::SpeedDial(QWidget *parent)
    : QDial(parent)
{
    setRange(0, 200);
    setValue(0);

    setWrapping(false);
    setNotchesVisible(false);

    setMinimumSize(220, 140);
}
void SpeedDial::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();


    // 다이얼 중심을 조금 위로 이동
    QPointF center(
        w / 2.0,
        h - 45.0
    );


    double radius = qMin(
        w * 0.42,
        (h - 40) * 0.8
    );


    // ===============================
    // 반원 배경
    // ===============================

    QRectF circleRect(
        center.x() - radius,
        center.y() - radius,
        radius * 2,
        radius * 2
    );


    painter.setBrush(QColor("#151B22"));
    painter.setPen(QPen(QColor("#26313D"), 3));

    painter.drawPie(
        circleRect,
        0,
        180 * 16
    );


    // ===============================
    // 눈금
    // ===============================

    painter.setPen(
        QPen(QColor("#607080"), 2)
    );


    for (int angle = -180;
         angle <= 180;
         angle += 15)
    {
        double rad = qDegreesToRadians((double)angle);


        QPointF p1(
            center.x()
                + (radius - 10) * qCos(rad),

            center.y()
                + (radius - 10) * qSin(rad)
        );


        QPointF p2(
            center.x()
                + radius * qCos(rad),

            center.y()
                + radius * qSin(rad)
        );


        painter.drawLine(p1, p2);
    }


    // ===============================
    // 현재 바늘
    // ===============================

    int dialValue = value();

    double angle = -180.0 + (dialValue / 200.0) * 180.0;
    double rad = qDegreesToRadians(angle);


    QPointF needleEnd(
        center.x()
            + (radius - 20) * qCos(rad),

        center.y()
            + (radius - 20) * qSin(rad)
    );


    painter.setPen(QPen( QColor("#00E5FF"),5,Qt::SolidLine, Qt::RoundCap));


    painter.drawLine(center,needleEnd);


    // ===============================
    // 중심 원
    // ===============================

    painter.setBrush( QColor("#00D9FF"));

    painter.setPen(Qt::NoPen);

    painter.drawEllipse(center,7,7);


    // ===============================
    // 값 문자열
    // ===============================

    QString text = QString("%1 km/h").arg(dialValue);


    // ===============================
    // 글씨
    // ===============================

    painter.setPen(
        QColor("#00E5FF")
    );


    QFont font =
        painter.font();

    font.setBold(true);
    font.setPointSize(11);

    painter.setFont(font);


    QRectF textRect(
        0,
        h - 28,
        w,
        24
    );


    painter.drawText(
        textRect,
        Qt::AlignCenter,
        text
    );
}

void SpeedDial::mousePressEvent(QMouseEvent *event)
{
    updateValueFromMouse(event->pos());
}

void SpeedDial::mouseMoveEvent(QMouseEvent *event)
{
    updateValueFromMouse(event->pos());
}

void SpeedDial::mouseReleaseEvent(QMouseEvent *event)
{
    updateValueFromMouse(event->pos());
}

void SpeedDial::updateValueFromMouse(const QPoint &pos)
{
    QPointF center(
        width() / 2.0,
        height() - 45.0
    );

    double dx = pos.x() - center.x();
    double dy = pos.y() - center.y();

    // 반원 아래쪽은 입력하지 않음
    if (dy > 0)
        return;

    // 마우스 위치 → 각도
    double angle = qRadiansToDegrees(qAtan2(dy, dx));

    // 윗쪽 반원에서는
    // 오른쪽 : 0도
    // 위쪽   : -90도
    // 왼쪽   : -180도

    if (angle > 0)
        angle = -180;

    // -180 ~ 0도를 0 ~ 200으로 변환
    int speed = qRound((angle + 180.0) / 180.0 * 200.0);

    speed = qBound(0, speed, 200);

    setValue(speed);
}