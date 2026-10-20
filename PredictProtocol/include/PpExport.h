#pragma once

#ifndef PP_API
#ifdef PP_SHARED
#ifdef PP_BUILD
#define PP_API __declspec(dllexport)
#else
#define PP_API __declspec(dllimport)
#endif
#else
#define PP_API
#endif
#endif // !PP_API

#ifndef PP_C_API
#ifdef PP_SHARED
#ifdef PP_BUILD
#define PP_C_API extern "C" __declspec(dllexport)
#else
#define PP_C_API extern "C" __declspec(dllimport)
#endif
#else
#define PP_C_API extern "C"
#endif
#endif // !PP_C_API