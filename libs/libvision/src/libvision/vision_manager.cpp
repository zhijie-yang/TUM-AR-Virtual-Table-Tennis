#include "libvision/vision_manager.hpp"

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

    int capture_serialize(const std::function<int(void*, int)>& processor)
    {
        return processor((void*)_frame.ptr(), _frame.rows * _frame.cols * _frame.channels());
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

int vision_manager::capture_serialize(const std::function<int(void*, int)>& processor)
{
    return _impl->capture_serialize(processor);
}
