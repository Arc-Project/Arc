#include "takephoto.h"
#include "ui_takephoto.h"
#include <QDebug>


TakePhoto::TakePhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TakePhoto)
{
    ui->setupUi(this);
    on_pushButtons_camstart_clicked();
}

TakePhoto::~TakePhoto()
{

    delete ui;
}


void TakePhoto::processImage(int i, QImage img)
{
    Q_UNUSED(i);
    qDebug()<<"processImg";
    QImage scaledImg = img.scaledToWidth(300);
    emit showPic(scaledImg);

    cam->stop();
    close();
}


//BLOCK TO STORE IMAGE(delete right after taking a picture)
void TakePhoto::deleteImage(int id, QString fileName){
    Q_UNUSED(id);
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

void TakePhoto::check_available_camera(){
   initTable();
   cameras = QCameraInfo::availableCameras();
   if(cameras.count() == 0)
       errorMsg("No available Camera");
   int row =0;
   foreach (const QCameraInfo &cameraInfo, cameras) {
       qDebug()<< "CAMERA: "<<cameraInfo.deviceName() <<cameraInfo.description();
       ui->tableWidget_cam_list->insertRow(row);
       ui->tableWidget_cam_list->setItem(row, 0, new QTableWidgetItem(cameraInfo.description()));
       row++;
   }

}

void TakePhoto::initTable(){
    errorMsg();
    ui->tableWidget_cam_list->setRowCount(0);
    ui->tableWidget_cam_list->setColumnCount(1);
    ui->tableWidget_cam_list->clear();
    ui->tableWidget_cam_list->setHorizontalHeaderLabels(QStringList()<<"CAMERA NAME");


}

void TakePhoto::on_pushButtons_camstart_clicked()
{
    errorMsg();
    if(!cameraon){
        ui->tableWidget_cam_list->hide();
        vf = new QCameraViewfinder(ui->verticalLayoutWidget);
        ui->verticalLayout->addWidget(vf);
        if(initialize){
             cam = new QCamera(QCameraInfo::defaultCamera());
             initialize = false;
        }
        else{
            int idx = ui->tableWidget_cam_list->currentRow();
            if(idx <0){
                errorMsg("Please Select Camera");
                ui->tableWidget_cam_list->show();
                return;
            }

            qDebug() <<"CAMERA INFO CHECK : " << cameras.at(idx).description();
            if(cameras.at(idx).isNull()){
                errorMsg("camera is unavailable.");
                return;
            }
            cam = new QCamera(cameras.at(idx));
        }
        cam->setViewfinder(vf);
        cic = new QCameraImageCapture(cam);

        connect(cic,SIGNAL(imageCaptured(int,QImage)),this,SLOT(processImage(int,QImage)));
        connect(cic, SIGNAL(imageSaved(int,QString)),this, SLOT(deleteImage(int, QString)));
        connect(cic, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
        connect(cam, SIGNAL(error(QCamera::Error)), this, SLOT(checkCam(QCamera::Error)));
        cam->setCaptureMode(QCamera::CaptureStillImage);


        cam->start();
        cameraon = true;
        ui->pushButtons_camstart->setText("Change Cam");
    }
    else{
        ui->verticalLayout->removeWidget(vf);
        vf->close();
        cic->destroyed();
        cam->stop();
        cam->destroyed();
        cameraon = false;
        ui->tableWidget_cam_list->show();
        ui->pushButtons_camstart->setText("Select Cam");
        check_available_camera();
    }

}

//need to fix;;;
void TakePhoto::checkCam(QCamera::Error error){
    QString msg;
    switch(error){
        case QCamera::NoError:
            break;
        case QCamera::CameraError:
            qDebug()<<"Camera ERROR OCCURED";
            msg = "Camera ERROR OCCURED";
            break;
        case QCamera::InvalidRequestError:
            qDebug()<<"System resource doesn't support requested functionality.";
            msg = "functionality is not supported.";
            break;
        case QCamera::ServiceMissingError:
            qDebug()<<"No camera service available.";
            msg = "No camera service available.";
            break;
        case QCamera::NotSupportedFeatureError:
            qDebug()<<"The feature is not supported.";
            msg = "The feature is not supported.";
            break;
    }
    errorMsg(msg);
}

void TakePhoto::errorMsg(QString msg){
    ui->label_cam_message->setText(msg);
}

void TakePhoto::readyForCapture(bool available){
    ui->pushButton_piccapture->setEnabled(available);
    if(!available){
        errorMsg("Camara is not available.");
    }
}

void TakePhoto::on_pushButton_piccapture_clicked()
{
    cam->searchAndLock();
    cic->capture();
}

void TakePhoto::closeEvent(QCloseEvent *end){
    cam->stop();
    QWidget::closeEvent(end);

}
