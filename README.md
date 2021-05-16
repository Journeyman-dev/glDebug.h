# glDebug.h 
_MIT Licensed OpenGL Debuging Library_

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
