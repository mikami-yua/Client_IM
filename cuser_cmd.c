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
	else if (!_stricmp(token, LOGIN_CMD)) {
		return login_cmd_proc(next_token);
	}
	else if (!_stricmp(token, FLIST_CMD)) {
		return flist_cmd_proc(next_token);
	}
	else if (!_stricmp(token, FADD_CMD)) {
		return add_cmd_proc(next_token);
	}
	else if (!_stricmp(token, FDEL_CMD)) {
		return del_cmd_proc(next_token);
	}
	else if (!_stricmp(token, ALIST_CMD)) {
		return alist_cmd_proc(next_token);
	}
	else if (!_stricmp(token, FSTAT_CMD)) {
		return fstat_cmd_proc(next_token);
	}
	else if (!_stricmp(token, CHAT_CMD)) {
		return chat_cmd_proc(next_token);
	}
	else if (!_stricmp(token, EXIT_CMD)) {
		return logout_cmd_proc(next_token);
	}
	else if (!_stricmp(token, MESG_CMD)) {
		return msg_cmd_proc(next_token);
	}
	else if (!_stricmp(token, DEBUG_CMD)) {
		return debug_cmd_proc(next_token);
	}
	else if (!_stricmp(token, HELP_CMD)) {
		return help_cmd_proc(next_token);
	}
	else {
		fprintf(stderr, "%s%s", INV_C_USAGE, CLENT_USAGE);
		return -1;
	}
}

/*
对命令的各个字段进行分解
@n_tok：去除了命令关键字的命令行字符串指针
@p:各个字段的指针数组
@n：数组的成员个数
@strict：格式是否是严格的
*/
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
	if (_stricmp(token,REG_CMD) &&
		_stricmp(token, LOGIN_CMD)&&
		_stricmp(token, HELP_CMD)&&
		_stricmp(token, MESG_CMD) &&
		_stricmp(token, EXIT_CMD) 
		) {
		if (myself.w_cstat != LOGIN_CSTAT) {
			fprintf(stderr, "%s", INV_STAT_USAGE);
			return -1;
		}
	}

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
int reg_cmd_process(char* n_token) {
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

/*
登录命令
*/
int login_cmd_proc(char* n_tok) {
	char* p[LOGIN_CMD_FLDS];
	int id,n;
	if (cmdline_proc(n_tok, p, LOGIN_CMD_FLDS, 1) == -1//判断字段有没有错
		|| !(id = (atoi(p[0])))
		|| strlen(p[1]) > MAX_USERPASSWD_LEN - 1//判断id和密码有没有问题
		) {
		fprintf(stderr, "%s%s\n", INV_C_USAGE, LOGIN_C_USAGE);
		return -1;
	}
	myself.w_id = id;
	memset(myself.w_pass, 0, MAX_USERPASSWD_LEN);
	strncpy(myself.w_pass, p[1], MAX_USERPASSWD_LEN - 1);
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	enc_login_msg(myself.w_buf, &n, 0);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;

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

/*
显示从from到to的消息
*/
int msg_cmd_proc(char* n_tok) {
	char* p[MESG_CMD_FLDS];
	int from, to;
	FILE* fp;

	if ((cmdline_proc(n_tok, p, MESG_CMD_FLDS, 1) == -1)
		|| !(from = atoi(p[0]))
		|| !(to = atoi(p[1]))
		) {
		fprintf(stderr, "%s%s", INV_C_USAGE, MESG_C_USAGE);
		return -1;
	}

	fp = fopen(CLIENT_LOG_FILE, "r");
	if (fp == NULL) {
		fprintf(stderr, "open file failde\n");
		return -1;
	}
	client_log_export(fp, stdout, from, to);

	fclose(fp);
}



int flist_cmd_proc(char* n_tok) {
	char* p[FLIST_CMD_FLDS];
	int n;
	if (cmdline_proc(n_tok, p, FLIST_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, FLIST_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	enc_fmgt_msg(myself.w_buf, &n, F_LREG,NULL,0);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

/*
添加好友
*/
int add_cmd_proc(char* n_tok) {
	int n, uid;
	char* p[ADD_CMD_FLDS];
	if (cmdline_proc(n_tok, p, ADD_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, FADD_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	uid = atoi(p[0]);
	enc_fmgt_msg(myself.w_buf, &n, F_ADD, &uid, 1);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

/*
删除好友
*/
int del_cmd_proc(char* n_tok) {
	int n, uid;
	char* p[DEL_CMD_FLDS];
	if (cmdline_proc(n_tok, p, DEL_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, FDEL_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	uid = atoi(p[0]);
	enc_fmgt_msg(myself.w_buf, &n, F_DEL, &uid, 1);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

/*
列出所有好友
*/
int alist_cmd_proc(char* n_tok) {
	char* p[ALIST_CMD_FLDS];
	int n;
	if (cmdline_proc(n_tok, p, ALIST_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, ALIST_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	enc_fmgt_msg(myself.w_buf, &n, F_ALET, NULL, 0);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

/*
查询好友状态
*/
int fstat_cmd_proc(char* n_tok) {
	int n, uid;
	char* p[FSTAT_CMD_FLDS];
	if (cmdline_proc(n_tok, p, FSTAT_CMD_FLDS, 1) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, FSTAT_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	uid = atoi(p[0]);
	enc_fmgt_msg(myself.w_buf, &n, F_STAT, &uid, 1);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

/*
发送消息
*/
int chat_cmd_proc(char* n_tok) {
	int n,rid;
	char* p[CHAT_CMD_FLDS];
	if (cmdline_proc(n_tok, p, CHAT_CMD_FLDS, 0) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, CHAT_C_USAGE);
		return -1;
	}
	memset(myself.w_buf, 0, MAX_MSG_SIZE);
	rid = atoi(p[0]);
	if (rid == 0) {//判断rid是否合法
		fprintf(stderr, "%s%s", INV_C_USAGE, CHAT_C_USAGE);
		return -1;
	}
	enc_chat_msg(myself.w_buf, &n, 0, rid, p[1]);
	client_send(myself.w_buf, n);
	myself.w_msgstat = MSG_RCVING;
	return 0;
}

int help_cmd_proc(char* n_tok) {
	char* p[HELP_CMD_FLDS];
	if (cmdline_proc(n_tok, p, HELP_CMD_FLDS, 0) == -1) {
		fprintf(stderr, "%s%s", INV_C_USAGE, HELP_C_USAGE);
		return -1;
	}
	fprintf(stdout, "%s", CLENT_USAGE);
	return 0;
}
//int usr_cmd_proc(char* n_tok);
int debug_cmd_proc(char* n_tok) {
	CLIENT_FRND* pos;
	printf("client:name=%s,id=%d,passwd=%s\n", myself.w_name, myself.w_id, myself.w_pass);
	printf("client:sockfd=%d,cstat=%d,friend count=%d\n", myself.w_sockfd, myself.w_cstat, myself.w_fndcnt);

	ufrnd_for_each_entry(pos, &myself.w_flisthd, c_node){
		printf("client:friend id=%d,name=%s,stat=%d", pos->c_id, pos->c_name, pos->c_stat);
	}
	return 0;
}