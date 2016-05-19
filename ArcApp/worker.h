#ifndef WORKER_H
#define WORKER_H
#include <QObject>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker();
    ~Worker();

    bool cont = true;

public slots:
    void process();

signals:
    void finished();
    void error(QString err);
    void shiftnochanged();

private:
    // add your variables here
};

#endif // WORKER_H
