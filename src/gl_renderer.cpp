#include "headers/input.h"
#include "headers/gl_renderer.h"

// #############################
//          OpenGL Structs
// #############################
struct GL_CONTEXT
{
    GLuint programId;
};

// #############################
//          OpenGL Globals
// #############################
static GL_CONTEXT glContext;

// #############################
//          OpenGL Functions
// #############################

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION || severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        SM_ASSERT(false, "OpenGL Debug Message: %s", message);
    }
    else
    {
        SM_TRACE("OpenGL Debug Message: %s", message);
    }
}

GLuint CompileShader(GLenum shaderType, char *file, BumpAllocator *transientStorage)
{
    // create shader id
    GLuint id = glCreateShader(shaderType);

    int fileSize = 0;

    // open file with shader
    char *shader = read_file(file, &fileSize, transientStorage);

    if (!shader)
    {
        SM_ASSERT(false, "Failed to load shaders");
        return false;
    }

    // tells opengl to use the shader source code
    // and compile it
    glShaderSource(id, 1, &shader, 0);
    glCompileShader(id);

    int success;
    char shaderLog[2048] = {};

    // check if the shader compiled successfully
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        // get the shader log
        glGetShaderInfoLog(id, sizeof(shaderLog), NULL, shaderLog);
        SM_ASSERT(false, "Failed to compile %s shader: %s", file, shaderLog);
        SM_ASSERT(false, "Shader source: %s", shader)
        return 0;
    }

    return id;
}

void CreateProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    // create program id
    glContext.programId = glCreateProgram();

    // attach shaders to program
    glAttachShader(glContext.programId, vertexShaderId);
    glAttachShader(glContext.programId, fragmentShaderId);

    // link program
    glLinkProgram(glContext.programId);

    int success;
    char programLog[2048] = {};

    // check if the program linked successfully
    glGetProgramiv(glContext.programId, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        // get the program log
        glGetProgramInfoLog(glContext.programId, sizeof(programLog), NULL, programLog);
        SM_ASSERT(false, "Failed to link program: %s", programLog);
        return;
    }

}

bool gl_init(BumpAllocator *transientStorage)
{
    load_gl_functions();

    glDebugMessageCallback(gl_debug_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertexShaderId = CompileShader(GL_VERTEX_SHADER, "assets/shaders/quad.vert", transientStorage);
    GLuint fragmentShaderId = CompileShader(GL_FRAGMENT_SHADER, "assets/shaders/quad.frag", transientStorage);

    if (!vertexShaderId || !fragmentShaderId)
    {
        SM_ASSERT(false, "Failed to compile shaders");
        return false;
    }

    SM_TRACE("Vertex Shader ID: %d", vertexShaderId);
    SM_TRACE("Fragment Shader ID: %d", fragmentShaderId);

    CreateProgram(vertexShaderId, fragmentShaderId);
    SM_TRACE("Program ID: %d", glContext.programId);

    if (!glContext.programId)
    {
        SM_ASSERT(false, "Failed to create program");
        return false;
    }

    // detach shaders from program
    glDetachShader(glContext.programId, vertexShaderId);
    glDetachShader(glContext.programId, fragmentShaderId);

    // delete shaders
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programId);

    return true;
}

void gl_render()
{
    glClearColor(119.0f / 255.0f, 33.0f / 255.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, input.screenSizeX, input.screenSizeY);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}