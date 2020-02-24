#include "DlgCheck.h"
#include "ui_DlgCheck.h"
#include "Configure.h"
#include <QFileDialog>
#include <QMessageBox>
#include "UtilityTool.h"
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

DlgCheck::DlgCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCheck)
{
    ui->setupUi(this);
    ui->le_front->setText(CODE_TO_UI("E:\\Projects\\qt\\panorama\\build-panorama-Desktop_Qt_5_14_0_MSVC2017_64bit-Debug\\bd"));
}

DlgCheck::~DlgCheck()
{
    delete ui;
}

void DlgCheck::on_pb_check_clicked()
{
    try{
        auto pconfig = Configure::get_instance();
        auto path_front = ui->le_front->text().toStdString();
        if(path_front.empty()){
            return;
        }
        std::vector<cv::Mat> imgs;
        boost::filesystem::path myPath(path_front);
        boost::filesystem::directory_iterator endIter;
        for (boost::filesystem::directory_iterator iter(myPath); iter != endIter; iter++) {
            if (boost::filesystem::is_directory(*iter)) {
                continue;
            } else {
                auto extent = iter->path().extension();
                if(".jpg" != extent.string()){
                    continue;
                }
                cv::Mat img_front = cv::imread(iter->path().string());
                imgs.push_back(img_front);
            }
        }
        cv::Mat map_x = cv::Mat(cv::Size(1920, 1080), CV_32FC1);
        cv::Mat map_y = cv::Mat(cv::Size(1920, 1080), CV_32FC1);
        cv::Size board_size(6, 9);
        if(!check_camera(map_x, map_y, imgs, board_size)){
            return;
        }
    }catch(const std::exception& e){
        LOG_ERROR("发生错误:"<<e.what());
        QMessageBox::critical(this, CODE_TO_UI("错误"), CODE_TO_UI(e.what()));
    }

}

bool DlgCheck::check_camera( cv::Mat& map_x, cv::Mat& map_y, std::vector<cv::Mat>& imgs, cv::Size& board_size){
    std::vector<cv::Point2f> corners;                  /****    缓存每幅图像上检测到的角点       ****/
    std::vector<std::vector<cv::Point2f>>  corners_Seq;    /****  保存检测到的所有角点       ****/
    int count = 0;
    std::vector<cv::Mat> image_Seq;
    for (auto& img : imgs)
    {
        corners.clear();
        // 灰度化图像
        cv::Mat imageGray;
        cv::cvtColor(img, imageGray, cv::COLOR_RGB2GRAY);
        // 自动获取图像上的角点坐标
        bool patternfound = findChessboardCorners(imageGray, board_size, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE +
                                                  cv::CALIB_CB_FAST_CHECK);
        if (!patternfound)
        {
            continue;
        }
        count = count + corners.size();
        corners_Seq.push_back( corners);
        image_Seq.push_back(img);
    }
    cv::Size square_size = cv::Size(20, 20);
    std::vector<std::vector<cv::Point3f> >  object_Points;        /****  保存定标板上角点的三维坐标   ****/

    cv::Mat image_points = cv::Mat(1, count, CV_32FC2, cv::Scalar::all(0));  /*****   保存提取的所有角点   *****/
    std::vector<int>  point_counts;
    // 这是是输入棋盘角点实际坐标，通过与图像上检测到的角点坐标做计算，就可以计算出相关的矩阵
    for (std::size_t t = 0; t<image_Seq.size(); ++t)
    {
        std::vector<cv::Point3f> tempPointSet;
        for (int i = 0; i<board_size.height; i++)
        {
            for (int j = 0; j<board_size.width; j++)
            {
                /* 假设定标板放在世界坐标系中z=0的平面上 */
                cv::Point3f tempPoint;
                tempPoint.x = i*square_size.width;
                tempPoint.y = j*square_size.height;
                tempPoint.z = 0;
                tempPointSet.push_back(tempPoint);
            }
        }
        object_Points.push_back(tempPointSet);
    }
    for (std::size_t i = 0; i< image_Seq.size(); ++i)
    {
        point_counts.push_back(board_size.width*board_size.height);
    }
    /* 开始定标 */
    cv::Size image_size = image_Seq[0].size();
    cv::Matx33d intrinsic_matrix;    /*****    摄像机内参数矩阵    ****/
    cv::Vec4d distortion_coeffs;     /* 摄像机的4个畸变系数：k1,k2,k3,k4*/
    std::vector<cv::Vec3d> rotation_vectors;                           /* 每幅图像的旋转向量 */
    std::vector<cv::Vec3d> translation_vectors;                        /* 每幅图像的平移向量 */
    int flags = 0;
    flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    flags |= cv::fisheye::CALIB_CHECK_COND;
    flags |= cv::fisheye::CALIB_FIX_SKEW;
    cv::fisheye::calibrate(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, flags, cv::TermCriteria(3, 20, 1e-6));

    cv::Mat rotation_matrix = cv::Mat(3, 3, CV_32FC1, cv::Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */
    cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
    cv::fisheye::initUndistortRectifyMap(intrinsic_matrix,distortion_coeffs,R,intrinsic_matrix,image_size,CV_32FC1,map_x,map_y);
    //cv::fisheye::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, R,
    //    cv::getOptimalNewCameraMatrix(intrinsic_matrix, distortion_coeffs, image_size, 1, image_size, 0),image_size, CV_32FC1, map_x, map_y);
    for (std::size_t i = 0; i < image_Seq.size(); ++i)
    {
        cv::Mat t = image_Seq[i].clone();
        cv::remap(image_Seq[i], t, map_x, map_y, cv::INTER_LINEAR);
        cv::imwrite((boost::format("./%d.jpg") % i).str(), t);
    }
    return true;
}

void DlgCheck::on_pb_front_clicked()
{
    auto path_folder = QFileDialog::getExistingDirectory(this, CODE_TO_UI("选择文件夹"), CODE_TO_UI("./"));
    if(path_folder.isEmpty()){
        return;
    }
    ui->le_front->setText(path_folder);
}
