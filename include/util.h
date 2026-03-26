#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>

int read_file(const char* path, char** content); //读取文件内容到content指针，返回文件大小，失败返回-1

int file_exists(const char* path); //检查文件是否存在

int is_path_safe(const char* path, const char* root_dir); //检查路径是否安全，防止目录遍历

void log_message(const char* level, const char* message, ...); //日志函数

#endif // _UTIL_H_