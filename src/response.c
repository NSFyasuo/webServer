#include "../include/response.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

char* make_response(int code, const char* type, char* body, int length){

    //1.创建存储数据的字符串
    char* response = (char*)malloc(512 + length); //分配足够的内存来存储响应报文
    if (response == NULL) {
        return NULL; //内存分配失败
    }

    //2.状态描述映射
    char* status = "OK";
    if (code == 404) {
        status = "Not Found";
    } else if (code == 500) {
        status = "Internal Server Error";
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

    return response;
}

int main(){
    char* body = "Hello, World!";
    char* response = make_response(200, "text/plain", body, strlen(body));
    printf("%s\n", response);
    free(response); //释放分配的内存
}
