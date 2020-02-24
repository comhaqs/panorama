#ifndef UTILITYTOOL_H
#define UTILITYTOOL_H

#include <QString>
#include <sstream>

void write_log(const std::string& tag, const std::string& msg);

#define CODE_TO_UI(T) (QString::fromLocal8Bit(T).toUtf8())

#define LOG_INFO(MSG) {std::stringstream tmp_stream;tmp_stream<<MSG;write_log("info", tmp_stream.str());}
#define LOG_ERROR(MSG) {std::stringstream tmp_stream;tmp_stream<<MSG;write_log("error", tmp_stream.str());}

#endif // UTILITYTOOL_H
