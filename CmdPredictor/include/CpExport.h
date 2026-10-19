#pragma once

// CpExport.h
// 导入导出
#ifndef CP_API
#define CP_SHARED 
#ifdef CP_BUILD
// 编译本文件时，导出
#define CP_API __declspec(dllexport)
#else
// 使用本文件时，导入
#define CP_API __declspec(dllimport)
#endif
#endif