#include "libvision/vision_manager.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <tuple>
#include <opencv2/calib3d.hpp>
#include <glm/glm.hpp>
#include <opencv2/core/types_c.h>
#include <cmath>
#include <fstream>

using namespace libvision;
using namespace cv;
using namespace std;

class vision_manager::impl {
private:
    cv::VideoCapture _cap;              //camera capture
    cv::Mat _frame;                     // the frame captured by camera
    cv::Mat _cameraMatrix, _distCoeffs; // the intrinsic matrix of camera and Distortion coefficients
    glm::mat4 _racket2table,_table2cam,_racket2cam;
    // _racket2table : the matrix of transformation for racket with regard to the table
    // _table2cam : the matrix of transformation for table with regard to the camera
    // _racket2cam : the matrix of transformation for racket with regard to the camera
    cv::Ptr<cv::aruco::Dictionary> _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    //the dictionary storing the aruco markers
    cv::Ptr<cv::aruco::DetectorParameters> _parameters = cv::aruco::DetectorParameters::create();
    //the parameters used when detecting markers
    std::vector<std::vector<cv::Point2f>> _markerCorners, _rejectedCandidates;
    //_markerCorners: vectors storing the corners of the detected markers starting from top left and clockwise
    //_rejectedCandidates:i mgPoints of those squares whose inner code has not a correct codification. (Not used)
    std::vector<int> _markerIds;
    //The id of each marker
    cv::Ptr<cv::aruco::Board> _board;
    cv::Matx31d _board_rvec, _board_tvec;
    //the current rotation and translation vector of board w.r.t the camera
    glm::mat4 _proj = glm::mat4(1.0f);
    //camera matrix in form of glm::mat4
    cv::Matx31d _Past_board_rvec, _Past_board_tvec;
    //the rotation and translation vector of board w.r.t the camera in last frame
    cv::Matx31d _print_board_rvec, _print_board_tvec;
    //the rotation and translation vector of board w.r.t the camera used for calculation
    bool _found = false;

    //comapre the current and past rotation and translation vector
    //if the difference is smaller than threshold return true
    bool _compare_vector(cv::Matx31d board_rvec,cv::Matx31d board_tvec,cv::Matx31d past_board_rvec,cv::Matx31d past_board_tvec)
    {
        bool f_rvec = false;
        bool f_tvec = false;
        if ((abs(board_tvec.val[0] - past_board_tvec.val[0]) < THRESHOLD)&&(abs(board_tvec.val[1] - past_board_tvec.val[1]) < THRESHOLD)&&(abs(board_tvec.val[2] - past_board_tvec.val[2]) < THRESHOLD)){
            f_tvec = true;
        }
        if ((abs(board_rvec.val[0] - past_board_rvec.val[0]) < THRESHOLD)&&(abs(board_rvec.val[1] - past_board_rvec.val[1]) < THRESHOLD)&&(abs(board_rvec.val[2] - past_board_rvec.val[2]) < THRESHOLD)){
            f_rvec = true;
        }
        return (f_rvec && f_rvec);
    }

    // transform rvec to matrix and fill the transformation matrix together with the tvec
    glm::mat4 _fill_mat4(cv::Matx31d rvec,cv::Matx31d tvec)
    {
        glm::mat4 _result;
        cv::Matx33d rmat;
        cv::Rodrigues(rvec, rmat);
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
            _result[3][j] = tvec.val[j];
        }
        _result[3][3] = 1.0f;
        return _result;
    }

    //create the aruco board during initialization step
    int _create_board()
    {
        cv::Mat markerImage;
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        std::vector<std::vector<cv::Point3f>> objectPoints(3);
        markerImage = cv::imread("../data/marker.png");
        cv::aruco::detectMarkers(markerImage, _dictionary, markerCorners, markerIds, _parameters, rejectedCandidates);

        //middle point of the center marker used to set the original point
        float middle_x;
        float middle_y;
        middle_x = (markerCorners[1][0].x + markerCorners[1][1].x + markerCorners[1][2].x + markerCorners[1][3].x) / 4;
        middle_y = (markerCorners[1][0].y + markerCorners[1][1].y + markerCorners[1][2].y + markerCorners[1][3].y) / 4;

        // set the original point to original point
        for (size_t i = 0; i < MARKERS_NUM; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {

                objectPoints[i].push_back(Point3f(markerCorners[i][j].x - middle_x, markerCorners[i][j].y - middle_y, 0.0));
            }
        }
        //create the board
        _board = cv::aruco::Board::create(objectPoints, _dictionary, markerIds);
        return 0;
    }

    //calibration of cameras and store parameters
    int _save_calibrate_paras()
    {
        //check if the parameters already exist
        ifstream file;
        file.open("../data/camera_paras.yml");
        if(file) {
            return 0;
        }
        cout<<"file doesn't exist"<<endl;
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
            int key = waitKey(1500);
            // read enter to save one frame from a view
            if (key == 13)
            {
                cv::aruco::detectMarkers(_frame, _dictionary, markerCorners, markerIds, _parameters,
                                         rejectedCandidates);
                if (markerIds.empty())
                {
                    return -1;
                }
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

        // After capturing in several viewpoints, start calibration and save parameters
        std::vector<cv::Mat> rvecs, tvecs;
        double repError = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, _board, imgSize, _cameraMatrix, _distCoeffs, rvecs, tvecs);
        FileStorage fs("../data/camera_paras.yml", FileStorage::WRITE);
        fs << "cameraMatrix" << _cameraMatrix << "distCoeffs" << _distCoeffs;
        fs.release();
        cv::destroyAllWindows();
        return 0;
    }

    //read the camera calibrators
    int _read_calibrate_paras()
    {
        FileStorage fs("../data/camera_paras.yml", FileStorage::READ);
        fs["cameraMatrix"] >> _cameraMatrix;
        fs["distCoeffs"] >> _distCoeffs;
        fs.release();

        glm::mat3 proj = glm::mat3(1.0f);
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                proj[i][j] = _cameraMatrix.at<double>(j,i);
            }
        }
        _proj = glm::mat4(glm::vec4(proj[0], 0),
                          glm::vec4(proj[1], 0),
                          glm::vec4(proj[2], 0),
                          glm::vec4(0, 0, 0, 1));

        return 0;
    }

    //reverse the rotation and translation matrix from A w.r.t B to B w.r.t to A
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
    // align the rotation vector return by estimatePoseBoard and estimatePoseSingleMarkers
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

    // detect the markers
    int _detect_markers()
    {
        cv::aruco::detectMarkers(_frame, _dictionary, _markerCorners, _markerIds, _parameters, _rejectedCandidates);
        if (_markerIds.size() == 0)
        {
            std::cerr << "Failed to detect markers"<< std::endl;
            return -1;
        }

        return 0;
    }

    //use the detect markers to estimate the position of table(board) and racket
    int _estimate_position()
    {
        int valid = cv::aruco::estimatePoseBoard(_markerCorners, _markerIds, _board, _cameraMatrix, _distCoeffs, _board_rvec, _board_tvec);
        if (valid == 0)
        {
            std::cerr << "Estimation failed"<< std::endl;
            return -1;
        }
        //scale the board translation vector
        _board_tvec.val[0] = _board_tvec.val[0] / MARKER_PIXEL * MARKER_SIZE *2;
        _board_tvec.val[1] = _board_tvec.val[1] / MARKER_PIXEL * MARKER_SIZE *2;
        _board_tvec.val[2] = _board_tvec.val[2] / MARKER_PIXEL * MARKER_SIZE *2;

        //if not the first captured frame
        if (_found)
        {
            // if the difference between last and current position of board cannot be neglected
            if (!(_compare_vector(_board_rvec,_board_tvec,_Past_board_rvec,_Past_board_tvec))){
                //update the rvec and tvec
                _print_board_rvec = _board_rvec;
                _print_board_tvec = _board_tvec;
            }
            //store the current frame
            _Past_board_tvec = _board_tvec;
            _Past_board_rvec = _board_rvec;
        }
        else
        {
            // the first frame
            _Past_board_rvec = _board_rvec;
            _Past_board_tvec = _board_tvec;
            _print_board_rvec = _board_rvec;
            _print_board_tvec = _board_tvec;
            _found = true;
        }

        std::vector<cv::Matx31d> rvecs, tvecs;
        // the vector storing rvec and tvec for each markers
        cv::aruco::estimatePoseSingleMarkers(_markerCorners, MARKER_SIZE, _cameraMatrix, _distCoeffs, rvecs, tvecs);
        bool f_racket = false;
        cv::Matx31d t_rvec, t_tvec, srvec, stvec, inv_rvec, inv_tvec;
        // t_rvec,t_tvec: the rvec and tvec for racket w.r.t table(board)
        // srvec,stvec: the rvec and tvec for racket w.r.t camera
        // inv_rvec, inv_tvec: the rvec and tvec for camera w.r.t table
        for (size_t i = 0; i < _markerIds.size(); i++)
        {
            //finding the racket
            if (_markerIds[i] == RACKET_MARKER_ID)
            {
                rvecs[i] = _transPerspective(rvecs[i]);
                srvec = rvecs[i];
                stvec = tvecs[i];
                f_racket = true;
            }
        }

        _table2cam = _fill_mat4(_board_rvec,_board_tvec);
        // if found the racket
        if (f_racket)
        {
            // get the rvec and tvec for camera w.r.t table
            tuple<cv::Matx31f, cv::Matx31f> t = _inversePerspective(_board_rvec,_board_tvec);
            inv_rvec = std::get<0>(t);
            inv_tvec = std::get<1>(t);
            // combine the rvec and tvec for racket w.r.t camera
            // and the rvec and tvec for camera w.r.t table
            // to get t_rvec,t_tvec: the rvec and tvec for racket w.r.t table(board)
            cv::composeRT(srvec, stvec, inv_rvec, inv_tvec, t_rvec, t_tvec);
            //fill the mat4 matrix for racket w.r.t to table and racket w.r.t camera
            _racket2table = _fill_mat4(t_rvec,t_tvec);
            _racket2cam = _fill_mat4(srvec, stvec);
        }
        return 0;
    }

public:
    impl() : _cap{}
    {}
    ~impl() = default;

    //get camera width
    int capture_width_get() const
    {
        return (int)_cap.get(cv::CAP_PROP_FRAME_WIDTH);
    }

    //get camera height
    int capture_height_get() const
    {
        return (int)_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    }

    //serialized the frame for render
    int capture_serialize(const std::function<int(void*, int, int, int)>& processor)
    {
        return processor((void*)_frame.ptr(), _frame.rows, _frame.cols, _frame.channels());
    }

    //initialized the camera and get camera parameters
    int init(const vision_settings& settings)
    {
        _cap.open(settings.camera_id);

        if (!_cap.isOpened())
        {
            std::cerr << "***Could not initialize capturing...***" << std::endl;
            return -1;
        }
        _create_board();


        int flag = _save_calibrate_paras();
        if (flag == -1)
        {
            std::cerr << "***Could not detect markers***" << std::endl;
            return -1;
        }

        _read_calibrate_paras();
        return 0;
    }

    //run a tick to get the frame and do pose estimation
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

    //serialized the racket 1 w.r.t camera transformation matrix across different library
    int racket1_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = _racket2table;
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    //serialized the racket 2 w.r.t camera transformation matrix across different library
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

    //serialized the camera w.r.t table transformation matrix across different library
    int view_serialize(const std::function<int(float*)>& processor)
    {
        glm::vec3 translation = _table2cam[3];
        glm::mat3 rotm = glm::mat3(glm::vec3(_table2cam[0]),
                                   glm::vec3(_table2cam[1]),
                                   glm::vec3(_table2cam[2]));
        glm::mat3 rotmInverse = glm::transpose(rotm);
        glm::vec3 transInverse = - rotmInverse * translation;
        glm::mat4 model = glm::mat4(glm::vec4(rotmInverse[0], 0),
                                    glm::vec4(rotmInverse[1], 0),
                                    glm::vec4(rotmInverse[2], 0),
                                    glm::vec4(transInverse, 1));
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    //serialized the table w.r.t camera transformation matrix across different library
    int table_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = _table2cam;
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    //serialized the camera parameters matrix across different library
    int proj_serialize(const std::function<int(float*)>& processor)
    {
        float arr_model[16] = {
                _proj[0][0], _proj[0][1], _proj[0][2], _proj[0][3],
                _proj[1][0], _proj[1][1], _proj[1][2], _proj[1][3],
                _proj[2][0], _proj[2][1], _proj[2][2], _proj[2][3],
                _proj[3][0], _proj[3][1], _proj[3][2], _proj[3][3]};
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

int vision_manager::proj_serialize(const std::function<int(float *)> &processor) {
    return _impl->proj_serialize(processor);
}

int vision_manager::view_serialize(const std::function<int(float *)> &processor) {
    return _impl->view_serialize(processor);
}
