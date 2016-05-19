#ifndef TAKEPHOTO_H
#define TAKEPHOTO_H

#include <QDialog>
#include <QCameraImageCapture>
#include <QMessageBox>
#include <QCamera>
#include <QCameraInfo>
#include <QLayout>
#include <QCameraViewfinder>
#include <QFileDialog>
//image processing
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace Ui {
class TakePhoto;
}

class TakePhoto : public QDialog
{
    Q_OBJECT

public:
    explicit TakePhoto(QWidget *parent = 0);
    ~TakePhoto();
    bool cameraon = false;
    bool initialize = true;
    QList<QCameraInfo> cameras;
    QCameraViewfinder * vf;
    QCameraImageCapture * cic;
    QCamera * cam;

    void check_available_camera();
    void initTable();
    void errorMsg(QString msg = "");
signals:
     void showPic(QImage img);

private slots:
    void on_pushButtons_camstart_clicked();

    void on_pushButton_piccapture_clicked();

    void processImage(int i, QImage img);
    void deleteImage(int id, QString fileName);
    void readyForCapture(bool available);
    void checkCam(QCamera::Error error);


private:
    Ui::TakePhoto *ui;
    void closeEvent(QCloseEvent *end);
};

#endif // TAKEPHOTO_H
