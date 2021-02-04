#pragma once
/*用户输入命令处理*/
#ifndef CUSER_CMD_H
#define CUSER_CMD_H


#define C_LOC 1
#define C_SERV 2
/*
指令结构体
*/
typedef struct user_cmd {
	char* u_str;
	short u_local;
	short u_mtype;
	int u_optcnt;
}USR_CMD;






#define REG_CMD		"reg"
#define LOGIN_CMD	"login"
#define FLIST_CMD	"flist"
#define FADD_CMD	"add"
#define FDEL_CMD	"del"
#define ALIST_CMD	"alist"
#define FSTAT_CMD	"stat"
#define CHAT_CMD	"talk"
#define EXIT_CMD	"exit"
#define HELP_CMD	"help"
#define MESG_CMD	"mesg"
#define DEBUG_CMD	"debug"

#define MAX_LINE_LEN	256
#define MAX_PROMPT_LEN	64
#define USER_PROM		"im_client"

#define REG_CMD_FLDS	4//一条注册命令有四个字段
#define LOGIN_CMD_FLDS	3//登录命令3个字段
#define LOGOUT_CMD_FLDS 1
#define FLIST_CMD_FLDS 1
#define ADD_CMD_FLDS 2
#define DEL_CMD_FLDS 2
#define ALIST_CMD_FLDS 1
#define FSTAT_CMD_FLDS 2
#define CHAT_CMD_FLDS 2
#define HELP_CMD_FLDS 1
#define DEBUG_CMD_FLDS 1
#define MESG_CMD_FLDS 3

/*帮助文档*/
#define REG_C_USAGE "reg <name> <passwd> use name and passwd regin。\n"
#define LOGIN_C_USAGE "login <uid> <passwd>\n"
#define FLIST_C_USAGE "flist list add friend.\n"
#define FADD_C_USAGE "add <uid> add friend use id\n"
#define FDEL_C_USAGE "del <uid> delete friend use id\n"
#define ALIST_C_USAGE "alist search all online-users\n"
#define FSTAT_C_USAGE "fstat <uid> search friend state\n"
#define CHAT_C_USAGE "talk <uid> <msg> send msg to friend\n"
#define EXIT_C_USAGE "user exit\n"
#define HELP_C_USAGE "help print help msg\n"
#define MESG_C_USAGE "mesg <from> <to> display msg\n"
#define DEBUG_C_USAGE "debug display current user info\n"
#define CLIENT_C_USAGE "client cmd helper(<must input argc>)\n"
#define CLENT_USAGE		\
		CLIENT_C_USAGE	\
		LOGIN_C_USAGE		\
		FLIST_C_USAGE		\
		FADD_C_USAGE		\
		FDEL_C_USAGE		\
		ALIST_C_USAGE		\
		FSTAT_C_USAGE		\
		CHAT_C_USAGE		\
		EXIT_C_USAGE		\
		HELP_C_USAGE		\
		MESG_C_USAGE		\
		DEBUG_C_USAGE		\

#define INV_C_USAGE "input cmd cannot illegal\n"
#define INV_STAT_USAGE "please login first\n\n"

extern USR_CMD all_cmds[];

/*
命令处理函数
@buf:用户输入的命令字符缓冲区
*/
int user_cmd_process(char* buf);

/*
用户状态检查
*/
int user_stat_check(char* token);

/*
处理注册命令
*/
int  reg_cmd_process(char *n_token);

int login_cmd_proc(char* n_tok);

int logout_cmd_proc(char* n_tok);

int msg_cmd_proc(char* n_tok);
int flist_cmd_proc(char* n_tok);
int add_cmd_proc(char* n_tok);
int del_cmd_proc(char* n_tok);
int alist_cmd_proc(char* n_tok);
int fstat_cmd_proc(char* n_tok);
int chat_cmd_proc(char* n_tok);
int help_cmd_proc(char* n_tok);
//int usr_cmd_proc(char* n_tok);
int debug_cmd_proc(char* n_tok);
/*
对命令的各个字段进行分解
@n_tok：去除命令关键字的字符串
@p：各个字段的指针数组
@n：数组的成员个数
@strict:格式是否是严格的，是否是严格等于字段的【1|0】
*/
int cmdline_proc(char *n_tok,char **p,int n,int strict);

int client_send(char* buf, int n);



#endif // !CUSER_CMD_H

