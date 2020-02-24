#ifndef DLGCHECK_H
#define DLGCHECK_H

#include <QDialog>
#include <opencv2/opencv.hpp>


namespace Ui {
class DlgCheck;
}

class DlgCheck : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCheck(QWidget *parent = nullptr);
    ~DlgCheck();

private slots:
    void on_pb_check_clicked();

    void on_pb_front_clicked();

private:
    Ui::DlgCheck *ui;

protected:
    virtual bool check_camera( cv::Mat& map_x, cv::Mat& map_y, std::vector<cv::Mat>& imgs, cv::Size& size);
};

#endif // DLGCHECK_H
