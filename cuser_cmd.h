
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

/*帮助文档*/
#define REG_C_USAGE "reg <name> <passwd> 通过用户名和密码注册用户。\n"
#define FLIST_C_USAGE "flist list add friend.\n"

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

#endif // !CUSER_CMD_H

