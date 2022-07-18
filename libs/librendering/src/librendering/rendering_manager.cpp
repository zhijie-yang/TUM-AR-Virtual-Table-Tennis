#include "librendering/rendering_manager.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <memory>
#include <sstream>

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
// #define GLFW_INCLUDE_NONE
#define __gl3_h_
#define __glext_h_
#define __gl3ext_h_
#endif //!__APPLE__
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

#include <rxcpp/rx.hpp>

#include <librendering/filesystem.h>
#include <librendering/shader_m.h>
#include <librendering/camera.h>
#include <librendering/model.h>

#include <glm/gtx/matrix_decompose.hpp>

#include "libframework/include/constant.h"

using namespace librendering;

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height); /**< On window size change */
void mouse_callback(GLFWwindow* window, double xpos, double ypos); /**< On mouse move */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); /**< On scrollwheel */
void processInput(GLFWwindow* window); /**< On any keyboard or mouse input */

static rendering_manager::impl* _instance;

/**
* Rendering manager implementation using PImpl paradigm.
*/
class rendering_manager::impl {
private:
    GLFWwindow* _window; /**< GLFW window currently in use */

    GLuint _frameTextureId; /**< Allocated texture id of the video capture frame */
    GLuint _frameVerticesId; /**< Allocated vertex array id of the fullscreen quad displaying video capture frame */
    GLuint _frameVerticesBufferId; /**< Allocated vertex buffer id of the fullscreen quad displaying video capture frame */
    GLuint _frameUVsBufferId; /**< Allocated uv buffer id of the fullscreen quad displaying video capture frame */
    std::unique_ptr<Shader> _frameShader; /**< Shader used to render video capture frame */

    glm::mat4 _racket1Model; /**< Player 1 racket transformation matrix */
    glm::mat4 _racket2Model; /**< Player 2 racket transformation matrix */
    glm::mat4 _ballModel; /**< Ball transformation matrix */
    glm::mat4 _tableModel; /**< Table scale/transformation matrix */
    glm::mat4 _view; /**< Camera view matrix */
    glm::mat4 _proj; /**< Camera projection matrix */
    glm::mat4 _inv_view;

    std::unique_ptr<Model> _ballObject; /**< Ball 3d model */
    std::unique_ptr<Model> _racketObject; /**< Racket 3d model */
    std::unique_ptr<Model> _tableObject; /**< Table 3d model */

    bool _freeCamEnabled; /**< Is free cam enabled */
	bool _paused; /**< Is rendering paused */
	bool _quit; /**< Should the app quit ASAP */
    rendering_settings _settings; /**< Initialization settings */

    int _match1; /**< Player 1 matches won */
    int _match2; /**< Player 2 matches won */
    int _score1; /**< Player 1 in-game score */
    int _score2; /**< Player 2 in-game score */

    float _deltaTime; /**< Frame update time */

    rxcpp::rxsub::subject<float> _update; /**< Update event, happens every frame */
    rxcpp::subscriber<float> _updateSub; /**< Update event publisher */
    rxcpp::observable<float> _updateObs; /**< Update event subscription source */

    rxcpp::composite_subscription _uiDisp, _frameDisp, _modelDisp; // Event subscriptions

    std::string _popupData; /**< Data to display in a popup */
    float _popupDuration; /**< How much longer to display the popup */

    char _ipAddress[256] = "127.0.0.1"; /**< Current ip address used in UI */
    int _port = 50051; /**< Current port used in UI */
    std::string _player1Name; /**< Player 1 name used in UI */
    std::string _player2Name; /**< Player 2 name used in UI */
    bool _player2IsSet = false;
    
    rendering_manager::scene _scene; /**< Current scene used in UI */

    bool _gameEnd = false; /**< Should the game end */
    // glm::vec3 table_translate; /**< Table translation offset */


	char _input_player_name[256] = "Alice";
	bool _ready_to_register = false;
public:
    // camera
    Camera camera; /**< Current free flying camera */
    float lastX; /**< Last mouse position x */
    float lastY; /**< Last mouse position y */
    bool firstMouse; /**< Is it the first mouse plugged in */

    // timing
    float deltaTime; /**< Time between frames in UI */
    float lastFrame; /**< Last frame drawing time for UI */

    std::unique_ptr<Shader> ourShader; /**< 3d model universal shader */

	impl() :
        _window{nullptr},
        _paused{ false }, _quit{ false },
        _frameTextureId{0}, _frameVerticesId{0},
        _settings{}, _scene(rendering_manager::scene::main_menu),
        _update{}, _updateSub{_update.get_subscriber()}, _updateObs{_update.get_observable()},
        _popupDuration{ -1.f },
        _match1{0}, _match2{0}, _score1{0}, _score2{0}
    {
        _instance = this;
    }
    ~impl() = default;

#pragma region Properties

    scene scene_get() const { return _scene; }
    void scene_set(scene value) { _scene = value; }

	bool quit_get() const { return _quit; }

	void quit_set() {
        _quit = true;
    }

    bool paused_get() const
    {
        return _paused;
    }

    void paused_set(bool value)
    {
        _paused = value;
    }

    std::function<int(float*)> racket2_deserialize()
    {
        return [&](float* data) -> int
        {
            _racket2Model = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));
            return 0;
        };
    }

    std::function<int(float*)> racket1_deserialize()
    {
        return [&](float* data) -> int
        {
            _racket1Model = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));
            // _racket1Model = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f)) * _racket1Model;
            // _racket1Model = glm::rotate(glm::pi<float>(), glm::vec3(0.0f, 0.0f, 0.0f)) * _racket1Model;
            // glm::mat4 racket1_transform = glm::mat4(glm::vec4(-1, 0, 0, 0),glm::vec4(0, 1, 0, 0),glm::vec4(0, 0, 1, 0),glm::vec4(0, 0, 0, 1));
            // _racket1Model = racket1_transform * _racket1Model;

            return 0;
        };
    }

    std::function<int(float*)> ball_deserialize()
    {
        return [&](float* data) -> int
        {
            _ballModel = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));

            // glm::vec3 translate = glm::vec3(_ballModel[3]);
            auto axis_permute = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                            glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            _ballModel = axis_permute * _ballModel;
            // _ballModel = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f)) * _ballModel;
            // _ballModel = glm::translate( translate) * _ballModel;
            // _ballModel =  _ballModel;
            // _tableModel = glm::rotate(glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * _tableModel;

            return 0;
        };
    }

    std::function<int(float*)> table_deserialize()
    {
        return [&](float* data) -> int
        {
            // glm::vec3 table_scale = CONST_TABLE_SCALE;
            // float net_height = table_scale.y;
            // table_scale.y = table_scale.z;
            // table_scale.z = net_height;
            _tableModel = glm::mat4(1.0f);
            // _tableModel = glm::scale(table_scale) * _tableModel;
            _tableModel = glm::rotate(glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * _tableModel;

            return 0;
        };
    }

    std::function<int(float*)> view_deserialize()
    {
        return [&](float* data) -> int
        {
            _view = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));

            glm::vec3 translation = _view[3];
            glm::mat3 rotm = glm::mat3(glm::vec3(_view[0]),
                                    glm::vec3(_view[1]),
                                    glm::vec3(_view[2]));
            glm::mat3 rotmInverse = glm::transpose(rotm);
            glm::vec3 transInverse = - rotmInverse * translation;
            auto inv_view = glm::mat4(glm::vec4(rotmInverse[0], 0),
                                glm::vec4(rotmInverse[1], 0),
                                glm::vec4(rotmInverse[2], 0),
                                glm::vec4(transInverse, 1));
            auto axis_permute = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                            glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                            glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            _inv_view = axis_permute * inv_view;

            return 0;
        };
    }

    std::function<int(float*)> proj_deserialize()
    {
        return [&](float* data) -> int
        {
            _proj = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));

            _proj = glm::perspective(2 * glm::atan(data[9]/data[5]),
                    (float)_settings.window_width / (float)_settings.window_height, 0.1f, 100.0f);

            return 0;
        };
    }

    std::function<int(const int&)> score1_deserialize()
    {
        return [&](const int& value) -> int
        {
            _score1 = value;
            return 0;
        };
    }

    std::function<int(const int&)> score2_deserialize()
    {
        return [&](const int& value) -> int
        {
            _score2 = value;
            return 0;
        };
    }

    std::function<int(const int&)> match1_deserialize()
    {
        return [&](const int& value) -> int
        {
            if (value != _match1) {
                std::stringstream ss;
                ss << "The gamr has ended. Current score is " << value << " to " << _match2;
                _popupData = ss.str();
                _popupDuration = _settings.popup_timeout;
            }
            _match1 = value;
            return 0;
        };
    }

    std::function<int(const int&)> match2_deserialize()
    {
        return [&](const int& value) -> int
        {
            if (value != _match2) {
                std::stringstream ss;
                ss << "The game has ended. Current score is " << _match1 << " to " << value;
                _popupData = ss.str();
                _popupDuration = _settings.popup_timeout;
            }
            _match2 = value;
            return 0;
        };
    }

    std::function<int(const char*)> player1_deserialize()
    {
        return [&](const char* value) -> int
        {
            _player1Name = value;
            return 0;
        };
    }

    std::function<int(const char*)> player2_deserialize()
    {
        return [&](const char* value) -> int
        {
            _player2Name = value;
            return 0;
        };
    }

    std::function<int(const bool&)> game_status_deserialize()
    {
        return [&](const float& value) -> int
        {
            _gameEnd = value;
            return 0;
        };
    }

    std::function<int(void*, int, int, int)> capture_deserialize()
    {
        return [&](void* data, int rows, int cols, int channels) -> int
        {
            glBindTexture(GL_TEXTURE_2D, _frameTextureId);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cols, rows, GL_BGR, GL_UNSIGNED_BYTE, data);
            return 0;
        };
    }

    int frametime_serialize(const std::function<int(float)>& processor)
    {
        return processor(_deltaTime);
    }

    int game_status_serialize(const std::function<int(bool)>& processor) {
        return processor(_gameEnd);
    }

    char* input_ip_get() const {
        return (char*) _ipAddress;
    }

    int input_port_get() const {
        return _port;
    }

    bool ready_to_register_get() const {
        return _ready_to_register;
    }

    char* input_player_name_get() const {
        return (char*) _input_player_name;
    }

    void player_2_name_set(std::string const& name) {
        _player2Name = name;
        _player2IsSet = true;
    }

    bool player_2_is_set() const {
        return _player2IsSet;
    }

#pragma region Properties

#pragma region Init

	int init(const rendering_settings& settings)
    {
        _settings = settings;
        // ---------------------------------------
        // Initial Parameters/Data Initialization
        // ---------------------------------------
        //camera = Camera{ glm::vec3(0.0f, 0.0f, 3.0f) };
        camera = Camera{ glm::vec3(0.0f, 0.8f, 3.5f) };
        lastX = settings.window_width / 2.f;
        lastY = settings.window_height / 2.f;
        firstMouse = true;
        deltaTime = 0.0f;
        lastFrame = 0.0f;

        _freeCamEnabled = settings.free_cam_enabled;

        _view = glm::mat4(1.0f);
        _proj = glm::perspective(glm::radians(45.0f), (float)_settings.window_width / (float)_settings.window_height, 0.1f, 100.0f);
        _ballModel = glm::mat4(1.0f);
        
        _tableModel = glm::mat4(1.f);
        // ---------------------------------------
        // GLFW Initialization
        // ---------------------------------------
        
        // Set error callback to get human-readable errors
        glfwSetErrorCallback(glfw_error_callback);
        // Initialize GLFW
        if (!glfwInit())
            return 1;

        // Used OpenGL version
        const char* glsl_version = "#version 330 core";

#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        
        // Create desktop window
        _window = glfwCreateWindow(settings.window_width, settings.window_height, "Virtual Tennis", NULL, NULL);
        if (!_window) {
            return 1;
        }
        // Make window the current context
        glfwMakeContextCurrent(_window);
        // Subscribe to window resize
        glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
        //glfwSetCursorPosCallback(_window, mouse_callback);
        glfwSetScrollCallback(_window, scroll_callback);
        // Disable the mouse cursor and snap it to window center
        //glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

        // ---------------------------------------
        // GLAD Initialization
        // ---------------------------------------

        // Load GLAD functions, extensions and modules
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // Flip images vertically on load
        stbi_set_flip_vertically_on_load(true);

        // Use the depth buffer
        glEnable(GL_DEPTH_TEST);

        // ---------------------------------------
        // Frame Initialization
        // ---------------------------------------
        if (init_frame(settings)) {
            std::cerr << "Failed to init frame.\n";
            return 1;
        }

        // ---------------------------------------
        // Model Initialization
        // ---------------------------------------
        if (init_model(settings)) {
            std::cerr << "Failed to init model.\n";
            return 1;
        }

        // ---------------------------------------
        // ImGUI Initialization
        // ---------------------------------------
        if (init_ui(glsl_version)) {
            std::cerr << "Failed to init UI.\n";
            return 1;
        }

        return 0;
    }

    int init_model(const rendering_settings& settings)
    {
        // Load shaders and 3d models
        ourShader = std::make_unique<Shader>("data/1.model_loading.vs", "data/1.model_loading.fs");
        _racketObject = std::make_unique<Model>("data/racket.fbx");
        _ballObject = std::make_unique<Model>("data/ball.fbx");
        _tableObject = std::make_unique<Model>("data/table.fbx");

        // Update loop hook
        _modelDisp = _updateObs.subscribe([&](float delta)
            {
                if (_scene != rendering_manager::scene::level) return;

                // don't forget to enable shader before setting uniforms
                ourShader->use();

                // view/projection transformations
                glm::mat4 view;
                if (_freeCamEnabled)
                {
                    view = camera.GetViewMatrix();
                }
                else
                {
                    view = _view;
                }

                // render the loaded models
                ourShader->setMat4("projection", _proj);
                // std::cout << glm::to_string(_view * _tableModel * glm::vec4(0, 0, 0, 1)) << std::endl;
                // auto uv = _proj * _view * _tableModel * glm::vec4(0, 0, 0, 1);
                // std::cout << glm::to_string(uv) << std::endl;
                // std::cout << "==========" << std::endl;
                ourShader->setMat4("view", _inv_view);
                ourShader->setMat4("model", _tableModel);
                ourShader->setMat4("scale", CONST_TABLE_RENDER_SCALE);
                _tableObject->Draw(*ourShader);
                ourShader->setMat4("model", _ballModel);
                ourShader->setMat4("scale", CONST_BALL_RENDER_SCALE);
                _ballObject->Draw(*ourShader);
                ourShader->setMat4("model", _racket1Model);
                ourShader->setMat4("scale", CONST_BALL_RENDER_SCALE);
                _racketObject->Draw(*ourShader);
            });

        return 0;
    }

    int init_frame(const rendering_settings& settings)
    {
        // Generate fullscreen quad for capture frame display
        glGenVertexArrays(1, &_frameVerticesId);
        glBindVertexArray(_frameVerticesId);
        static const GLfloat g_vertex_buffer_data[] = {
           -1.0f, -1.0f, 0.0f, // bl
           1.0f, -1.0f, 0.0f,  // br
           1.0f,  1.0f, 0.0f,  // tr

           -1.0f, -1.0f, 0.0f, // bl
           1.0f,  1.0f, 0.0f,  // tr
           -1.0f, 1.0f, 0.0f,  // tl
        };
        static const GLfloat g_uv_buffer_data[] = {
           0.0f, 1.0f, // bl
           1.0f, 1.0f, // br
           1.0f, 0.0f, // tr

           0.0f, 1.0f, // bl
           1.0f, 0.0f, // tr
           0.0f, 0.0f, // tl
        };
        // Generate buffers for the quad and fill in data
        glGenBuffers(1, &_frameVerticesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _frameVerticesBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
        glGenBuffers(1, &_frameUVsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _frameUVsBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
        // Load fullscreen quad shader
        _frameShader = std::make_unique<Shader>("data/fullscreen.vs", "data/fullscreen.fs");
        // Generate a texture for the capture
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenTextures(1, &_frameTextureId);
        glBindTexture(GL_TEXTURE_2D, _frameTextureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        // Fill with empty data as a start
        auto* data = new char[settings.frame_width * settings.frame_height * 3];
        glTexImage2D(GL_TEXTURE_2D, // Type of texture
            0,                      // Pyramid level (for mip-mapping) - 0 is the top level
            GL_RGB,                 // Internal colour format to convert to
            settings.frame_width,   // Image width  i.e. 640 for Kinect in standard mode
            settings.frame_height,  // Image height i.e. 480 for Kinect in standard mode
            0,                      // Border width in pixels (can either be 1 or 0)
            GL_RGB,                 // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
            GL_UNSIGNED_BYTE,       // Image data type
            data);                  // The actual image data itself
        delete[] data;

        // Frame rendering update loop hook
        _frameDisp = _updateObs.subscribe([&](float delta)
            {
                if (_scene != rendering_manager::scene::level) return;

                // Use fullscreen quad shader
                _frameShader->use();
                // Upload fullscreen quad data
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, _frameVerticesBufferId);
                glVertexAttribPointer(
                    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                );
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, _frameUVsBufferId);
                glVertexAttribPointer(
                    1,                  // attribute 1.
                    2,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                );
                // Select fullscreen quad texture aka capture frame
                glBindTexture(GL_TEXTURE_2D, _frameTextureId);
                // Draw quad in 2 triangles with 3 vertices each
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Cleanup
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(0);
                // Clear depth, so quad will always be the background
                glClear(GL_DEPTH_BUFFER_BIT);
            });

        return 0;
    }

    int init_ui(const char* glsl_version)
    {
        // Verify if IMGUI is usable
        IMGUI_CHECKVERSION();
        // Create context and set the OpenGL+GLFW toolchain
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        std::cout << "Hot keys:" << std::endl;
        std::cout << "\tESC - quit the program" << std::endl;
        std::cout << "\tp - pause video" << std::endl;

        // Get IMGUI events
        ImGuiIO& io = ImGui::GetIO();

        // UI handling and rendering update hook
        _uiDisp = _updateObs.subscribe([&](float delta)
            {
                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                // Calculate popup duration logic
                _popupDuration -= delta;
                if (_popupDuration < 0.f) {
                    _popupDuration = -1.f;
                }
                if (_popupDuration >= 0.f) {
                    // Display popup
                    ImGui::SetNextWindowPos(ImVec2());
                    ImGui::Begin("Hey!");

                    ImGui::Text(_popupData.c_str());

                    ImGui::End();
                }

                // Get desktop window size to use for UI anchoring
                auto* viewport = ImGui::GetMainViewport();
                ImVec2 center = viewport->GetCenter();
                ImVec2 brCorner = viewport->WorkSize;

//                if(_gameEnd) {
//                    scene_set(rendering_manager::scene::main_menu);
//                    _gameEnd = false;
//                }

                // Draw current UI scene
                switch (_scene) {
                    case rendering_manager::scene::main_menu:
                    {
                        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                        ImGui::Text("Welcome to Virtual AR Tennis!");

                        auto& style = ImGui::GetStyle();
                        auto buttonWidth = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
                        auto buttonHeight = 0.0f;

                        if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
                            scene_set(rendering_manager::scene::connection);
                        }
                        if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
                            quit_set();
                        }

                        ImGui::End();
                    }
                    break;
                    case rendering_manager::scene::connection:
                    {
                        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                        ImGui::Begin("Multiplayer Connection", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                        ImGui::Text("Host or Join a Game!");
                        ImGui::Text("Enter the IP Address and Port of the host endpoint");
                        ImGui::Text("Then click \"Host\" or \"Join\".");

                        auto& style = ImGui::GetStyle();
                        auto buttonWidth = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
                        auto buttonHeight = 0.0f;
                        auto padding = style.ItemSpacing.x / 2.f;

                        if (ImGui::InputText("IP Address", _ipAddress, 256)) {}
                        if (ImGui::InputInt("Port", &_port)) {}
                        if (ImGui::InputText("Name", _input_player_name, 256)) {}

                        if (ImGui::Button("Offline", ImVec2(buttonWidth / 2.f - padding, buttonHeight))) {
                            scene_set(rendering_manager::scene::level);
                            // TODO: start a server

                            // TODO: call client.connectServer
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Join", ImVec2(buttonWidth / 2.f - padding, buttonHeight))) {
                            scene_set(rendering_manager::scene::waiting_p2);
                            // TODO: call client.connectServer
                            _ready_to_register = true;
                        }
                        if (ImGui::Button("Back", ImVec2(buttonWidth, buttonHeight))) {
                            scene_set(rendering_manager::scene::main_menu);
                        }

                        ImGui::End();
                    }
                    break;
                    case rendering_manager::scene::waiting_p2:
                    {
                        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                        ImGui::Begin("Multiplayer Connection", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                        auto& style = ImGui::GetStyle();
                        auto buttonWidth = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
                        auto buttonHeight = 0.0f;

                        ImGui::Text("Waiting for the other player");
                        if (this->player_2_is_set()) {
                            scene_set(rendering_manager::scene::level);
                        }

                        if (ImGui::Button("Cancel", ImVec2(buttonWidth, buttonHeight))) {
                            _ready_to_register = false;
                            scene_set(rendering_manager::scene::connection);
                        }

                        ImGui::End();
                    }
                    break;
                    case rendering_manager::scene::level:
                    {
                        ImGui::SetNextWindowPos(ImVec2(brCorner.x, 0.f), ImGuiCond_Always, ImVec2(1.f, 0.f));
                        std::stringstream title;
                        title << "Playing match " << _match1 + _match2 + 1;

                        ImGui::Begin(title.str().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                        ImGui::SetWindowFontScale(2.f);

                        ImGui::Text("Current score is:");

                        std::stringstream ss;
                        ss << _player1Name << ": " << _score1;
                        ImGui::Text(ss.str().c_str());
                        ImGui::SameLine();

                        ImGui::Text(" vs ");
                        ImGui::SameLine();

                        ss.str("");
                        ss.clear();
                        ss << _player2Name << ": " << _score2;
                        ImGui::Text(ss.str().c_str());

                        ImGui::End();
                    }
                    break;
                    case rendering_manager::scene::ending:
                    {
                        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                        ImGui::Begin("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                        ImGui::Text("The match has ended!");
                        ImGui::Text("Total game score is:");

                        std::stringstream ss;
                        ss << _player1Name << ": " << _match1;
                        ImGui::Text(ss.str().c_str());
                        ImGui::SameLine();

                        ImGui::Text(" vs ");
                        ImGui::SameLine();

                        ss.str("");
                        ss.clear();
                        ss << _player2Name << ": " << _match2;
                        ImGui::Text(ss.str().c_str());

                        auto& style = ImGui::GetStyle();
                        auto buttonWidth = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
                        auto buttonHeight = 0.0f;

                        if (ImGui::Button("Back to menu", ImVec2(buttonWidth, buttonHeight))) {
                            // TODO: network disconnect
                            scene_set(rendering_manager::scene::main_menu);
                        }

                        ImGui::End();
                    }
                    break;
                }

                // Rendering into a buffer
                ImGui::Render();
                // Rendering onto the screen
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            });

        return 0;
    }
#pragma endregion Init

	int run_tick()
    {
        // If user tried to close the window, terminate
        _quit = _quit || glfwWindowShouldClose(_window);

        // Get current frame time and delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        _deltaTime = deltaTime;

        // Process user input
        processInput(_window);

        // Clear OpenGL rendering buffer
        glClearColor(1.f, 1.f, 1.f, 1.f);
        // Color and depth, to be exact
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update all hooks
        _updateSub.on_next(_deltaTime);
        
        // Swap double-buffered display
        glfwSwapBuffers(_window);
        // Get new user input events
        glfwPollEvents();

        return 0;
    }

	int term()
    {
        // Clear all update hooks
        _uiDisp.unsubscribe();
        _modelDisp.unsubscribe();
        _frameDisp.unsubscribe();

        // Cleanup IMGUI UI
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Delete fullscreen quad buffers, data, and shader
        glDeleteBuffers(1, &_frameVerticesBufferId);
        glDeleteBuffers(1, &_frameUVsBufferId);
        glDeleteProgram(_frameShader->ID);
        glDeleteTextures(1, &_frameTextureId);
        glDeleteVertexArrays(1, &_frameVerticesId);

        // Delete universal shader
        glDeleteProgram(ourShader->ID);

        // Terminate GLFW session
        glfwTerminate();

        return 0;
    }
};

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _instance->camera.ProcessKeyboard(FORWARD, _instance->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _instance->camera.ProcessKeyboard(BACKWARD, _instance->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _instance->camera.ProcessKeyboard(LEFT, _instance->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _instance->camera.ProcessKeyboard(RIGHT, _instance->deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (_instance->firstMouse)
    {
        _instance->lastX = xpos;
        _instance->lastY = ypos;
        _instance->firstMouse = false;
    }

    float xoffset = xpos - _instance->lastX;
    float yoffset = _instance->lastY - ypos; // reversed since y-coordinates go from bottom to top

    _instance->lastX = xpos;
    _instance->lastY = ypos;

    _instance->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    _instance->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

rendering_manager::rendering_manager()
    : _impl{ std::make_unique<impl>() }
{}

librendering::rendering_manager::~rendering_manager()
{}

bool rendering_manager::quit_get() const
{
    return _impl->quit_get();
}

void rendering_manager::quit_set()
{
    _impl->quit_set();
}

bool rendering_manager::paused_get() const
{
    return _impl->paused_get();
}

void rendering_manager::paused_set(bool value)
{
    _impl->paused_set(value);
}

rendering_manager::scene rendering_manager::scene_get() const
{
    return _impl->scene_get();
}

void rendering_manager::scene_set(rendering_manager::scene value)
{
    _impl->scene_set(value);
}

std::function<int(void*, int, int, int)> rendering_manager::capture_deserialize()
{
    return _impl->capture_deserialize();
}

int rendering_manager::init(const rendering_settings& settings)
{
    return _impl->init(settings);
}

int rendering_manager::run_tick()
{
    return _impl->run_tick();
}

int rendering_manager::term()
{
    return _impl->term();
}

std::function<int(float*)> rendering_manager::proj_deserialize()
{
    return _impl->proj_deserialize();
}

std::function<int(float*)>rendering_manager::view_deserialize()
{
    return _impl->view_deserialize();
}

std::function<int(float*)> rendering_manager::ball_deserialize()
{
    return _impl->ball_deserialize();
}

std::function<int(float*)> rendering_manager::racket1_deserialize()
{
    return _impl->racket1_deserialize();
}

std::function<int(float*)> rendering_manager::racket2_deserialize()
{
    return _impl->racket2_deserialize();
}

std::function<int(float*)> rendering_manager::table_deserialize()
{
    return _impl->table_deserialize();
}

std::function<int(const int&)> rendering_manager::score1_deserialize()
{
    return _impl->score1_deserialize();
}

std::function<int(const int&)> rendering_manager::score2_deserialize()
{
    return _impl->score2_deserialize();
}

std::function<int(const int&)> rendering_manager::match1_deserialize()
{
    return _impl->match1_deserialize();
}

std::function<int(const int&)> rendering_manager::match2_deserialize()
{
    return _impl->match2_deserialize();
}

std::function<int(const char*)> rendering_manager::player1_deserialize()
{
    return _impl->player1_deserialize();
}

std::function<int(const char*)> rendering_manager::player2_deserialize()
{
    return _impl->player2_deserialize();
}

std::function<int(const bool&)> rendering_manager::game_status_deserialize()
{
    return _impl->game_status_deserialize();
}

int rendering_manager::frametime_serialize(const std::function<int(float)>& processor)
{
    return _impl->frametime_serialize(processor);
}

int rendering_manager::game_status_serialize(const std::function<int(bool)>& processor) {
    return _impl->game_status_serialize(processor);
}

char* rendering_manager::input_ip_get() const {
    return _impl->input_ip_get();
}

int rendering_manager::input_port_get() const {
    return _impl->input_port_get();
}

bool rendering_manager::ready_to_register_get() const {
    return _impl->ready_to_register_get();
}

char* rendering_manager::input_player_name_get() const {
    return _impl->input_player_name_get();
}

void rendering_manager::player_2_name_set(std::string const& name) {
    _impl->player_2_name_set(name);
}

bool rendering_manager::player_2_is_set() const {
    return _impl->player_2_is_set();
}
