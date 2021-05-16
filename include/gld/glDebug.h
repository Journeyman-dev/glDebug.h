/*
MIT License

Copyright (c) 2021 Daniel Valcour

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* glDebug.h - MIT Licensed OpenGL Debuging Library
		
	This is a header only library. You need to do this in a c or cpp file somewhere in your project (only once)
	to implement the library:
			
		#define GLD_IMPLEMENTATION
		#include <gld/glDebug.h>
			
	glDebug.h is useful for producing verbose debug messages for OpenGL. Unlike the OpenGL debug callback system, 
	glDebug.h works with OpenGL versions older than 4.3. Also, glDebug.h works on the same thread as your OpenGL 
	calls. This allows for you to manually add debug breaks to your debug callback function (using __debugbreak() 
	in Visual Studio), so finding the exact location of an error is as easy as looking at the call stack.

	The OpenGL debugging techniques used in this library may be helpful for development, but they are also very
	perforamance heavy. Because of this, this library is designed with macros which only call debugging 
	related code if your project is in debug configuration (when _NDEBUG macro is not defined, which is handled 
	automatically by CMake). In release configuration, glDebug.h has no impact at all, and your code will compile
	as if it wasn't there in the first place. Furthermore, you can define the macro GLD_ACTIVE as 0 before 
	including glDebug.h to disable all library features in a region of your code even in debug configuration. This
	way, you can specify to only debug the OpenGL calls you are currently working on without having to add and remove
	the macros in other places.
		
	You are meant to write glDebug.h macros in your code like	they are functions with semicolons following them. 
	I have found this helpful when using the library in certain IDEs like Visual Studio, where if you don't use 
	semicolons the editor will try to unintended every macro call, making the code hard/annoying to keep tidy.
		
	glDebug.h has 5 macros that provide debugging functionality:
	 - GLD_CLEAR() - Call this macro once at the top of every block of OpenGL functions you
	  wish to debug. This function clears all OpenGL errors that are already enqueued in 
	  the state machine. If you don't call this first before using other error checking 
	  macros, you may get errors reported from previously executed parts of your code.
	 - GLD_SET_CALLBACK() - Call this macro only once, and do it before you call any of the
	  other debug macros in this library. Pass in a pointer to a function (a function pointer) that
	  returns void and takes in two const char* arguments. In this function you are meant to write
	  code you want to execute whenever an error occurs. The first argument is the type of error in 
	  textual form, and the second argument is a stringified version of the entire line where the error occurred.
	 - GLD_CALL(glFunc) - Pass in entire programming statments into this macro (without the semicolon)
	  to test any OpenGL calls in it for errors. You are meant to pass in entire statements, including 
	  both the rvalue and lvalue, but not the semicolon (though you should still add a semicolon after the macro).
	  You can also pass multiple statements into a single GLD_CALL macro as long as you don't add a semicoloon after
	  the last statement.
	 - GLD_COMPILE(shaderHandleVar) - Instead of calling glCompileShader(), call this to compile
	  and then check for compile errors.
	 - GLD_LINK(programHandleVar) - Instead of calling glLinkProgram(), call this to link and
	  then check for linking errors.
*/

#ifdef __cplusplus //extern C guard
extern "C" {
#endif

#ifndef NDEBUG
#ifndef GLD_ACTIVE
#define GLD_ACTIVE 1
#endif
#else
#define GLD_ACTIVE 0
#endif

#if GLD_ACTIVE == 1
#ifndef GLD_PROTOTYPES
#define GLD_PROTOTYPES
void glDebug_ClearErrors();
void glDebug_SetCallback(void (*callback)(const char*, const char*));
void glDebug_ErrorCheck(const char* glFunc);
void glDebug_CheckShaderCompile(const char* identifier, int handle);
void glDebug_CheckProgramLink(const char* identifier, int handle); 
#endif
#define GLD_CLEAR() glDebug_ClearErrors();
#define GLD_SET_CALLBACK(callback) glDebug_SetCallback(callback);
#define GLD_CALL(glFunc) glFunc; glDebug_ErrorCheck( #glFunc );
#define GLD_COMPILE(shaderHandleVar) glCompileShader(shaderHandleVar); glDebug_CheckShaderCompile( #shaderHandleVar , shaderHandleVar);
#define GLD_LINK(programHandleVar) glLinkProgram(programHandleVar); glDebug_CheckProgramLink( #programHandleVar, programHandleVar );
#else
#define GLD_CLEAR()
#define GLD_SET_CALLBACK(callback)
#define GLD_CALL(x) x;
#define GLD_COMPILE(shaderHandleVar) glCompileShader(shaderHandleVar);
#define GLD_LINK(programHandleVar) glLinkProgram(programHandleVar);
#endif

#ifdef GLD_IMPLEMENTATION
#ifndef GLD_IMPLEMENTED
#define GLD_IMPLEMENTED
#if GLD_ACTIVE != 0
static void (*sGL_DEBUG_CALLBACK) (const char*, const char*) = NULL;

void glDebug_SetCallback(void (*callback) (const char*, const char*))
{
	sGL_DEBUG_CALLBACK = callback;
}

void glDebug_ClearErrors() 
{
	while (glGetError() != GL_NO_ERROR);
}

#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_OUT_OF_MEMORY 0x0505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_CONTEXT_LOST 0x0507
#define GL_TABLE_TOO_LARGE 0x8031

void glDebug_ErrorCheck(const char* glFunc)
{
	if (!sGL_DEBUG_CALLBACK)
	{
		return;
	}
	
	GLenum err = glGetError();
	switch (err)
	{
	case GL_NO_ERROR:
		break;
	case GL_INVALID_ENUM:
		sGL_DEBUG_CALLBACK("glError: GL_INVALID_ENUM", glFunc);
		break;
	case GL_INVALID_VALUE:
		sGL_DEBUG_CALLBACK("glError: GL_INVALID_VALUE", glFunc);
		break;
	case GL_INVALID_OPERATION:
		sGL_DEBUG_CALLBACK("glError: GL_INVALID_OPERATION", glFunc);
		break;
	case GL_STACK_OVERFLOW:
		sGL_DEBUG_CALLBACK("glError: GL_STACK_OVERFLOW", glFunc);
		break;
	case GL_OUT_OF_MEMORY:
		sGL_DEBUG_CALLBACK("glError: GL_OUT_OF_MEMORY", glFunc);
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		sGL_DEBUG_CALLBACK("glError: GL_INVALID_FRAMEBUFFER_OPERATION", glFunc);
		break;
	case GL_CONTEXT_LOST:
		sGL_DEBUG_CALLBACK("glError: GL_CONTEXT_LOST", glFunc);
		break;
	case GL_TABLE_TOO_LARGE:
		sGL_DEBUG_CALLBACK("glError: GL_TABLE_TOO_LARGE", glFunc);
		break;
	default:
		sGL_DEBUG_CALLBACK("glError: unknown", glFunc);
		break;
	}
}

void glDebug_CheckShaderCompile(const char* identifier, int handle)
{
	GLint compiled;
	GLD_CALL(glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled));
	if (compiled != GL_TRUE && sGL_DEBUG_CALLBACK)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		GLD_CALL(glGetShaderInfoLog(handle, 1024, &log_length, message))
		if (log_length < 1024)
		{
			message[log_length] = '\0';
		}
		else
		{
			message[log_length-1] = '\0';
		}
		sGL_DEBUG_CALLBACK((const char*)&message, identifier);
	}
}

void glDebug_CheckProgramLink(const char* identifier, int handle)
{
	GLint program_linked;
	GLD_CALL(glGetProgramiv(handle, GL_LINK_STATUS, &program_linked));
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		GLD_CALL(glGetProgramInfoLog(handle, 1024, &log_length, message));
		if (log_length < 1024)
		{
			message[log_length] = '\0';
		}
		else
		{
			message[log_length-1] = '\0';
		}
		sGL_DEBUG_CALLBACK((const char*)&message, identifier);
	}
}
#endif
#endif
#endif


#ifdef __cplusplus //extern C guard
}
#endif