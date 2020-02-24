#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <memory>


#define CONFIGURE_PATH_CAMERA_FRONT "configure.path.camera_front"
#define CONFIGURE_PATH_CAMERA_BACK "configure.path.camera_back"
#define CONFIGURE_PATH_CAMERA_LEFT "configure.path.camera_left"
#define CONFIGURE_PATH_CAMERA_RIGHT "configure.path.camera_right"


class Configure
{
public:
    static std::shared_ptr<Configure> get_instance();



protected:
    Configure();

};

#endif // CONFIGURE_H
