#include "UtilityTool.h"
#include <QDebug>


void write_log(const std::string& tag, const std::string& msg){
    qDebug()<<msg.c_str();
}
