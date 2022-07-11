#include "libvision/vision_manager.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>
#include <iostream>

using namespace libvision;

class vision_manager::impl {
private:
    cv::VideoCapture _cap;
    cv::Mat _frame;
public:
    impl() : _cap{}
    {}
    ~impl() = default;

    int capture_width_get() const
    {
        return (int)_cap.get(cv::CAP_PROP_FRAME_WIDTH);
    }

    int capture_height_get() const
    {
        return (int)_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    }

    int capture_serialize(const std::function<int(void*, int, int, int)>& processor)
    {
        return processor((void*)_frame.ptr(), _frame.rows, _frame.cols, _frame.channels());
    }

    int init(const vision_settings& settings)
    {
        _cap.open(settings.camera_id);

        if (!_cap.isOpened())
        {
            std::cerr << "***Could not initialize capturing...***" << std::endl;
            return -1;
        }

        return 0;
    }

    int run_tick()
    {
        _cap >> _frame;

        if (_frame.empty())
        {
            std::cerr << "Failed to capture frame";
            return -1;
        }

        return 0;
    }

    int term()
    {
        _cap.release();

        return 0;
    }

    int racket1_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = glm::translate(glm::vec3(0.7625f, 0.3f, 1.37f))
                          * glm::rotate(glm::pi<float>() / -2.f, glm::vec3(1.f, 0.f, 0.f))
                          * glm::mat4(1.0f);
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    int racket2_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = glm::translate(glm::vec3(-0.7625f, 0.3f, -1.37f))
                          * glm::rotate(glm::pi<float>() / -2.f, glm::vec3(1.f, 0.f, 0.f))
                          * glm::mat4(1.0f);
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    int table_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = glm::mat4(1.0f);
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }
};

vision_manager::vision_manager()
    : _impl{ std::make_unique<impl>() }
{}

vision_manager::~vision_manager()
{}

int vision_manager::capture_width_get() const
{
	return _impl->capture_width_get();
}

int vision_manager::capture_height_get() const
{
	return _impl->capture_height_get();
}

int vision_manager::init(const vision_settings& settings)
{
	return _impl->init(settings);
}

int vision_manager::run_tick()
{
	return _impl->run_tick();
}

int vision_manager::term()
{
	return _impl->term();
}

int vision_manager::capture_serialize(const std::function<int(void*, int, int, int)>& processor)
{
    return _impl->capture_serialize(processor);
}

int vision_manager::racket1_serialize(const std::function<int(float *)> &processor) {
    return _impl->racket1_serialize(processor);
}

int vision_manager::racket2_serialize(const std::function<int(float *)> &processor) {
    return _impl->racket2_serialize(processor);
}

int vision_manager::table_serialize(const std::function<int(float *)> &processor) {
    return _impl->table_serialize(processor);
}
