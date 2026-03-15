#ifndef _REQUEST_H_
#define _REQUEST_H_

//模拟http请求
//请求行
//请求头
//请求体

typedef struct {
    char method[16];    //请求方法
    char path[256];     //请求路径
    char version[16];   //http版本号

}Request;

//解析http请求报文，填充到Request结构体中
int parse_request(const char* request_str, Request* req);


//对url进行解码操作
void url_decode(char* dst, const char* src);

#endif //_REQUEST_H_