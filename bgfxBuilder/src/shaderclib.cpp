#include <cstdarg>
#include <cstdio>

namespace bgfx
{
	typedef void(*UserErrorFn)(void*, const char*, va_list);
	static UserErrorFn s_user_error_fn = nullptr;
	static void* s_user_error_ptr = nullptr;
	void setShaderCErrorFunction(UserErrorFn fn, void* user_ptr)
	{
		s_user_error_fn = fn;
		s_user_error_ptr = user_ptr;
	}
}

void printError(FILE* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	if (bgfx::s_user_error_fn)
	{
		bgfx::s_user_error_fn(bgfx::s_user_error_ptr, format, args);
	}
	else
	{
		vfprintf(file, format, args);
	}
	va_end(args);
}


#define fprintf printError
#define main GPfakeMain
#include "../../bgfx/tools/shaderc/shaderc.cpp"
#include "../../bgfx/tools/shaderc/shaderc_hlsl.cpp"
#include "../../bgfx/tools/shaderc/shaderc_glsl.cpp"
#define g_allocator g_shaderc_allocator
//#include "shaderc_spirv.cpp"
namespace bgfx 
{
	bool compilePSSLShader(const Options&, uint32_t, const std::string&, bx::WriterI*)
	{
		return false;		
	}
	bool compileSPIRVShader(const Options&, uint32_t, const std::string&, bx::WriterI*)
	{
		return false;
	}
}
#undef fprintf
#include "bx/allocator.h"
