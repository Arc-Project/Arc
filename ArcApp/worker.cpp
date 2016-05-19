#include "worker.h"
#include <QThread>
#include "mainwindow.h"

// --- CONSTRUCTOR ---
Worker::Worker() {
    // you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}

// --- PROCESS ---
// Start processing data.
void Worker::process() {    
    // allocate resources using new here
    while (true) {
        // qDebug("Updating Shift No");
        emit this->shiftnochanged();
        QThread::sleep(1);
    }

    //emit finished();
}
