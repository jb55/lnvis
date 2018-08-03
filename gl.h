
#ifndef LNVIS_GL_H
#define LNVIS_GL_H


#include <GL/gl.h>

#ifndef NO_GL_HEADERS

void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);

void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);

void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);

GLuint glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName);

void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

void glGenBuffers(GLsizei n, GLuint * buffers);
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
void glBindBuffer(GLenum target, GLuint buffer);
GLuint glCreateShader(GLenum shaderType);
void glCompileShader(GLuint shader);


void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);


void glShaderSource(GLuint shader, GLsizei count, const GLchar **string,
                    const GLint *length);

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
GLint glGetUniformLocation(GLuint program, const GLchar *name);
GLint glGetAttribLocation(GLuint program, const GLchar *name);
void glEnableVertexAttribArray(GLuint index);
void glDeleteShader(GLuint shader);

void glDeleteBuffers(GLsizei n, const GLuint * buffers);

void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const GLvoid * pointer);

void glUniform1i(GLint location, GLint i);
void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLuint glCreateProgram(void);
void glAttachShader(GLuint program, GLuint shader);
void glLinkProgram(GLuint program);

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose,
                        const GLfloat *value);

void glUseProgram(GLuint program);
void glUniform1f(GLint location, GLfloat v0);
void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void glDeleteProgram(	GLuint program);

void glDisableVertexAttribArray(GLuint index);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);

GLboolean glIsBuffer(	GLuint buffer);

#endif


#endif /* LNVIS_GL_H */
