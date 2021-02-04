#include"im_client.h"



/*初始化windowssocket*/
void socklib_init() {
	WSADATA wsa_data;
	int ret=WSAStartup(MAKEWORD(2,2),&wsa_data);
	if (ret != 0) {
		printf("client:windows socket load failed with err:%d", ret);
		exit(-1);
	}

}

/*socket清理*/
void sock_cleanup(int fd) {
	closesocket(fd);
	WSACleanup();
}

/*
socket的初始化
*/
int client_sock_init(char *ip) {
	int sfd;
	struct sockaddr_in serv;
	socklib_init();

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		printf("client:socket err \n");
		return -1;
	}

	memset(&serv,0,sizeof(serv));
	serv.sin_family=AF_INET;
	serv.sin_port = htons(SERVER_PORT);
	//判断地址是否正确
	if (inet_pton(AF_INET, ip, &serv.sin_addr) <= 0) {
		printf("client:inet_pton err for %s\n", ip);
		closesocket(sfd);
		return -2;
	}
	//连接服务器
	int con_num=connect(sfd, (struct sockaddr*)&serv, sizeof(serv));

	//判断连接是否成功
	if (con_num < 0) {
		printf("client:connect err\n");
		return -3;
	}

	return 0;
}

/*
初始化参数
*/
int init_args(int argc,char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage:%s <ipv4 ip>\n", argv[1]);
		return -1;
	}
	//init client socket fd
	myself.w_sockfd = client_sock_init(argv[1]);//通过ip地址初始化一个客户端的socket
	if (myself.w_sockfd < 0) {
		return -1;
	}

	return 0;
}

/*
多线程
*/
void* client_cli_thread(void* arg) {
	char line[MAX_LINE_LEN];
	char user_prompt[MAX_PROMPT_LEN];

	while (1) {
		if (myself.w_id != -1) {
			//有登录id
			_snprintf(user_prompt, MAX_PROMPT_LEN, "%s(%s)#",USER_PROM,myself.w_name);//im_client(alice)#
		}
		else {
			//没有登录id
			_snprintf(user_prompt, MAX_PROMPT_LEN, "%s(unkonwn)#", USER_PROM);
		}

		printf("%s", user_prompt);
		memset(line, 0, sizeof(line));
		//获取用户输入的命令
		if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
			break;
		}
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';
		}
		if (user_cmd_process(line) == -2) {//代表退出指令
			printf("client:user logout...\n");
			break;
		}
	}
	closesocket(myself.w_sockfd);
	return NULL;
}

/*
主线程
*/
void client_main_loop() {
	int n;

	pthread_t client_stdin_pid;
	pthread_create(&client_stdin_pid, NULL, client_cli_thread, NULL);//创建多线程

	//对网络子线程消息处理
	n = recv(myself.w_sockfd, myself.w_buf, MAX_MSG_SIZE,0);//从服务器收取报文
	while (n>0)
	{
		printf("client:recv msg len=%d\n", n);
		dec_server_resp(myself.w_buf,n);//解析服务器收到的消息
	}

	printf("client:recv len < 0 exiting...\n");
}



/*
要求只有一个参数ipv4
*/
int main(int argc, char** argv) {
	//用户结构初始化
	init_user_struct(&myself);
	//参数初始化
	if (init_aegs(argc, argv));
	//客户端主循环
	client_main_loop();//主线程
	//清理函数
	sock_cleanup(myself.w_sockfd);
	return 0;
}