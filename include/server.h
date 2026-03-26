#ifndef __SERVER_H__
#define __SERVER_H__

int create_socket(int port);

/**
 * 启动服务器
 * @param port 服务器监听的端口号
 * @param root_dir 服务器的根目录，用于存储静态文件
 * @return 成功返回 0，失败返回 -1
 */
int start_server(int port, char* root_dir);

/**
 * 设置套接字为非阻塞模式
 * @param fd 套接字文件描述符
 */

void set_non_blocking(int fd);

/**
 * 处理客户端请求
 * @param client_fd 客户端套接字文件描述符
 */
void handle_client(int client_fd);


#endif // __SERVER_H__

