#include "os/ubo.h"
#include "os/extensionfunctions.h"
#include "opengl.h"
#include "renderer.h"

sad::os::UBO::UBO(sad::Renderer* renderer, size_t buffer_size) : m_renderer(renderer), m_id(-1), m_buffer_size(buffer_size), m_f(nullptr), m_user_data(nullptr)
{
    if (!m_renderer)
    {
        m_renderer = sad::Renderer::ref();
    }
    m_f = m_renderer->opengl()->extensionFunctions();
}

sad::os::UBO::UBO(const sad::os::UBO& ubo) : m_renderer(ubo.m_renderer), m_id(-1), m_buffer_size(ubo.m_buffer_size), m_f(ubo.m_f), m_user_data(ubo.m_user_data)
{

}

sad::os::UBO& sad::os::UBO::operator=(const sad::os::UBO& ubo)
{
    if (m_id != -1)
    {
        this->tryUnload();
    }
    m_renderer = ubo.m_renderer;
    m_id = ubo.m_id;
    m_buffer_size = ubo.m_buffer_size;
    m_f = ubo.m_f;
    m_user_data = ubo.m_user_data;
    return *this;
}


void sad::os::UBO::tryLoadToGPU()
{
    sad::os::ExtensionFunctions* f = m_f;
    f->glGenBuffers(1, &m_id);
    this->tryLogGlError("sad::os::UBO::tryLoadToGPU: glGenBuffers()");

    f->glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    this->tryLogGlError("sad::os::UBO::tryLoadToGPU: glBindBuffer(GL_UNIFORM_BUFFER, m_id)");

    f->glBufferData(GL_UNIFORM_BUFFER, m_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    this->tryLogGlError("sad::os::UBO::tryLoadToGPU: glBufferData");

    f->glBindBuffer(GL_UNIFORM_BUFFER, 0);
    this->tryLogGlError("sad::os::UBO::tryLoadToGPU: glBindBuffer(GL_UNIFORM_BUFFER, 0)");
}

void sad::os::UBO::tryUnload()
{
    if (m_id != -1)
    {
        sad::os::ExtensionFunctions* f = m_f;
        f->glDeleteBuffers(1, &m_id);
        this->tryLogGlError("sad::os::UBO::tryUnload: glBindBuffer(GL_UNIFORM_BUFFER, 0)");
        m_id = -1;
    }
}

void sad::os::UBO::setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    if (m_id == -1)
    {
        this->tryLoadToGPU();
    }
    sad::os::ExtensionFunctions* f = m_f;

    f->glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    this->tryLogGlError("sad::os::UBO::setSubData: glBindBuffer(GL_UNIFORM_BUFFER, m_id)");

    f->glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    this->tryLogGlError("sad::os::UBO::setSubData: glBufferSubData");

    f->glBindBuffer(GL_UNIFORM_BUFFER, 0);
    this->tryLogGlError("sad::os::UBO::setSubData: glBindBuffer(GL_UNIFORM_BUFFER, 0)");
}

void sad::os::UBO::setUserData(void* user_data)
{
    m_user_data = user_data;
}

void* sad::os::UBO::userData() const
{
    return m_user_data;
}

void sad::os::UBO::bind(GLintptr offset, GLuint uniformBlockBinding)
{
    sad::os::ExtensionFunctions* f = m_f;
    f->glBindBufferRange(GL_UNIFORM_BUFFER, uniformBlockBinding, m_id, offset, m_buffer_size);
}

GLuint sad::os::UBO::id() const
{
    return m_id;
}

sad::os::UBO::~UBO()
{
    if (m_id != -1)
    {
        this->tryUnload();
    }
}

void sad::os::UBO::tryLogGlError(const char* op) const
{
    sad::Renderer* r = m_renderer;
    if (!r)
    {
        r = sad::Renderer::ref();
    }

    if (r->isGLGetErrorDebugCallsDisabled())
    {
        return;
    }

    const GLenum err_code = glGetError();
    if (err_code != GL_NO_ERROR)
    {
        const sad::String error_string = reinterpret_cast<const char*>(gluErrorString(err_code));
        sad::String error_data = op;
        error_data += ": ";
        error_data += error_string;
        SL_LOCAL_WARNING(error_data, *r);
    }
}
