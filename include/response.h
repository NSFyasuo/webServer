#ifndef RESPONSE_H
#define RESPONSE_H

//生成完整的http响应报文

char* make_response(int code, const char* type, char* body, int length);    //code:状态码 type:内容类型 body:内容 length:内容长度

//根据请求路径生成http响应报文
//char* file_response(const char* path);

#endif // 