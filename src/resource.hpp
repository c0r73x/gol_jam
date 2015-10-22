#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_

#include <SDL2/SDL_opengl.h>
#include <unistd.h>
#include <stdio.h>
#include <GL/glu.h>

#include <vector>

#include "errorhandler.hpp"

#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

class Resource
{
public:
    Resource()
    {
        filename = 0;
    }
    virtual ~Resource() {}
    virtual int load(const char *filename) = 0;

    char *filename;
};

class TextureResource : public Resource
{
public:
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t type;

    TextureResource()
    {
        id     = -1;
        width  = 0;
        height = 0;
        bpp    = 0;
        type   = 0;

        glGenTextures(1, &id);
    }
    ~TextureResource()
    {
        glDeleteTextures(1, &id);
    }
};

class ShaderResource : public Resource
{
public:
    ShaderResource()
    {
    }
    ~ShaderResource()
    {
    }
};

class ModelResource : public Resource
{
public:
    int num_tris;

    uint32_t vertex_buffer;
    uint32_t indices_buffer;
    uint32_t uv_buffer;
    uint32_t normals_buffer;

    ModelResource()
    {
        num_tris = 0;
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &indices_buffer);
        glGenBuffers(1, &uv_buffer);
        glGenBuffers(1, &normals_buffer);
    }
    ~ModelResource()
    {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &indices_buffer);
        glDeleteBuffers(1, &uv_buffer);
        glDeleteBuffers(1, &normals_buffer);
    }
};

typedef std::vector<Resource *> resContainer;

class ResourceHandler
{
public:
    ResourceHandler();
    ~ResourceHandler();

    int init();
    void update();

    TextureResource *getTexture(const char *filename);
    ModelResource *getModel(const char *filename);
private:
    Resource *getResource(const char *filename);
    Resource *getByType(const char *ext);

    resContainer resources;

    char datapath[FILENAME_MAX];
    int watcher;
    int inotify;
};

#endif // RESOURCE_HPP_
