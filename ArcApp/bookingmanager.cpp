#include "bookingmanager.h"

bookingManager::bookingManager()
{

}
bool bookingManager::checkValidDate(QDate start, QDate end){
    if(start >= end)
        return false;
    return true;

}
