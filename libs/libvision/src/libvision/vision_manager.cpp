#include "libvision/vision_manager.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <tuple>
#include <opencv2/calib3d.hpp>
#include <glm/glm.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>
#include <iostream>

using namespace libvision;
using namespace cv;
using namespace std;
#define CENTER_MAREKER_ID 23
#define RACKET_MAREKER_ID 40
#define MARKERS_NUM 3
#define VIEW_POINTS 8
class vision_manager::impl {
private:
	cv::VideoCapture _cap;
	cv::Mat _frame;
	cv::Mat _cameraMatrix, _distCoeffs;
	glm::mat4 _result;
	cv::Ptr<cv::aruco::Dictionary> _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	cv::Ptr<cv::aruco::DetectorParameters> _parameters = cv::aruco::DetectorParameters::create();
	std::vector<std::vector<cv::Point2f>> _markerCorners, _rejectedCandidates;
	std::vector<int> _markerIds;
	cv::Ptr<cv::aruco::Board> _board;

	int _create_board()
	{
		cv::Mat markerImage;
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
		cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
		std::vector<std::vector<cv::Point3f>> objectPoints(3);
		markerImage = cv::imread("data/marker.png");
		cv::aruco::detectMarkers(markerImage, _dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
		float middle_x;
		float middle_y;

		middle_x = (markerCorners[1][0].x + markerCorners[1][1].x + markerCorners[1][2].x + markerCorners[1][3].x) / 4;
		middle_y = (markerCorners[1][0].y + markerCorners[1][1].y + markerCorners[1][2].y + markerCorners[1][3].y) / 4;

		for (size_t i = 0; i < MARKERS_NUM; i++)
		{
			objectPoints[i].push_back(Point3f(markerCorners[i][0].x - middle_x, markerCorners[i][0].y - middle_y, 0.0));
			objectPoints[i].push_back(Point3f(markerCorners[i][1].x - middle_x, markerCorners[i][1].y - middle_y, 0.0));
			objectPoints[i].push_back(Point3f(markerCorners[i][2].x - middle_x, markerCorners[i][2].y - middle_y, 0.0));
			objectPoints[i].push_back(Point3f(markerCorners[i][3].x - middle_x, markerCorners[i][3].y - middle_y, 0.0));
		}
		_board = cv::aruco::Board::create(objectPoints, _dictionary, markerIds);
		return 0;
	}

	int _save_calibrate_paras()
	{
		cv::Mat outputImage;
		cv::namedWindow("Webcam", cv::WindowFlags::WINDOW_AUTOSIZE);
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
		cv::Size imgSize;// camera image size
		imgSize.height = (int)_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		imgSize.width = (int)_cap.get(cv::CAP_PROP_FRAME_WIDTH);

		std::vector<std::vector<cv::Point2f>> allCornersConcatenated;
		std::vector<int> allIdsConcatenated;
		std::vector<int> markerCounterPerFrame;
		// Detect aruco board from several viewpoints and fill allCornersConcatenated, allIdsConcatenated and markerCounterPerFrame
		int count = 0;
		while (true) {
			run_tick();
			cv::imshow("Webcam", _frame);
			int key = waitKey(1500);
			if (key == 13)
			{

				cv::aruco::detectMarkers(_frame, _dictionary, markerCorners, markerIds, _parameters, rejectedCandidates);
				cv::imshow("Webcam", _frame);
				allCornersConcatenated.insert(
					allCornersConcatenated.end(),
					std::make_move_iterator(markerCorners.begin()),
					std::make_move_iterator(markerCorners.end())
				);
				allIdsConcatenated.insert(
					allIdsConcatenated.end(),
					std::make_move_iterator(markerIds.begin()),
					std::make_move_iterator(markerIds.end())
				);
				markerCounterPerFrame.push_back((int)markerIds.size());
				count++;
			}
			if (count == VIEW_POINTS)
			{
				break;
			}

		}

		// After capturing in several viewpoints, start calibration
		std::vector<cv::Mat> rvecs, tvecs;
		double repError = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, _board, imgSize, _cameraMatrix, _distCoeffs, rvecs, tvecs);
		FileStorage fs("camera_paras.yml", FileStorage::WRITE);
		fs << "cameraMatrix" << _cameraMatrix << "distCoeffs" << _distCoeffs;
		fs.release();
		return 0;
	}

	int _read_calibrate_paras()
	{
		FileStorage fs("camera_paras.yml", FileStorage::READ);
		fs["cameraMatrix"] >> _cameraMatrix;
		fs["distCoeffs"] >> _distCoeffs;
		fs.release();
		return 0;
	}

	tuple<cv::Matx31d, cv::Matx31d> _inversePerspective(cv::Matx31d rvec, cv::Matx31d tvec) {
		cv::Matx33d rmat;
		cv::Rodrigues(rvec, rmat);
		rmat = rmat.t();
		cv::Matx31d invtvec = rmat * (-tvec);
		cv::Matx31d invrvec;
		cv::Rodrigues(rmat, invrvec);
		return std::make_tuple(invrvec, invtvec);
	}

	int _detect_markers()
	{
		cv::aruco::detectMarkers(_frame, _dictionary, _markerCorners, _markerIds, _parameters, _rejectedCandidates);
		if (_markerIds.size() == 0)
		{
			std::cerr << "Failed to detect markers";
			return -1;
		}
		return 0;
	}

	int _estimate_position()
	{
		cv::Matx31d rvec, tvec;
		int valid = cv::aruco::estimatePoseBoard(_markerCorners, _markerIds, _board, _cameraMatrix, _distCoeffs, rvec, tvec);
		if (valid == 0)
		{
			std::cerr << "Estimation failed";
			return -1;
		}
		std::vector<cv::Matx31d> rvecs, tvecs;
		cv::aruco::estimatePoseSingleMarkers(_markerCorners, 50, _cameraMatrix, _distCoeffs, rvecs, tvecs);
		bool f_racket = false;
		cv::Matx31d t_rvec, t_tvec, srvec, stvec, inv_rvec, inv_tvec;

		for (size_t i = 0; i < _markerIds.size(); i++)
		{

			if (_markerIds[i] == RACKET_MAREKER_ID)
			{
				srvec = rvecs[i];
				stvec = tvecs[i];
				f_racket = true;
				tuple<cv::Matx31f, cv::Matx31f> t = _inversePerspective(rvec, tvec);
				inv_rvec = std::get<0>(t);
				inv_tvec = std::get<1>(t);
			}
		}

		if (f_racket) {

			cv::composeRT(srvec, stvec, inv_rvec, inv_tvec, t_rvec, t_tvec);
			cv::Matx33d rmat;
			cv::Rodrigues(t_rvec, rmat);
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					_result[i][j] = rmat.val[i + j * 3];
				}
				_result[i][3] = 0.0f;
			}
			for (size_t j = 0; j < 3; j++)
			{
				_result[3][j] = t_tvec.val[j];
			}
			_result[3][3] = 1.0f;

		}
		return 0;
	}

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
		_create_board();
		_save_calibrate_paras();
		_read_calibrate_paras();
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
		_detect_markers();
		_estimate_position();
		return 0;
	}

	int term()
	{
		_cap.release();

		return 0;
	}

	glm::mat4 get_transformation_matrix()
	{
		return _result;
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

