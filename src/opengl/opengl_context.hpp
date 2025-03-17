#pragma once
#include "defines.hpp"

#include <EGL/egl.h>
#include <GL/gl.h>

#include <GLES2/gl2.h>

typedef struct opengl_context
{
    u32 shader_program;
} opengl_context;

typedef void (*GL_BIND_VERTEX_ARRAYS)(GLuint arrays);
inline GL_BIND_VERTEX_ARRAYS glBindVertexArray = (GL_BIND_VERTEX_ARRAYS)eglGetProcAddress("glBindVertexArrays");

typedef void (*GL_GEN_VERTEX_ARRAYS)(GLsizei n, GLuint *arrays);
inline GL_GEN_VERTEX_ARRAYS glGenVertexArrays = (GL_GEN_VERTEX_ARRAYS)eglGetProcAddress("glGenBuffers");

typedef void (*GL_DELETE_VERTEX_ARRAYS)(GLsizei n, GLuint *arrays);
inline GL_DELETE_VERTEX_ARRAYS glDeleteVertexArrays =
    (GL_DELETE_VERTEX_ARRAYS)eglGetProcAddress("glDeleteVertexArrays");

typedef void (*GL_VIEW_PORT)(GLint x, GLint y, GLsizei width, GLsizei height);
inline GL_VIEW_PORT glViewPort = (GL_VIEW_PORT)eglGetProcAddress("glViewPort");
