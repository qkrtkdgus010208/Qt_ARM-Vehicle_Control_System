#ifndef STEERINGWHEEL_H
#define STEERINGWHEEL_H

#include <QLabel>
#include <QPixmap>

class SteeringWheel : public QLabel
{
    Q_OBJECT

public:
    explicit SteeringWheel(QWidget *parent = nullptr);

public slots:
    void setAngle(int angle);

private:
    QPixmap originalPixmap;
};

#endif // STEERINGWHEEL_H