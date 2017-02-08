#include "Renders/VideoOut.h"

#define GL_UNPACK_ROW_LENGTH 0x0CF2

////////////////////////////////////////////////////////////////////////////////////////////////////
////

static const GLfloat g_vertexVertices[] =
{
    -1.0f,  -1.0f,
     1.0f,  -1.0f,
    -1.0f,   1.0f,
     1.0f,   1.0f
};

static const GLfloat g_textureVertices[] =
{
    0.0f,    1.0f,
    1.0f,    1.0f,
    0.0f,    0.0f,
    1.0f,    0.0f
};

static const char * g_s_vertexShader =
"    attribute vec4 position;                                   \n"
"    attribute vec2 texcoord;                                   \n"
"    uniform mat4 modelViewProjectionMatrix;                    \n"
"    varying vec2 v_texcoord;                                   \n"
"    void main()                                                \n"
"    {                                                          \n"
"        gl_Position = modelViewProjectionMatrix * position;    \n"
"        v_texcoord  = texcoord.xy;                             \n"
"    }                                                          \n";

static const char * g_s_textureShader =
"    varying highp vec2 v_texcoord;                                                                 \n"
"    uniform sampler2D  s_texture_y;                                                                \n"
"    uniform sampler2D  s_texture_u;                                                                \n"
"    uniform sampler2D  s_texture_v;                                                                \n"
"                                                                                                   \n"
"    void main()                                                                                    \n"
"    {                                                                                              \n"
"        highp float y = texture2D(s_texture_y, v_texcoord).r - 0.0625;                             \n"
"        highp float u = texture2D(s_texture_u, v_texcoord).r - 0.5;                                \n"
"        highp float v = texture2D(s_texture_v, v_texcoord).r - 0.5;                                \n"
"                                                                                                   \n"
"        gl_FragColor = clamp(vec4(mat3(1.1643,  1.16430, 1.1643,                                   \n"
"                                          0.0, -0.39173, 2.0170,                                   \n"
"                                       1.5958, -0.81290,    0.0) * vec3(y, u, v), 1.0), 0.0, 1.0); \n"
"    }                                                                                              \n";

VideoOut::VideoOut(ANativeWindow * window) : AVOutBase(true, "video renderer"),
                                             m_window(window),
                                             m_image_width(0),
                                             m_image_height(0),
                                             m_surface_width(0),
                                             m_surface_height(0),
                                             m_program(0),
                                             m_vertex_shader(0),
                                             m_fragment_shader(0)
{
    memset(m_textures, 0, sizeof(m_textures));
}

VideoOut::~VideoOut()
{
    stop();
}

bool
VideoOut::init(int width, int height)
{
    GLint status = GL_FALSE;
    if (!m_window)
    {
        RP_LOG_E("invalid param: window mustn't be null.");
        return false;
    }

    /* EGL */
    /* OpenGL context is bound to a thread; you cannot call OpenGL operations from another thread.
     * With other words, all OpenGL operations on a single context have to be called from the same thread that used to make the context current (with eglMakeCurrent).
     */
    if (egl_init_display(m_window) < 0)
    {
        return false;
    }

    /* GL ES */
    if (!init_shaders() ||
        !init_program() ||
        !init_textures(width, height))
    {
        goto fail;
    }

    return true;

fail:

    clear();

    return false;
}

void
VideoOut::clear()
{
    egl_destroy_display();

    if (m_program)
    {
        glUseProgram(0);
        glDeleteProgram(m_program);
        m_program = 0;

        if (m_vertex_shader)
        {
            glDeleteShader(m_vertex_shader);
            m_vertex_shader = 0;
        }

        if (m_fragment_shader)
        {
            glDeleteShader(m_fragment_shader);
            m_fragment_shader = 0;
        }
    }
}

void
VideoOut::on_end_render()
{
    clear();
}

void
VideoOut::on_frame(AVFrame * frame)
{
    if (frame->get_width() != m_image_width || frame->get_height() != m_image_height)
    {
        /* (re)init opengl */
        clear();

        RP_LOG_W("VideoOut image size changed from %dx%d to %dx%d.",
                  m_image_width, m_image_height, frame->get_width(), frame->get_height());

        if (!init(frame->get_width(), frame->get_height()))
        {
            RP_LOG_E("VideoOut init failed.(width: %d, height:%d).", frame->get_width(), frame->get_height());
            return;
        }

        m_image_width  = frame->get_width();
        m_image_height = frame->get_height();
    }

    /* render the image */
    update_surface_size();

    update_textures(frame->get_y(),
                    frame->get_u(),
                    frame->get_v(),
                    frame->get_width(),
                    frame->get_height(),
                    frame->get_stride());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    egl_swap_buffers();

    m_framerate.PushData(1);

    ///< RP_LOG_D("render frame rate %.2f", m_framerate.GetBitRate());
}

void
VideoOut::update_textures(const uint8_t  * y,
                          const uint8_t  * u,
                          const uint8_t  * v,
                          const int        width,
                          const int        height,
                          const int        stride)
{
    m_textures[PLANE_Y].data    = y;
    m_textures[PLANE_U].data    = u;
    m_textures[PLANE_V].data    = v;

    m_textures[PLANE_Y].width   = width;
    m_textures[PLANE_U].width   = width >> 1;
    m_textures[PLANE_V].width   = width >> 1;

    m_textures[PLANE_Y].height  = height;
    m_textures[PLANE_U].height  = height >> 1;
    m_textures[PLANE_V].height  = height >> 1;

    m_textures[PLANE_Y].stride  = stride;
    m_textures[PLANE_U].stride  = stride >> 1;
    m_textures[PLANE_V].stride  = stride >> 1;

    for (int texture = 0; texture < PLANE_MAX; ++texture)
    {
        glActiveTexture(m_textures[texture].name);
        glBindTexture(GL_TEXTURE_2D, m_textures[texture].id);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, m_textures[texture].stride);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_textures[texture].width, m_textures[texture].height, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_textures[texture].data);
        glUniform1i(m_textures[texture].uniform_var, texture);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }
}

bool
VideoOut::init_program()
{
    GLint status;
    /* create one program */
    m_program = glCreateProgram();

    /* attach the shaders */
    glAttachShader(m_program, m_vertex_shader);
    glAttachShader(m_program, m_fragment_shader);

    /* link the program */
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        RP_LOG_E("SurfaceDrawer: failed to link program!");
        return false;
    }

    glValidateProgram(m_program);
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        RP_LOG_E("failed to validate program!");
        return false;
    }

    glUseProgram(m_program);

    return true;
}

void
VideoOut::update_surface_size()
{
    /* check surface for every image */
    int width, height;
    egl_get_surface_size(width, height);

    if (width != m_surface_width || height != m_surface_height)
    {
        /* once surface size changed, update our viewport */
        m_surface_width  = width;
        m_surface_height = height;

        glViewport(0, 0, width, height);
    }
}

bool
VideoOut::init_shaders()
{
    m_vertex_shader = load_shader(g_s_vertexShader, GL_VERTEX_SHADER);
    if (!m_vertex_shader)
    {
        return false;
    }

    m_fragment_shader = load_shader(g_s_textureShader, GL_FRAGMENT_SHADER);
    if (!m_fragment_shader)
    {
        return false;
    }

    return true;
}

GLuint
VideoOut::load_shader(const char * shader_string, int type)
{
    int status;
    /* create shader */
    GLuint shader = glCreateShader(type);
    /* set shader source */
    glShaderSource(shader, 1, &shader_string, NULL);
    /* compile */
    glCompileShader(shader);
    /* check compile status */
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLchar infoLog[1024] = {0};
        glGetShaderInfoLog(shader, 1024, 0, infoLog);

        RP_LOG_E("load shader failed: %s", (char *)infoLog);
        return 0;
    }

    return shader;
}

bool
VideoOut::init_textures(int width, int height)
{
    m_textures[PLANE_Y].name = GL_TEXTURE0;
    m_textures[PLANE_U].name = GL_TEXTURE1;
    m_textures[PLANE_V].name = GL_TEXTURE2;

    m_textures[PLANE_Y].uniform_var = glGetUniformLocation(m_program, "s_texture_y");
    m_textures[PLANE_U].uniform_var = glGetUniformLocation(m_program, "s_texture_u");
    m_textures[PLANE_V].uniform_var = glGetUniformLocation(m_program, "s_texture_v");

    m_textures[PLANE_Y].width = width;
    m_textures[PLANE_U].width = width >> 1;
    m_textures[PLANE_V].width = width >> 1;

    m_textures[PLANE_Y].height = height;
    m_textures[PLANE_U].height = height >> 1;
    m_textures[PLANE_V].height = height >> 1;

    for (int texture = 0; texture < PLANE_MAX; ++texture)
    {
        glGenTextures(1, &m_textures[texture].id);

        glBindTexture(GL_TEXTURE_2D, m_textures[texture].id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_textures[texture].width, m_textures[texture].height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    /* vars int the shaders */
    m_uni_matrix      = glGetUniformLocation(m_program, "modelViewProjectionMatrix");
    m_position_handle = glGetAttribLocation(m_program,  "position");
    m_texture_coord   = glGetAttribLocation(m_program,  "texcoord");

    glVertexAttribPointer(m_position_handle, 2, GL_FLOAT, 0, 0, g_vertexVertices);
    glEnableVertexAttribArray(m_position_handle);

    glVertexAttribPointer(m_texture_coord, 2, GL_FLOAT, 0, 0, g_textureVertices);
    glEnableVertexAttribArray(m_texture_coord);

    mat4f_load_ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    return true;
}

void
VideoOut::mat4f_load_ortho(float left, float right, float bottom, float top, float near, float far)
 {
    float r_l = right - left;
    float t_b = top - bottom;
    float f_n = far - near;
    float tx  = -(right + left)/(right - left);
    float ty  = -(top + bottom)/(top - bottom);
    float tz  = -(far + near)/(far - near);

    GLfloat modelviewProj[16] =
    {
        2.0f/r_l,     0.0f,      0.0f, 0.0f,
            0.0f, 2.0f/t_b,      0.0f, 0.0f,
            0.0f,     0.0f, -2.0f/f_n, 0.0f,
              tx,       ty,        tz, 1.0f
    };

    /* update 'modelViewProjectionMatrix' in the vertex shader */
    glUniformMatrix4fv(m_uni_matrix, 1, GL_FALSE, modelviewProj);
}

int
VideoOut::egl_init_display(ANativeWindow * window)
{
    const EGLint attribs[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, ///< OpenGL ES 3 also uses EGL_OPENGL_ES2_BIT
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,       8,
        EGL_GREEN_SIZE,      8,
        EGL_RED_SIZE,        8,
        EGL_ALPHA_SIZE,      EGL_DONT_CARE,
        EGL_DEPTH_SIZE,      16,
        EGL_STENCIL_SIZE,    0,
        EGL_SAMPLE_BUFFERS,  1,
        EGL_NONE
    };

    EGLint context_attribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLint       format;
    EGLint       majorVersion;
    EGLint       minorVersion;
    EGLint       configCount = 0;
    EGLBoolean   eglStatus;

    m_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_egl_display == EGL_NO_DISPLAY)
    {
        check_egl_error("eglGetDisplay");
        return -1;
    }

    eglInitialize(m_egl_display, &majorVersion, &minorVersion);
    check_egl_error("eglInitialize");

    eglBindAPI(EGL_OPENGL_ES_API);

    eglStatus = eglChooseConfig(m_egl_display, attribs, &m_egl_config, 1, &configCount);
    if (!eglStatus || configCount != 1)
    {
        RP_LOG_E("EGL failed to return any matching configurations: %i", configCount);
        return -1;
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
     */
    eglGetConfigAttrib(m_egl_display, m_egl_config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    m_egl_surface = eglCreateWindowSurface(m_egl_display, m_egl_config, window, NULL);
    if (m_egl_surface == EGL_NO_SURFACE)
    {
        check_egl_error("eglCreateWindowSurface");
        return -1;
    }

    m_egl_context = eglCreateContext(m_egl_display, m_egl_config, EGL_NO_CONTEXT, context_attribs);
    if (m_egl_context == EGL_NO_CONTEXT)
    {
        check_egl_error("eglCreateContext");
        return -1;
    }

    if (eglMakeCurrent(m_egl_display, m_egl_surface, m_egl_surface, m_egl_context) == EGL_FALSE)
    {
        RP_LOG_E("Unable to eglMakeCurrent");
        return -1;
    }

    /* NOTE: 'interval' value of 0 causes the Samsung S6 to not render correctly on startup. */
    eglSwapInterval(m_egl_display, 1);

    return 0;
}

int
VideoOut::egl_get_surface_size(int & width, int & height)
{
    eglQuerySurface(m_egl_display, m_egl_surface,  EGL_WIDTH,  &width);
    eglQuerySurface(m_egl_display, m_egl_surface, EGL_HEIGHT, &height);

    return 0;
}

int
VideoOut::egl_swap_buffers()
{
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    eglWaitGL();

    return eglSwapBuffers(m_egl_display, m_egl_surface);
}

void
VideoOut::egl_destroy_display()
{
    if (m_egl_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (m_egl_surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_egl_display, m_egl_surface);
        }

        if (m_egl_context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_egl_display, m_egl_context);
        }

        eglTerminate(m_egl_display);
    }
}

void
VideoOut::check_egl_error(const char * op)
{
    for (EGLint error = eglGetError(); error!= EGL_SUCCESS; error = eglGetError())
    {
        RP_LOG_E("error::after %s() glError (0x%x)", op, error);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
