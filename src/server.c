#include "../include/server.h"
#include "../include/request.h"
#include "../include/util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>


#define MAX_EVENTS 1000
#define BUFFER_SIZE 1024

int server_running = 1; //全局变量，表示服务器是否正在运行
    
int create_socket(int port) {
    struct sockaddr_in addr;
    

    /**
     * 创建套接字
     * AF_INET: IPv4 地址族
     * SOCK_STREAM: 面向连接的套接字类型，适用于 TCP 协议
     * 0: 协议参数，通常设置为 0，表示使用默认协议（对于 SOCK_STREAM 来说是 TCP）    
     */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        return -1;
    }
    //处理Address already in use错误
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // 设置服务器地址结构
    addr.sin_family = AF_INET; // IPv4
    // 将端口号转换为网络字节序并存储在地址结构中
    addr.sin_port = htons(port);
    // 监听所有可用的网络接口
    addr.sin_addr.s_addr = INADDR_ANY; 

    // 将套接字绑定到指定的地址和端口
    if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        return -1;
    }

    // 开始监听连接请求，允许最多 1000 个等待连接
    if(listen(fd, 1000) < 0){
        close(fd);
        return -1;
    }

    return fd; 
}

void set_non_blocking(int fd){
    //获取当前套接字的文件状态标志
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int start_server(int port, char* root_dir){

    struct sockaddr_in client_addr; //客户端地址结构
    socklen_t client_addr_len = sizeof(client_addr); //客户端地址长度
    


    //1.创建套接字
    int server_fd = create_socket(port);
    if(server_fd < 0){
        return -1;
    }

    set_non_blocking(server_fd);

    //创建epoll实例
    // EPOLL_CLOEXEC: 在执行 exec 系列函数时自动关闭 epoll 文件描述符，防止资源泄漏
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if(epfd < 0){
        close(server_fd);
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听可读事件
    ev.data.fd = server_fd; // 将服务器套接字的文件描述符存储在事件数据中
    // 将服务器套接字添加到 epoll 实例中，以便监视其事件
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) < 0){
        close(server_fd);
        close(epfd);
        return -1;
    }
    
    while(server_running){
        struct epoll_event events[MAX_EVENTS];
        // 等待事件发生，最多等待 1000 个事件
        /**
         * epoll_wait 函数的参数说明：
         * epfd: epoll 实例的文件描述符
         * events: 用于存储发生事件的数组
         * maxevents: events 数组的最大长度
         * timeout: 等待事件的超时时间，单位为毫秒，-1 表示无限等待
         */
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if(n < 0){
            break; // 错误发生，退出循环
        }

        for(int i = 0; i < n; i++){
            if(events[i].data.fd == server_fd){
                // 处理新的连接请求
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if(client_fd < 0){
                    continue; // 接受连接失败，继续处理下一个事件
                }

                //获取客户端的IP地址和端口号
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                
                int client_port = ntohs(client_addr.sin_port);
                log_message("INFO", "新客户端已连接: %s:%d", client_ip, client_port);
                
                // 设置客户端套接字为非阻塞模式
                set_non_blocking(client_fd); 
                ev.events = EPOLLIN; // 监听可读事件
                ev.data.fd = client_fd; // 将客户端套接字的文件描述符存储在事件数据中
                // 将客户端套接字添加到 epoll 实例中，以便监视其事件
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) < 0){
                    close(client_fd); // 添加失败，关闭客户端套接字
                    continue; // 继续处理下一个事件
                }

            } else {
                // 处理客户端请求
                int client_fd = events[i].data.fd; // 获取发生事件的客户端套接字文件描述符
                // 处理客户端请求的函数，传入客户端套接字文件描述符和根目录c
                handle_client(client_fd);
                // 处理完成后，关闭客户端套接字
                close(client_fd);
                // 从 epoll 实例中移除客户端套接字，以停止监视其事件
                epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);

            }

        }
    }


    // 关闭服务器套接字和 epoll 实例
    close(server_fd);
    close(epfd);
    return 0;
}

void handle_client(int client_fd){
    char buffer[BUFFER_SIZE];
    /*
     * client_fd：套接字文件描述符，必须是已连接的（对于TCP）或已连接的数据报套接字（对于UDP，但通常用recvfrom）。
     * buffer：指向接收缓冲区的指针，用于存放接收到的数据。
     * len：接收缓冲区的大小（以字节为单位）。
     * flags：标志位，用于控制接收行为。
     */
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if(bytes <= 0){
        close(client_fd);
        return;
    }

    //将接受到的数据转化为普通字符串
    buffer[bytes] = '\0';

    Request req;
    char *response = NULL; //HTTP响应指针
    int resp_len = 0;
    //调用parse_request解析HTTP请求，并封装成Request对象
    if(parse_request(buffer, &req) == 1){

        log_message("INFO", "请求: %s %s %s", req.method, req.path, req.version);

        if(strcmp(req.method, "GET") == 0){
            char path[512];
            if(strcmp(req.path, "/") == 0){
                //127.0.0.1:8080    根路径/　---> /static/index.html
                snprintf(path, sizeof(path), "../static/index.html");
            }else{
                //其他路径映射到  ../static　＋　路径
                snprintf(path, sizeof(path), "../static%s", req.path);
            }

            if(is_path_safe(path, "../static") && file_exists(path)){
                resp_len = file_response(path, &response);
            }else{
                resp_len = error_response(404, "资源不存在", &response);
            }
        }else{
            resp_len = error_response(501, "请求方法不支持！", &response);
        }
    }else{
        //收到的数据不遵循ＨＴＴＰ协议
        log_message("ERROR", "无效的HTTP请求");
        resp_len = error_response(400, "请求错误", &response);
    }

    if(response){
        send(client_fd, response, resp_len, 0);
        free(response);
    }
    close(client_fd);
}
