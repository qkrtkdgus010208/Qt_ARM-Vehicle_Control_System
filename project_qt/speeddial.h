#ifndef SPEEDDIAL_H
#define SPEEDDIAL_H

#include <QDial>

class SpeedDial : public QDial
{
    Q_OBJECT

public:
    explicit SpeedDial(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void updateValueFromMouse(const QPoint &pos);
};

#endif
