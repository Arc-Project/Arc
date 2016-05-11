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
    if(!cameraon){
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
        cic->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
        connect(cam, SIGNAL(stateChanged(QCamera::State)),this, SLOT(checkCam(QCamera::Status)));
        connect(cic,SIGNAL(imageCaptured(int,QImage)),this,SLOT(processImage(int,QImage)));
        connect(cam, SIGNAL(error(QCamera::Error)), this, SLOT(checkCam(QCamera::Error)));
        cam->setCaptureMode(QCamera::CaptureStillImage);
        qDebug()<<"CAMERA STATUS 3: " <<cam->status();

        cam->start();
        cameraon = true;
        ui->pushButtons_camstart->setText("Cam OFF");
        ui->pushButton_piccapture->setEnabled(true);
    }
    else{
        ui->verticalLayout->removeWidget(vf);
        vf->close();
        cic->destroyed();
        cam->destroyed();
        cameraon = false;
        ui->pushButtons_camstart->setText("Cam ON");
        ui->pushButton_piccapture->setEnabled(false);
    }

}

//need to fix;;;
void TakePhoto::checkCam(QCamera::Status error){
    /*
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
*/
    switch(error){
        case QCamera::CameraError:
            qDebug()<<"Camera ERROR OCCURED";
            break;
        case QCamera::InvalidRequestError:
            qDebug()<<"System resource doesn't support requested functionality.";
            break;
        case QCamera::ServiceMissingError:
            qDebug()<<"No camera service available.";
            break;
        case QCamera::NotSupportedFeatureError:
            qDebug()<<"The feature is not supported.";
            break;
    }
}

void TakePhoto::on_pushButton_piccapture_clicked()
{
    cam->searchAndLock();
    cic->capture();
}
