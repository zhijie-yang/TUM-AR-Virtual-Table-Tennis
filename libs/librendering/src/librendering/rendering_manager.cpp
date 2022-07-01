#include "librendering/rendering_manager.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

#include <librendering/filesystem.h>
#include <librendering/shader_m.h>
#include <librendering/camera.h>
#include <librendering/model.h>

using namespace librendering;

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

static rendering_manager::impl* _instance;

class rendering_manager::impl {
private:
    GLFWwindow* _window;

    GLuint _frameTextureId;
    GLuint _frameVerticesId;
    GLuint _frameVerticesBufferId;
    GLuint _frameUVsBufferId;
    std::unique_ptr<Shader> _frameShader;

    glm::mat4 _racket1Model;
    glm::mat4 _racket2Model;
    glm::mat4 _ballModel;
    glm::mat4 _view;
    glm::mat4 _proj;

    std::unique_ptr<Model> _ballObject;
    std::unique_ptr<Model> _racketObject;

    bool _freeCamEnabled;
	bool _paused;
	bool _quit;
    rendering_settings _settings;
public:
    // camera
    Camera camera;
    float lastX;
    float lastY;
    bool firstMouse;

    // timing
    float deltaTime;
    float lastFrame;

    std::unique_ptr<Shader> ourShader;

	impl() :
        _window{nullptr},
        _paused{ false }, _quit{ false },
        _frameTextureId{0}, _frameVerticesId{0},
        _settings{}
    {
        _instance = this;
    }
    ~impl() = default;

	bool quit_get() const
    {
        return _quit;
    }

	void quit_set()
    {
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

    std::function<int(const glm::mat4&)> racket2_deserialize()
    {
        return [&](const glm::mat4& value) -> int
        {
            _racket2Model = value;
            return 0;
        };
    }

    std::function<int(const glm::mat4&)> racket1_deserialize()
    {
        return [&](const glm::mat4& value) -> int
        {
            _racket1Model = value;
            return 0;
        };
    }

    std::function<int(const glm::mat4&)> ball_deserialize()
    {
        return [&](const glm::mat4& value) -> int
        {
            _ballModel = value;
            return 0;
        };
    }

    std::function<int(const glm::mat4&)> view_deserialize()
    {
        return [&](const glm::mat4& value) -> int
        {
            _view = value;
            return 0;
        };
    }

    std::function<int(const glm::mat4&)> proj_deserialize()
    {
        return [&](const glm::mat4& value) -> int
        {
            _proj = value;
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

	int init(const rendering_settings& settings)
    {
        _settings = settings;

        camera = Camera{ glm::vec3(0.0f, 0.0f, 3.0f) };
        lastX = settings.window_width / 2.f;
        lastY = settings.window_height / 2.f;
        firstMouse = true;
        deltaTime = 0.0f;
        lastFrame = 0.0f;

        _freeCamEnabled = settings.free_cam_enabled;

        _view = glm::mat4(1.0f);
        _proj = glm::perspective(glm::radians(camera.Zoom), (float)_settings.window_width / (float)_settings.window_height, 0.1f, 100.0f);
        _ballModel = glm::mat4(1.0f);
        _ballModel = glm::translate(_ballModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        _ballModel = glm::scale(_ballModel, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        _racket1Model = _racket2Model = _ballModel;

        // ---------------------------------------
        // GLFW Initialization
        // ---------------------------------------
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;

        _window = glfwCreateWindow(settings.window_width, settings.window_height, "Virtual Tennis", NULL, NULL);
        if (!_window) {
            return 1;
        }
        glfwMakeContextCurrent(_window);
        glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
        glfwSetCursorPosCallback(_window, mouse_callback);
        glfwSetScrollCallback(_window, scroll_callback);
        //glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        const char* glsl_version = "#version 330 core";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // ---------------------------------------
        // GLAD Initialization
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        stbi_set_flip_vertically_on_load(true);

        glEnable(GL_DEPTH_TEST);

        ourShader = std::make_unique<Shader>("data/1.model_loading.vs", "data/1.model_loading.fs");
        _racketObject = std::make_unique<Model>("data/racket.fbx");
        _ballObject = std::make_unique<Model>("data/ball.fbx");

        // ---------------------------------------
        // Frame Initialization
        // ---------------------------------------
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
        glGenBuffers(1, &_frameVerticesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _frameVerticesBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
        glGenBuffers(1, &_frameUVsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _frameUVsBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

        _frameShader = std::make_unique<Shader>("data/fullscreen.vs", "data/fullscreen.fs");

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenTextures(1, &_frameTextureId);
        glBindTexture(GL_TEXTURE_2D, _frameTextureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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

        // ---------------------------------------
        // ImGUI Initialization
        // ---------------------------------------
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        std::cout << "Hot keys:" << std::endl;
        std::cout << "\tESC - quit the program" << std::endl;
        std::cout << "\tp - pause video" << std::endl;

        return 0;
    }

	int run_tick()
    {
        _quit = _quit || glfwWindowShouldClose(_window);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(_window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Main Menu");

            ImGui::Text("Main Menu");

            if (ImGui::Button("Cool")) {
                //
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f * deltaTime, 1.f / deltaTime);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _frameShader->use();

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
        glBindTexture(GL_TEXTURE_2D, _frameTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glClear(GL_DEPTH_BUFFER_BIT);

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
        ourShader->setMat4("projection", _proj);
        ourShader->setMat4("view", view);

        // render the loaded model
        ourShader->setMat4("model", _ballModel);
        _ballObject->Draw(*ourShader);
        ourShader->setMat4("model", _racket1Model);
        _racketObject->Draw(*ourShader);
        ourShader->setMat4("model", _racket2Model);
        _racketObject->Draw(*ourShader);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(_window);
        glfwPollEvents();

        return 0;
    }

	int term()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glDeleteBuffers(1, &_frameVerticesBufferId);
        glDeleteBuffers(1, &_frameUVsBufferId);
        glDeleteProgram(_frameShader->ID);
        glDeleteTextures(1, &_frameTextureId);
        glDeleteVertexArrays(1, &_frameVerticesId);

        glDeleteProgram(ourShader->ID);

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

std::function<int(const glm::mat4&)> rendering_manager::proj_deserialize()
{
    return _impl->proj_deserialize();
}

std::function<int(const glm::mat4&)> rendering_manager::view_deserialize()
{
    return _impl->proj_deserialize();
}

std::function<int(const glm::mat4&)> rendering_manager::ball_deserialize()
{
    return _impl->proj_deserialize();
}

std::function<int(const glm::mat4&)> rendering_manager::racket1_deserialize()
{
    return _impl->proj_deserialize();
}

std::function<int(const glm::mat4&)> rendering_manager::racket2_deserialize()
{
    return _impl->proj_deserialize();
}
