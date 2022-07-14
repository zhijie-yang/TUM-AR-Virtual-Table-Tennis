#include "libvision/vision_manager.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <tuple>
#include <opencv2/calib3d.hpp>
#include <glm/glm.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace libvision;
using namespace cv;
using namespace std;
#define CENTER_MARKER_ID 23
#define RACKET_MARKER_ID 40
#define MARKERS_NUM 3
#define VIEW_POINTS 8
#define MARKER_SIZE 0.07 // unit:meter
#define MARKER_PIXEL 199
class vision_manager::impl {
private:
	cv::VideoCapture _cap;
	cv::Mat _frame;
	cv::Mat _cameraMatrix, _distCoeffs;
	glm::mat4 _racket2table,_table2cam,_racket2cam;
	cv::Ptr<cv::aruco::Dictionary> _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	cv::Ptr<cv::aruco::DetectorParameters> _parameters = cv::aruco::DetectorParameters::create();
	std::vector<std::vector<cv::Point2f>> _markerCorners, _rejectedCandidates;
	std::vector<int> _markerIds;
	cv::Ptr<cv::aruco::Board> _board;
	cv::Matx31d _board_rvec, _board_tvec;
    cv::KalmanFilter _KF;

    glm::mat4 _fill_mat4(cv::Matx31d rvec,cv::Matx31d tvec)
	{
		glm::mat4 _result;
		cv::Matx33d rmat;
		cv::Rodrigues(rvec, rmat);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				_result[i][j] = rmat.val[i + j * 3];
			}
			_result[i][3] = 0.0f;
		}
		for (int j = 0; j < 3; j++)
		{
			_result[3][j] = tvec.val[j];
		}
		_result[3][3] = 1.0f;	
		return _result;	
	}
	
	int _create_board()
	{
		cv::Mat markerImage;
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
		std::vector<std::vector<cv::Point3f>> objectPoints(3);
		markerImage = cv::imread("../data/marker.png");
		cv::aruco::detectMarkers(markerImage, _dictionary, markerCorners, markerIds, _parameters, rejectedCandidates);


		for (size_t i = 0; i < MARKERS_NUM; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				objectPoints[i].push_back(Point3f(markerCorners[i][j].x , markerCorners[i][j].y , 0.0));
			}
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
		while (true) 
		{
			_cap >> _frame;
			cv::imshow("Webcam", _frame);
			waitKey(1500);


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

			if (count == VIEW_POINTS)
			{
				break;
			}

		}

		// After capturing in several viewpoints, start calibration
		std::vector<cv::Mat> rvecs, tvecs;
		double repError = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, _board, imgSize, _cameraMatrix, _distCoeffs, rvecs, tvecs);
		FileStorage fs("../data/camera_paras.yml", FileStorage::WRITE);
		fs << "cameraMatrix" << _cameraMatrix << "distCoeffs" << _distCoeffs;
		fs.release();
		cv::destroyAllWindows();
		return 0;
	}

	int _read_calibrate_paras()
	{
		FileStorage fs("../data/camera_paras.yml", FileStorage::READ);
		fs["cameraMatrix"] >> _cameraMatrix;
		fs["distCoeffs"] >> _distCoeffs;
		fs.release();
		return 0;
	}

	tuple<cv::Matx31d, cv::Matx31d> _inversePerspective(cv::Matx31d rvec, cv::Matx31d tvec)
	{
		cv::Matx33d rmat;
		cv::Rodrigues(rvec, rmat);
		rmat = rmat.t();
		cv::Matx31d invtvec = rmat * (-tvec);
		cv::Matx31d invrvec;
		cv::Rodrigues(rmat, invrvec);
		return std::make_tuple(invrvec, invtvec);
	}

	cv::Matx31d _transPerspective(cv::Matx31d rvec) 
	{
		cv::Matx33d rmat;
		cv::Rodrigues(rvec, rmat);
		rmat = -rmat;
		rmat.val[0] *= -1;
		rmat.val[3] *= -1;
		rmat.val[6] *= -1;
		cv::Matx31d invrvec;
		cv::Rodrigues(rmat, invrvec);
		return invrvec;
	}

	int _detect_markers()
	{
        cv::Mat detect_frame;
        undistort(_frame, detect_frame, _cameraMatrix, _distCoeffs);
        cv::aruco::detectMarkers(detect_frame, _dictionary, _markerCorners, _markerIds, _parameters, _rejectedCandidates);
		if (_markerIds.empty())
		{
			std::cerr << "Failed to detect markers"<< std::endl;
			return -1;
		}

		return 0;
	}

	int _estimate_position()
	{
		int valid = cv::aruco::estimatePoseBoard(_markerCorners, _markerIds, _board, _cameraMatrix, _distCoeffs, _board_rvec, _board_tvec);
		if (valid == 0)
		{
			std::cerr << "Estimation failed"<< std::endl;
			return -1;
		}
		_board_tvec.val[0] = _board_tvec.val[0] / MARKER_PIXEL * MARKER_SIZE;
		_board_tvec.val[1] = _board_tvec.val[1] / MARKER_PIXEL * MARKER_SIZE;
		_board_tvec.val[2] = _board_tvec.val[2] / MARKER_PIXEL * MARKER_SIZE;
		cv::drawFrameAxes(_frame, _cameraMatrix, _distCoeffs, _board_rvec, _board_tvec, MARKER_SIZE / 2);
		std::vector<cv::Matx31d> rvecs, tvecs;

		cv::aruco::estimatePoseSingleMarkers(_markerCorners, MARKER_SIZE, _cameraMatrix, _distCoeffs, rvecs, tvecs);
		bool f_racket = false;
		cv::Matx31d t_rvec, t_tvec, srvec, stvec, inv_rvec, inv_tvec;


		for (size_t i = 0; i < _markerIds.size(); i++)
		{
			if (_markerIds[i] == RACKET_MARKER_ID)
			{
				rvecs[i] = _transPerspective(rvecs[i]);
				srvec = rvecs[i];
				stvec = tvecs[i];
				f_racket = true;
			}
		}

		if (f_racket) 
		{
			tuple<cv::Matx31f, cv::Matx31f> t = _inversePerspective(_board_rvec,_board_tvec);
			inv_rvec = std::get<0>(t);
			inv_tvec = std::get<1>(t);
			cv::composeRT(srvec, stvec, inv_rvec, inv_tvec, t_rvec, t_tvec);
			_racket2table = _fill_mat4(t_rvec,t_tvec);
			_racket2cam = _fill_mat4(srvec, stvec);
			_table2cam = _fill_mat4(_board_rvec,_board_tvec);

			//glm::mat3 rotM = glm::mat3(glm::vec3(_result[0]),
			//glm::vec3(_result[1]);
			//glm::vec3(_result[2]));
			//glm::vec3 translation = glm::vec3(_result[3]);
			//glm::vec4  marker_pos = glm::vec4(0, 0, 0, 1);
			//glm::vec3 transformedPos = rotM * marker_pos + translation;
			//std::cout << "X" << transformedPos[0]  << "\t Y" << transformedPos[1]  << "\t Z" << transformedPos[2] << std::endl;
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
        cv::Size size;// camera image size
        size.height = (int)_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        size.width = (int)_cap.get(cv::CAP_PROP_FRAME_WIDTH);
		if (!_cap.isOpened())
		{
			std::cerr << "***Could not initialize capturing...***" << std::endl;
			return -1;
		}
		_create_board();
		//_save_calibrate_paras();
		_read_calibrate_paras();
        _cameraMatrix =cv::getOptimalNewCameraMatrix(_cameraMatrix,_distCoeffs,size,0.5);
        int nStates = 12;            // the number of states
        int nMeasurements = 6;       // the number of measured states
        int nInputs = 0;             // the number of action control
        double dt = 0.125;           // time between measurements (1/FPS)
        //initKalmanFilter(_KF, nStates, nMeasurements, nInputs, dt);    // init function

        return 0;
	}

	int run_tick()
	{
		_cap >> _frame;

		if (_frame.empty())
		{
			std::cerr << "Failed to capture frame"<< std::endl;
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
