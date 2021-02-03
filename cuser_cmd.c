#include"im_client.h"


/*
流程：
	1.调用fgets函数从标准输入中获取用户输入命令，并对命令预处理
	2.使用strtok_s函数，讲用户命令的第一个字段解析出来（命令关键字）
	3.用多分支的if else判断命令的类型，根据命令嗲用不同的命令处理函数
	4.在命令处理中，调用cmdline_proc函数对用户输入的命令关键字后的各个字段进行提取
	5.若命令字段合法，则对字段内容进行合法性验证
	6.根据命令的类型判读，该命令是网络的命令还是本地的命令，若为网络命令，则对对应的消息进行封装，调用client_send，把封装的消息发往服务器
		本地命令，则调用响应的函数进行处理，命令处理完毕，再转至第一步，继续等待用户输入的下一个命令
*/


/*
命令处理函数
@buf：命令字符缓冲区
*/
int user_cmd_process(char* buf) {
	char *token = NULL, * next_token = NULL, seps[] = "\t";
	token = strtok_s(buf, seps, &next_token);
	if (!token) {
		return 0;
	}
	if (user_stat_check(token) < 0) return -1;

	if (!_stricmp(token, REG_CMD)) {
		return reg_cmd_process(next_token);
	}

	if (!_stricmp(token, LOGIN_CMD)) {
		return login_cmd_process(next_token);
	}
}

int cmdline_proc(char* n_tok, char** p, int n, int strict) {
	int count;//命令的个数
	char* token = NULL, seps[] = "\t";
	if (strict) {
		count = n;
	}
	else {
		count = n - 1;
	}
	for (int i = 0; i < count; i++) {//把所有的命令都解析了
		token = strtok_s(NULL, seps, &n_tok);
		p[i] = token;
		if (i != count - 1 && p[i] == NULL)//用户提供的参数不够
		{
			return -1;
		}
	}
	if (strict && p[count - 1]) {//定义为严格，但实际是不严的
		return -1;
	}
	if (!strict) {//非严格模式
		p[n - 1] = n_tok; 
		if (n_tok) {
			return -1;
		}
	}
}



/*
用户状态检查
*/
int user_stat_check(char* token) {
	return 0;
}

int client_send(char* buf, int n) {
	int len;
	len = send(myself.w_sockfd, buf, n, 0);
	if (len != n) {
		closesocket(myself.w_sockfd);
		init_user_struct(&myself);
	}


	return len;
}



/*
处理注册命令
*/
int  reg_cmd_process(char* n_token) {
	int n;
	char* p[REG_CMD_FLDS];
	//判断输入是否合法
	if (cmdline_proc(n_token, p, REG_CMD_FLDS, 1) < 0) {
		fprintf(stderr, "%s%s", INV_C_USAGE, REG_C_USAGE);
		return -1;
	}

	if (strlen(p[0]) > MAXNAME_LEN - 1) {//长度检查
		fprintf(stderr, "name too long,max=%d allowed%s\n", MAXNAME_LEN - 1,REG_C_USAGE);//-1是为了结尾标志
		return -1;
	}

	if (strcmp(p[1], p[2]) != 0) {//两次输入的密码是否相同
		fprintf(stderr, "passwords must match\n%s", REG_C_USAGE);//-1是为了结尾标志
	}
	if (strlen(p[1]) > MAX_USERPASSWD_LEN - 1) {//密码超长
		fprintf(stderr, "passwd too long,max=%d allowed%s\n", MAX_USERPASSWD_LEN - 1, REG_C_USAGE);//-1是为了结尾标志
		return -1;
	}

	strcpy(myself.w_name, p[0]);
	strcpy(myself.w_pass, p[1]);
	//清空myself的缓冲区，接下来需要编码发送了
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	enc_reg_msg(myself.w_buf,&n,0);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;//myself的状态是等待服务器返回消息

	return 0;

}

int login_cmd_proc(char* n_tok) {

	return 0;
}

int logout_cmd_proc(char* n_tok) {
	int n;
	char* p[LOGOUT_CMD_FLDS];
	if (cmdline_proc(n_tok, p, LOGOUT_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, EXIT_C_USAGE);
		return -1;
	}
	if (myself.w_cstat == LOGIN_CSTAT) {
		memset(myself.w_buf, 0, MAX_MSG_SIZE);
		enc_login_msg(myself.w_buf, &n, 0);
		send(myself.w_sockfd, myself.w_buf, n, 0);
		myself.w_cstat = MSG_RCVING;
	}

	return -2;
}