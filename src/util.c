#include "../include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>

int read_file(const char* path, char** content){
    FILE* file = fopen(path, "rb"); // 使用二进制模式读取
    if(file == NULL){
        return -1; // 打开文件失败
    }
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET); // 将文件指针重新定位到文件开头

    //分配大小到content指针
    *content = (char*)malloc(file_size); //分配足够的内存来存储文件内容，不加1
    if(*content == NULL){
        fclose(file);
        return -1; // 内存分配失败
    }

    // 读取文件内容
    size_t read_size = fread(*content, 1, file_size, file);
    if(read_size != (size_t)file_size){
        free(*content);
        fclose(file);
        return -1; // 读取失败
    }
    // 不添加\0，因为是二进制
    fclose(file);
    return file_size; // 返回读取的字节数
}

int file_exists(const char* path){
    return access(path, F_OK) == 0;
}

int is_path_safe(const char* path, const char* root_dir){
    char resolved_path[PATH_MAX];
    char resolved_root[PATH_MAX];

    // 获取绝对路径
    if(realpath(path, resolved_path) == NULL){
        return 0; // 路径无效
    }
    if(realpath(root_dir, resolved_root) == NULL){
        return 0; // 根目录无效
    }

    // 检查resolved_path是否以resolved_root开头
    size_t root_len = strlen(resolved_root);
    if(strncmp(resolved_path, resolved_root, root_len) != 0){
        return 0; // 路径不在根目录内
    }
    // 确保根目录后是'/'或结束
    if(resolved_path[root_len] != '\0' && resolved_path[root_len] != '/'){
        return 0;
    }
    return 1;
}

void log_message(const char* level, const char* message, ...){
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    va_list args;
    va_start(args, message);
    fprintf(stderr, "[%s] %s: ", time_str, level);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
}

