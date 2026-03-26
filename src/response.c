#include "../include/response.h"
#include "../include/util.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

int make_response(char** resp, int code, const char* type, char* body, int length){

    //1.创建存储数据的字符串
    char* response = (char*)malloc(512 + length); //分配足够的内存来存储响应报文
    if (response == NULL) {
        *resp = NULL;
        return 0; //内存分配失败
    }

    //2.状态描述映射
    char* status = "OK";
    if (code == 404) {
        status = "Not Found";
    } else if (code == 500) {
        status = "Internal Server Error";
    } else if (code == 400) {
        status = "Bad Request";
    } else if (code == 501) {
        status = "Not Implemented";
    }

    //3.生成时间戳
    time_t now = time(NULL);
    char time_str[128];
    strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));

    
    int pos = snprintf(response, 512,
                    "HTTP/1.1 %d %s\r\n"
                    "Date: %s\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n",  //响应头和响应体之间的空行
                    code, status, time_str, type, length);
    
        
    //4.将响应体内容复制到响应报文中
    if(body && length > 0){
        memcpy(response + pos, body, length); //将响应体内容复制到响应报文中
        
    }

    *resp = response;
    return pos + length;
}

int file_response(char* path, char** resp){
    char* data = NULL;
    int len = 0;
    //  读取路径上的文件
    len = read_file(path, &data);
    if(len > 0){
        //  根据文件尾部的扩展名确定Content-Type
        char* type = get_mime_type(path);
        int resp_len = make_response(resp, 200, type, data, len);
        free(data);
        return resp_len;
    } else {
        return error_response(404, "文件未找到", resp);
    }
}


int error_response(int code, char* msg, char** resp){
    char *html = (char*)malloc(512);
    if(!html) {
        *resp = NULL;
        return 0;
    }

    //格式化字符串
    int html_len = snprintf(html, 512, "<html>"
                        "<head>"
                        "    <title>出错了</title>"
                        "</head>"
                        "<body>"
                        "       <h1>%d 错误</h1>"
                        "       <p>%d 错误信息：%s</p>"
                        "</body>"
                        "</html>", code, code, msg);

    int resp_len = make_response(resp, code, "text/html", html, html_len);
    free(html);
    return resp_len;
}

//根据文件扩展名返回ＭＩＭＥ类型
char* get_mime_type(char *path){
    // 查找最后一个点号
    char* ext = strrchr(path, '.');
    if(!ext){
        return "application/octet-stream";
    }
    ext++; // 跳过点号
    if(strcasecmp(ext, "html") == 0 || strcasecmp(ext, "htm") == 0){
        return "text/html;charset=UTF-8";
    }else if(strcasecmp(ext, "css") == 0){
        return "text/css";
    }else if(strcasecmp(ext, "js") == 0){
        return "text/javascript";
    }else if(strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0){
        return "image/jpeg";
    }else if(strcasecmp(ext, "png") == 0){
        return "image/png";
    }else if(strcasecmp(ext, "gif") == 0){
        return "image/gif";
    }else if(strcasecmp(ext, "txt") == 0){
        return "text/plain;charset=UTF-8";
    }else if(strcasecmp(ext, "json") == 0){
        return "application/json";
    }else if(strcasecmp(ext, "xml") == 0){
        return "application/xml";
    }else{
        return "application/octet-stream";
    }
}