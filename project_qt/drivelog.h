#ifndef DRIVELOG_H
#define DRIVELOG_H

#include <QDialog>

namespace Ui {
class DriveLog;
}

class DriveLog : public QDialog
{
    Q_OBJECT

public:
    explicit DriveLog(QWidget *parent = nullptr);
    ~DriveLog();
    void setDriveLog(
        const QString &startTime,
        const QString &endTime,
        const QString &driveTime,
        int maxSpeed,
        double avgSpeed,
        int leftCount,
        int rightCount,
        int temp,
        int humidity
        );

private:
    Ui::DriveLog *ui;
};

#endif // DRIVELOG_H
