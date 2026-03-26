#ifndef RESPONSE_H
#define RESPONSE_H

/**
 * 生成 HTTP 响应报文
 * @param resp 输出参数，指向生成的响应字符串
 * @param code HTTP 状态码
 * @param type 响应内容的 MIME 类型
 * @param body 响应体内容
 * @param length 响应体长度
 * @return 响应总长度
 */
int make_response(char** resp, int code, const char* type, char* body, int length);

//根据请求路径生成http响应报文
int file_response(char* path, char** resp);

/**
 * 生成错误响应页面
 * @param code　错误码
 * @param msg　　错误信息
 * @return 响应长度
 */
int error_response(int code, char* msg, char** resp);

char* get_mime_type(char *path);

#endif // 