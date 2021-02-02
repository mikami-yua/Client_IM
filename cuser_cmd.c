#include"im_client.h"

int user_cmd_process(char* buf) {
	char *token = NULL, * next_token = NULL, seps[] = "\t";
	token = strtok_s(buf, seps, &next_token);
	if (!token) {
		return 0;
	}
	//对用户状态检测
	if (user_stat_check(token) < 0) return -1;

	if (!_stricmp(token, REG_CMD)) {
		return reg_cmd_process(next_token);
	}
}

/*
用户状态检查
*/
int user_stat_check(char* token) {
	return 0;
}

/*
处理注册命令
*/
int  reg_cmd_process(char* n_token) {
	return 0;
}