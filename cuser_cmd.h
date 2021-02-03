
/*用户输入命令处理*/
#ifndef CUSER_CMD_H
#define CUSER_CMD_H

#define REG_CMD		"reg"
#define LOGIN_CMD	"login"
#define FLIST_CMD	"flist"
#define FADD_CMD	"add"
#define FDEL		"del"
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

/*帮助文档*/
#define REG_C_USAGE "reg <name> <passwd> use name and passwd regin。\n"
#define FLIST_C_USAGE "flist list add friend.\n"

#define INV_C_USAGE "input cmd cannot illegal"

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

