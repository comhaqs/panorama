#include "Configure.h"
#include <mutex>

std::shared_ptr<Configure> Configure::get_instance(){
    static std::shared_ptr<Configure> s_instance;
    static std::mutex s_mutex;

    if(!s_instance){
        std::lock_guard<std::mutex> lock(s_mutex);
        if(!s_instance){
            s_instance = std::shared_ptr<Configure>();

        }
    }
    return s_instance;
}

Configure::Configure()
{

}
