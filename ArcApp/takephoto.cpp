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
    QImage scaledImg = img.scaledToWidth(300);
    emit showPic(scaledImg);

    cam->stop();
    close();
}


//BLOCK TO STORE IMAGE(delete right after taking a picture)
void TakePhoto::deleteImage(int id, QString fileName){

    qDebug()<<"\nsavedimage : "<<fileName;
    QFile file(fileName);
      QFileInfo fi(file);
      qint64 fileSize = fi.size();
      if (! file.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) return;

      QByteArray block(65536, '\0');
      while (fileSize > 0) {
        if (fileSize < block.size()) block.truncate(fileSize);
        qint64 written = file.write(block);
        if (written != block.size()) return;
        fileSize -= written;
      }
      Q_ASSERT(fileSize == 0);
      file.remove();
      return;
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

        connect(cic,SIGNAL(imageCaptured(int,QImage)),this,SLOT(processImage(int,QImage)));
        connect(cic, SIGNAL(imageSaved(int,QString)),this, SLOT(deleteImage(int, QString)));
        connect(cic, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));

        connect(cam, SIGNAL(error(QCamera::Error)), this, SLOT(checkCam(QCamera::Error)));
        cam->setCaptureMode(QCamera::CaptureStillImage);
        qDebug()<<"CAMERA STATUS 3: " <<cam->status();

        cam->start();
        cameraon = true;
        ui->pushButtons_camstart->setText("Cam OFF");
  //      ui->pushButton_piccapture->setEnabled(true);
    }
    else{
        ui->verticalLayout->removeWidget(vf);
        vf->close();
        cic->destroyed();
        cam->stop();
        cam->destroyed();
        cameraon = false;
        ui->pushButtons_camstart->setText("Cam ON");
   //     ui->pushButton_piccapture->setEnabled(false);
    }

}

//need to fix;;;
void TakePhoto::checkCam(QCamera::Error error){
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

void TakePhoto::readyForCapture(bool available){
    ui->pushButton_piccapture->setEnabled(available);
}

void TakePhoto::on_pushButton_piccapture_clicked()
{
    cam->searchAndLock();
    cic->capture();
}
