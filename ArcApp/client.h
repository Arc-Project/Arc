#ifndef CLIENT_H
#define CLIENT_H
#include <QString>

class Client
{
public:
    Client();
    QString clientId;
    QString fName, mName, lName;
    double balance;
    QString fullName;

};

#endif // CLIENT_H
