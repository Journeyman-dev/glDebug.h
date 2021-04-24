# glDebug.h 
_MIT Licensed OpenGL Debuging Library_

This is a header only library. You need to do this in a cpp file somewhere in your project (only once)
to implment the library:
		
	#define GLD_IMPLEMENTATION
	#include <gld/glDebug.h>
		
glDebug is useful for producing verbose debug messages for OpenGL. Unlike the debug callback system, 
glDebug works with OpenGL versions older than 4.3. Also, glDebug works on the same thread as your OpenGL 
calls. This allows for you to manually add debug breaks to your debug callback function, so finding the 
exact location of an error is as easy as looking at the call stack. Without tools like this libarary, that
kind of debugging is hard to do, and I often found myself confused as to what was wrong with my OpenGL code.

The OpenGL debugging techniques used in this library may be helpful for development, but they are also very
perforamance heavy. Because of this, this library is designed with macros which only call debugging 
related code if your project is in debug configuration. In release configuration, glDebug.h has no impact
at all, and your code will compile as if it wasn't there in the first place. Furthermore, you can define 
the macro GLD_ACTIVE as 0 before including glDebug.h to disable all library features in a region of
your code even in debug configuration. If you want more control you can include glDebug.h multiple times 
in a single code file, and then define GLD_ACTIVE to be 1 to enable or 0 to disable the library in
different sections of your code. This way, you can specify to only debug the OpenGL calls you are currently
working on without having to add and remove the macros often.
	
You are meant to write glDebug macros in your code like	they are functions with semicolons following them. 
I have found this helpful when using the library in certain IDEs like Visual Studio, where if you don't use 
semicolons the editor will try to unindent every macro call, making the code hard/annoying to keep tidy.
	
glDebug.h has 5 macros that provide debbugging functionality:
 - GLD_CLEAR() - Call this macro once at the top of every block of OpenGL functions you
  wish to debug. This function clears all opengl errors that are already enqueued in 
  the state machine. If you don't call this first before using other error checking 
  macros, you may get errors reported that were actually in completley different parts of your code.
 - GLD_SET_CALLBACK() - Call this macro only once, and do it before you call any of the
  other debug macros in this library. Pass in a pointer to a function (a function pointer) that
  returns void and takes in two const char* arguments. In this function you are meant to write
  code you want to execute whenever an error occurs. The first argument is the type of error in 
  textual form, and the second argument is a stringified version of the entire line where the error occured.
 - GLD_CALL(glFunc) - Pass in entire programming statments into this macro (without the semicolon)
  to test any OpenGL calls in it for errors. You are meant to pass in entire statements, including 
  both the rvalue and lvalue, but not the semicolon (though you should still add a semicolon after the macro).
 - GLD_COMPILE(shaderHandleVar) - Instead of calling glCompileShader(), call this instead to compile
  and then check for compile errors.
 - GLD_LINK(programHandleVar) - Instead of calling glLinkProgram(), call this instead to link and
  then check for linking errors.
