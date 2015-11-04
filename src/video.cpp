#include "video.hpp"
#include "errorhandler.hpp"

#include "engine.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


int Video::init(int width, int height)
{
    screen_width = width;
    screen_height = height;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_RendererInfo ri;
    int ret = SDL_CreateWindowAndRenderer(
                  width,
                  height,
                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE,
                  &window,
                  &renderer);

    if(ret != 0) {
        lprintf(LOG_ERROR, "Failed to create SDL window!");
        return 1;
    }

    context = SDL_GL_CreateContext(window);

    if(!context) {
        lprintf(LOG_ERROR, "Failed to create OpenGL context!");
        return 1;
    }

    SDL_GetRendererInfo(renderer, &ri);

    if(!(ri.flags & SDL_RENDERER_ACCELERATED) ||
        !(ri.flags & SDL_RENDERER_TARGETTEXTURE)) {

        lprintf(LOG_ERROR, "Unsupported hardware!");
        return 1;
    }

    #ifdef _WIN32

    if(gl3wInit()) {
        lprintf(LOG_ERROR, "Failed to initialize OpenGL!");
        return 1;
    }

    if(!gl3wIsSupported(3, 2)) {
        lprintf(LOG_ERROR, "OpenGL 3.2 not supported!");
        return 1;
    }

    #endif
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0.3f, 0.3f, 0);
    glViewport(0, 0, width, height);

    lprintf(LOG_INFO, "Video started successfully");
    lprintf(
        LOG_INFO,
        "^cOpenGL:^0\t%s",
        glGetString(GL_VERSION));
    lprintf(
        LOG_INFO,
        "^cShaderModel:^0\t%s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    lprintf(
        LOG_INFO,
        "^cVendor:^0\t%s",
        glGetString(GL_VENDOR));
    lprintf(
        LOG_INFO,
        "^cRenderer:^0\t%s",
        glGetString(GL_RENDERER));

    ProjMat = glm::perspective(
            45.0f,
            static_cast<float>(width) /
            static_cast<float>(height),
            0.1f,
            1000.0f);

    testentity[0].init("test1", "scripts/test.as");
    testentity[1].init("test2", "scripts/test2.as");
    testentity[2].init("test3", "scripts/test3.as");

    return 0;
}

void Video::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    ProjMat = glm::perspective(
            45.0f,
            static_cast<float>(width) /
            static_cast<float>(height),
            0.1f,
            1000.0f);

    screen_width = width;
    screen_height = height;
}

void Video::checkOpenGLErrors()
{
    GLenum err = glGetError();

    while(err != GL_NO_ERROR) {
        lprintf(LOG_ERROR, "OpenGL: %s (%i)", gluErrorString(err), err);
        err = glGetError();
    }
}

void Video::update()
{
    checkOpenGLErrors();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.pos = glm::vec3(0, 0, 5);


    glm::mat4 CamRotMat =
        glm::yawPitchRoll(
            -camera.yaw * RAD,
            -camera.pitch * RAD,
            0.0f);

    glm::mat4 CamTransMat = glm::translate(glm::mat4(1.0f), camera.pos);

    glm::mat4 CamMat = CamTransMat * CamRotMat;
    glm::mat4 ViewMat = glm::inverse(CamMat);

    camera.update(ProjMat, ViewMat);

    testentity[0].draw(ProjMat, ViewMat);
    testentity[1].draw(ProjMat, ViewMat);
    testentity[2].draw(ProjMat, ViewMat);

    SDL_GL_SwapWindow(window);
}

Video::Video()
{
    renderer = nullptr;
    window = nullptr;
    context = nullptr;

    screen_width = 0;
    screen_height = 0;
}

Video::~Video()
{
    lprintf(LOG_INFO, "Shutting down video");
}
