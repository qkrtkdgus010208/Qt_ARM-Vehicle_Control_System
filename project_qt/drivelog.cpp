#include "drivelog.h"
#include "ui_drivelog.h"

DriveLog::DriveLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DriveLog)
{
    ui->setupUi(this);
    setWindowTitle("Drive Log");
    setModal(true);
    connect(ui->btnClose_2,&QPushButton::clicked,this,&QDialog::accept);
}

DriveLog::~DriveLog()
{
    delete ui;
}

void DriveLog::setDriveLog(
    const QString &startTime,
    const QString &endTime,
    const QString &driveTime,
    int maxSpeed,
    double avgSpeed,
    int leftCount,
    int rightCount,
    int temp,
    int humidity
)
{
    ui->lblStartTime->setText(startTime);
    ui->lblEndTime->setText(endTime);
    ui->lblDriveTime->setText(driveTime);

    ui->lblMaxSpeed->setText(
        QString("%1 Km/h").arg(maxSpeed)
    );

    ui->lblAvgSpeed->setText(
        QString("%1 Km/h").arg(avgSpeed, 0, 'f', 1)
    );

    ui->lblLeftCount->setText(
        QString("%1 회").arg(leftCount)
    );

    ui->lblRightCount->setText(
        QString("%1 회").arg(rightCount)
    );

    ui->lblTemp->setText(
        QString("%1 °C").arg(temp)
    );

    ui->lblHumidity->setText(
        QString("%1 %").arg(humidity)
    );
}
