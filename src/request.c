#include "../include/request.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//原始数据
//请求行
//请求头
//请求体
int parse_request(const char* request_str, Request* req) {
    //1.以strtok分割请求行 只取第一行
    char* line = strtok((char*)request_str, "\r\n");
    if (line == NULL) {
        return -1; //解析失败
    }

    //2.再次使用strtok分割请求行 换取三个部分
    char* method = strtok(line, " ");
    char* path = strtok(NULL, " ");
    char* version = strtok(NULL, " ");

    if (method == NULL || path == NULL || version == NULL) {
        return -1; //解析失败
    }

    //3.将解析结果存储到Request结构体中
    strncpy(req->method, method, sizeof(req->method) - 1);
    strncpy(req->path, path, sizeof(req->path) - 1);
    strncpy(req->version, version, sizeof(req->version) - 1);

    url_decode(req->path, path); //对路径进行url_decode解码
    strncpy(req->path, path, sizeof(req->path) - 1);

    //确保字符串以null结尾
    req->method[sizeof(req->method) - 1] = '\0';
    req->path[sizeof(req->path) - 1] = '\0';
    req->version[sizeof(req->version) - 1] = '\0';

    return 1; //成功解析
}

//4.对路径字符串进行url_decode解码

void url_decode(char* dst, const char* src){
    while (*src) {
        if (*src == '%') {
            //将%后面的两个十六进制字符转换为一个字节
            char hex[3] = { src[1], src[2], '\0' };
            *dst++ = (char)strtol(hex, NULL, 16);
            src += 3; //跳过%和两个十六进制字符
        } else if (*src == '+') {
            *dst++ = ' '; //将+转换为空格
            src++;
        } else {
            *dst++ = *src++; //直接复制其他字符
        }
    }
    *dst = '\0'; // 确保以null结尾
}

