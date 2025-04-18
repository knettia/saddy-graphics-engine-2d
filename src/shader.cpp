#include "shader.h"

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX 
#endif
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#endif

#ifdef X11
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif

#include <3rdparty/glext/glext.h>

#include "renderer.h"
#include "opengl.h"
#include "slurp.h"

#include "log/log.h"

#include "os/extensionfunctions.h"
#include "opticksupport.h"

DECLARE_SOBJ(sad::Shader)



sad::Shader::Shader() : m_is_on_gpu(false), m_program(0), m_renderer(nullptr), m_f(nullptr)
{
    PROFILER_EVENT;
    m_renderer = sad::Renderer::ref();
    m_f = m_renderer->opengl()->extensionFunctions();
}

sad::Shader::Shader(const sad::Shader& o) : m_is_on_gpu(false),
m_program(0),
m_vertex_program(o.m_vertex_program),
m_fragment_program(o.m_fragment_program),
m_renderer(o.m_renderer)
{
    PROFILER_EVENT;
    if (!m_renderer)
    {
        m_renderer = sad::Renderer::ref();
    }
    m_f = m_renderer->opengl()->extensionFunctions();
}

sad::Shader& sad::Shader::operator=(const sad::Shader& o)
{
    PROFILER_EVENT;
    tryDestroy();
    m_is_on_gpu = false;
    m_program = 0;
    m_vertex_program = o.m_vertex_program;
    m_fragment_program = o.m_fragment_program;
    m_renderer = o.m_renderer;
    m_f = o.m_f;
    return *this;
}

sad::Shader::~Shader()
{
    PROFILER_EVENT;
    tryDestroy();
}

void sad::Shader::setRenderer(sad::Renderer* r)
{
    PROFILER_EVENT;
    tryDestroy();
    m_renderer = r;
    if (!m_renderer)
    {
        m_renderer = sad::Renderer::ref();
    }
    m_f = m_renderer->opengl()->extensionFunctions();
}

sad::Renderer* sad::Shader::renderer() const
{
    PROFILER_EVENT;
    return m_renderer;
}

void sad::Shader::setVertexProgram(const sad::String& vertexProgram)
{
    PROFILER_EVENT;
    tryDestroy();
    m_vertex_program.setValue(vertexProgram);
}

void sad::Shader::clearVertexProgram()
{
    PROFILER_EVENT;
    m_vertex_program.clear();
}

void sad::Shader::setFragmentProgram(const sad::String& fragmentProgram)
{
    PROFILER_EVENT;
    tryDestroy();
    m_fragment_program.setValue(fragmentProgram);
}

void sad::Shader::clearFragmentProgram()
{
    PROFILER_EVENT;
    m_fragment_program.clear();
}


bool sad::Shader::loadVertexProgramFromFile(const sad::String& fileName)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;
    sad::Maybe<sad::String> maybe_string = sad::slurp(fileName, r);
    if (maybe_string.exists())
    {
        tryDestroy();
        m_vertex_program = maybe_string.value();
        return true;
    }
    return false;
}

bool sad::Shader::loadFragmentProgramFromFile(const sad::String& fileName)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;
    sad::Maybe<sad::String> maybe_string = sad::slurp(fileName, r);
    if (maybe_string.exists())
    {
        tryDestroy();
        m_fragment_program = maybe_string.value();
        return true;
    }
    return false;
}

void sad::Shader::tryUpload()
{
    PROFILER_EVENT;
    if (!m_is_on_gpu)
    {
        sad::Renderer* r = m_renderer;
        sad::os::ExtensionFunctions* f = m_f;

        GLint success = 0;
        const int info_log_length = 1024;
        GLchar info_log[info_log_length];

        GLuint vertex_program = this->tryCompileShader(GL_VERTEX_SHADER, m_vertex_program.mutableValue());
        if (vertex_program)
        {
            GLuint fragment_program = this->tryCompileShader(GL_FRAGMENT_SHADER, m_fragment_program.mutableValue());
            if (fragment_program)
            {
                m_program = f->glCreateProgram();
                f->glAttachShader(m_program, vertex_program);
                f->glAttachShader(m_program, fragment_program);
                f->glLinkProgram(m_program);
                f->glGetProgramiv(m_program, GL_LINK_STATUS, &success);
                if (!success)
                {
                    f->glGetProgramInfoLog(m_program, info_log_length, nullptr, info_log);
                    f->glDeleteProgram(m_program);
                    sad::String message = "Unable to link program with error, listed below:\n";
                    message += info_log;
                    SL_LOCAL_WARNING(message, *r);

                    m_program = 0;
                }
                else
                {
                    m_is_on_gpu = true;
                }

                f->glDeleteShader(vertex_program);
                f->glDeleteShader(fragment_program);
            }
        }
    }
}


void sad::Shader::use()
{
    PROFILER_EVENT;
    try {
        if (!m_is_on_gpu)
            this->tryUpload();
        m_f->glUseProgram(m_program);
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *m_renderer);
    }
}

void sad::Shader::disable()
{
    PROFILER_EVENT;
    try {
        m_f->glUseProgram(0);
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *m_renderer);
    }
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, int v0)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform1i, loc_name, v0);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, unsigned int v0)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform1ui, loc_name, v0);
}


void sad::Shader::setUniformVariable(const sad::String& loc_name, float v0)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform1f, loc_name, v0);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, int v0, int v1)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform2i, loc_name, v0, v1);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, unsigned int v0, unsigned int v1)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform2ui, loc_name, v0, v1);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, float v0, float v1)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform2f, loc_name, v0, v1);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, int v0, int v1, int v2)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform3i, loc_name, v0, v1, v2);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, unsigned int v0, unsigned int v1, unsigned int v2)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform3ui, loc_name, v0, v1, v2);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, float v0, float v1, float v2)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform3f, loc_name, v0, v1, v2);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, int v0, int v1, int v2, int v3)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform4i, loc_name, v0, v1, v2, v3);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform4ui, loc_name, v0, v1, v2, v3);
}

void sad::Shader::setUniformVariable(const sad::String& loc_name, float v0, float v1, float v2, float v3)
{
    PROFILER_EVENT;
    this->invoke(&sad::os::ExtensionFunctions::glUniform4f, loc_name, v0, v1, v2, v3);
}

void sad::Shader::tryDestroy() const
{
    PROFILER_EVENT;
    if (m_is_on_gpu)
    {
        m_f->glDeleteProgram(m_program);
        const_cast<sad::Shader*>(this)->m_is_on_gpu = false;
        const_cast<sad::Shader*>(this)->m_program = 0;
    }
}

void sad::Shader::tryDestroy()
{
    PROFILER_EVENT;
    if (m_is_on_gpu)
    {
        m_f->glDeleteProgram(m_program);
        m_is_on_gpu = false;
        m_program = 0;
    }
}

int sad::Shader::getUniformLocation(const sad::String& name)
{
    PROFILER_EVENT;
    if (name.length() == 0)
    {
        return 0;
    }
    if (m_is_on_gpu)
    {
        return m_f->glGetUniformLocation(m_program, name.c_str());
    }
    return 0;
}

unsigned int sad::Shader::getUniformBlockIndex(const sad::String& name)
{
    PROFILER_EVENT;
    if (name.length() == 0)
    {
        return GL_INVALID_INDEX;
    }
    if (m_is_on_gpu)
    {
        sad::Renderer* r = m_renderer;
        return m_f->glGetUniformBlockIndex(m_program, name.c_str());
    }
    return GL_INVALID_INDEX;
}

void sad::Shader::uniformBlockBinding(unsigned int uniformBlockIndex, unsigned int uniformBlockBinding)
{
    PROFILER_EVENT;
    if (m_is_on_gpu)
    {
        m_f->glUniformBlockBinding(m_program, uniformBlockIndex, uniformBlockBinding);
    }

}

void sad::Shader::setUniformMatrix4x3(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix4x3fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix3x4(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix3x4fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix4x2(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix4x2fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix2x4(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix2x4fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix3x2(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix3x2fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix2x3(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix2x3fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix4(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix4fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniformMatrix3(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix3fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}


void sad::Shader::setUniformMatrix2(int location, int count, bool transpose, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniformMatrix3fv(location, count, (transpose) ? GL_TRUE : GL_FALSE, value);
    }
}

void sad::Shader::setUniform4(int location, int count, const unsigned int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4uiv(location, count, value);
    }
}

void sad::Shader::setUniform3(int location, int count, const unsigned int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3uiv(location, count, value);
    }
}

void sad::Shader::setUniform2(int location, int count, const unsigned int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2uiv(location, count, value);
    }
}

void sad::Shader::setUniform1(int location, int count, const unsigned int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1uiv(location, count, value);
    }
}


void sad::Shader::setUniform4(int location, int count, const int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4iv(location, count, value);
    }
}

void sad::Shader::setUniform3(int location, int count, const int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3iv(location, count, value);
    }
}

void sad::Shader::setUniform2(int location, int count, const int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2iv(location, count, value);
    }
}

void sad::Shader::setUniform1(int location, int count, const int* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1iv(location, count, value);
    }
}


void sad::Shader::setUniform4(int location, int count, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4fv(location, count, value);
    }
}

void sad::Shader::setUniform3(int location, int count, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3fv(location, count, value);
    }
}

void sad::Shader::setUniform2(int location, int count, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2fv(location, count, value);
    }
}

void sad::Shader::setUniform1(int location, int count, const float* value)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1fv(location, count, value);
    }
}

void sad::Shader::setUniform(int location, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4ui(location, v0, v1, v2, v3);
    }
}

void sad::Shader::setUniform(int location, unsigned int v0, unsigned int v1, unsigned int v2)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3ui(location, v0, v1, v2);
    }
}


void sad::Shader::setUniform(int location, unsigned int v0, unsigned int v1)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2ui(location, v0, v1);
    }
}


void sad::Shader::setUniform(int location, unsigned int v0)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1ui(location, v0);
    }
}

void sad::Shader::setUniform(int location, int v0, int v1, int v2, int v3)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4i(location, v0, v1, v2, v3);
    }
}

void sad::Shader::setUniform(int location, int v0, int v1, int v2)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3i(location, v0, v1, v2);
    }
}

void sad::Shader::setUniform(int location, int v0, int v1)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2i(location, v0, v1);
    }
}


void sad::Shader::setUniform(int location, int v0)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1i(location, v0);
    }
}

void sad::Shader::setUniform(int location, float v0, float v1, float v2, float v3)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform4f(location, v0, v1, v2, v3);
    }
}

void sad::Shader::setUniform(int location, float v0, float v1, float v2)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform3f(location, v0, v1, v2);
    }
}

void sad::Shader::setUniform(int location, float v0, float v1)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform2f(location, v0, v1);
    }
}

void sad::Shader::setUniform(int location, float v0)
{
    PROFILER_EVENT;
    if (m_is_on_gpu && (location != -1))
    {
        return m_f->glUniform1f(location, v0);
    }
}

void sad::Shader::tryLogGlError(const char* op) const
{
    PROFILER_EVENT;
    if (!m_renderer)
    {
        return;
    }
    if (m_renderer->isGLGetErrorDebugCallsDisabled())
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

        sad::Renderer* r = m_renderer;
        SL_LOCAL_WARNING(error_data, *r);
    }
}

unsigned int sad::Shader::shaderId() const
{
    PROFILER_EVENT;
    return m_program;
}

// ======================================== PRIVATE METHODS  ========================================

unsigned int  sad::Shader::tryCompileShader(unsigned int shader_type, const sad::String& program_text) const
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;

    sad::os::ExtensionFunctions* f = m_f;

    GLint success = 0;
    const int info_log_length = 1024;
    GLchar info_log[info_log_length];

    GLuint  program = f->glCreateShader(shader_type);
    const GLchar* source = program_text.c_str();
    f->glShaderSource(program, 1, &source, nullptr);
    f->glCompileShader(program);
    f->glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        f->glGetShaderInfoLog(program, info_log_length, nullptr, info_log);
        sad::String message = "Unable to compile ";
        if (shader_type == GL_VERTEX_SHADER)
        {
            message += "vertex";
        }
        else
        {
            message += "fragment";
        }
        message += " shader with error, listed below\n";
        message += info_log;
        message += "\nCompiled shader was:\n";
        message += program_text;
        SL_LOCAL_WARNING(message, *r);
        f->glDeleteShader(program);
        return 0;
    }
    return program;
}

sad::os::ExtensionFunctions* sad::Shader::f() const
{
    PROFILER_EVENT;
    return m_f;
}

template<typename T, typename A>
void sad::Shader::invoke(T a, const sad::String& location_name, A v0)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;

    try {
        this->tryUpload();
        sad::os::ExtensionFunctions* f = m_f;
        GLint location = f->glGetUniformLocation(m_program, location_name.c_str());
        if (location != -1)
        {
            (f->*a)(location, v0);
        }
        else
        {
            sad::String message = sad::String("Uniform variable with name\"") + location_name + "\" is not found in shader program";
            SL_LOCAL_WARNING(message, *r);
        }
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *r);
    }
}

template<typename T, typename A>
void sad::Shader::invoke(T a, const sad::String& location_name, A v0, A v1)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;

    try {
        this->tryUpload();
        sad::os::ExtensionFunctions* f = m_f;
        GLint location = f->glGetUniformLocation(m_program, location_name.c_str());
        if (location != -1)
        {
            (f->*a)(location, v0, v1);
        }
        else
        {
            sad::String message = sad::String("Uniform variable with name\"") + location_name + "\" is not found in shader program";
            SL_LOCAL_WARNING(message, *r);
        }
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *r);
    }
}

template<typename T, typename A>
void sad::Shader::invoke(T a, const sad::String& location_name, A v0, A v1, A v2)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;

    try {
        this->tryUpload();
        sad::os::ExtensionFunctions* f = m_f;
        GLint location = f->glGetUniformLocation(m_program, location_name.c_str());
        if (location != -1)
        {
            (f->*a)(location, v0, v1, v2);
        }
        else
        {
            sad::String message = sad::String("Uniform variable with name\"") + location_name + "\" is not found in shader program";
            SL_LOCAL_WARNING(message, *r);
        }
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *r);
    }
}

template<typename T, typename A>
void sad::Shader::invoke(T a, const sad::String& location_name, A v0, A v1, A v2, A v3)
{
    PROFILER_EVENT;
    sad::Renderer* r = m_renderer;

    try {
        this->tryUpload();
        sad::os::ExtensionFunctions* f = m_f;
        GLint location = f->glGetUniformLocation(m_program, location_name.c_str());
        if (location != -1)
        {
            (f->*a)(location, v0, v1, v2, v3);
        }
        else
        {
            sad::String message = sad::String("Uniform variable with name\"") + location_name + "\" is not found in shader program";
            SL_LOCAL_WARNING(message, *r);
        }
    }
    catch (std::logic_error& ex) {
        SL_LOCAL_FATAL(ex.what(), *r);
    }
}
