#include "takephoto.h"
#include "ui_takephoto.h"
#include <QDebug>


TakePhoto::TakePhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TakePhoto)
{
    ui->setupUi(this);
}

TakePhoto::~TakePhoto()
{
    delete ui;
}


void TakePhoto::processImage(int i, QImage img)
{
    qDebug()<<"processImg";
    emit showPic(img);
    cam->stop();
    close();
}

void TakePhoto::on_pushButtons_camstart_clicked()
{
    vf = new QCameraViewfinder(ui->verticalLayoutWidget);
    ui->verticalLayout->addWidget(vf);

    foreach(QCameraInfo info, QCameraInfo::availableCameras())
    {
            cam = new QCamera(info);
    }
    qDebug()<<"CAMERA STATUS 1: " <<cam->status();
    cam->setViewfinder(vf);
    qDebug()<<"CAMERA STATUS 2: " <<cam->status();
    cic = new QCameraImageCapture(cam);
    connect(cam, SIGNAL(stateChanged(QCamera::State)),this, SLOT(checkCam(QCamera::Status)));
    connect(cic,SIGNAL(imageCaptured(int,QImage)),this,SLOT(processImage(int,QImage)));

    cam->setCaptureMode(QCamera::CaptureStillImage);
    qDebug()<<"CAMERA STATUS 3: " <<cam->status();

    cam->start();

}

//need to fix;;;
void TakePhoto::checkCam(QCamera::Status status){
    switch(status){
        case QCamera::ActiveState:
        case QCamera::LoadedState:
            qDebug()<<"CAMERA READY ";
            ui->pushButtons_camstart->setEnabled(true);
            break;
        case QCamera::UnavailableStatus:
            qDebug()<<"Camera Unavailable";
            ui->pushButtons_camstart->setEnabled(false);
            break;
        default:
            qDebug()<<"ERROR RETRY";
            ui->pushButtons_camstart->setEnabled(false);
    }

}

void TakePhoto::on_pushButton_piccapture_clicked()
{
    cam->searchAndLock();
    cic->capture();
}
