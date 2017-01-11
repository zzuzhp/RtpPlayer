#ifndef ___VIDEOOUT_H___
#define ___VIDEOOUT_H___

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "pro_stat.h"
#include "Common/AVJitter.h"
#include "Renders/AVOutBase.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
//// draw an i420 surface using OpenGL ES

class VideoOut : public AVOutBase
{
public:

    static VideoOut * create_instance(ANativeWindow * window)
    {
        return new (std::nothrow)VideoOut(window);
    }

private:

    VideoOut(ANativeWindow * window);

    ~VideoOut();

    bool init(int width, int height);

    void clear();

    void update_surface_size();

    bool init_program();

    bool init_shaders();

    bool init_textures(int width, int height);

    GLuint load_shader(const char * shader_string, int type);

    int  load_texture();

    void update_textures(const uint8_t  * y,
                         const uint8_t  * u,
                         const uint8_t  * v,
                         const int        width,
                         const int        height,
                         const int        stride);

    void mat4f_load_ortho(float   left,
                          float   right,
                          float   bottom,
                          float   top,
                          float   near,
                          float   far);

    /* EGL: managing drawing surface */
    int egl_init_display(ANativeWindow * window);

    int egl_get_surface_size(int & width, int & height);

    int egl_swap_buffers();

    void egl_destroy_display();

    void check_egl_error(const char * op);

    void on_frame(AVFrame * frame);

    void on_start_render(){}

    void on_end_render();

private:

    enum {PLANE_Y = 0};
    enum {PLANE_U = 1};
    enum {PLANE_V = 2};
    enum {PLANE_MAX = 3};

    struct GLTexture
    {
        const uint8_t   * data;
        int               width;
        int               height;
        int               stride;
        int               name;
        GLuint            id;
        GLint             uniform_var;
    } m_textures[PLANE_MAX];

    ANativeWindow             * m_window;

    int                         m_image_width;
    int                         m_image_height;

    int                         m_surface_width;
    int                         m_surface_height;

    EGLDisplay                  m_egl_display;
    EGLContext                  m_egl_context;
    EGLConfig                   m_egl_config;
    EGLSurface                  m_egl_surface;
    EGLNativeWindowType         m_window_type;

    GLuint                      m_program;
    GLuint                      m_vertex_shader;
    GLuint                      m_fragment_shader;

    GLint                       m_uni_matrix;
    GLuint                      m_position_handle;
    GLuint                      m_texture_coord;

    AVJitter                    m_jitter;
    CProStatBitRate             m_framerate;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___VIDEOOUT_H___
