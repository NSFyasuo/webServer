#include "../include/server.h"
#include <stdio.h>
#include <signal.h>

extern int server_running; //全局变量，表示服务器是否正在运行

void handle_signal(int sig){
        server_running = 0; //设置服务器运行状态为0，表示服务器停止运行

}



int main(int argc, char* argv[]){
    int port = 8080; //默认端口号
    char* root_dir = "../static"; //默认根目录

    signal(SIGINT, handle_signal); //注册信号处理函数，处理Ctrl+C中断信号

    if(start_server(port, root_dir) < 0){
        fprintf(stderr, "服务器启动失败\n");
        return -1;
    }


    return 0;
}
