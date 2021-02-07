#pragma once
#ifndef CLIENT_USER_H
#define CLIENT_USER_H
/*客户端好友管理*/
#include"msg_type.h"
#include"list.h"
//#include"im_client.h"

/*好友结构体*/
typedef struct client_friend {
	char c_name[MAXNAME_LEN];
	int c_id;
	struct list_head c_node;
	int c_stat;//好友状态
}CLIENT_FRND;

#define ufrnd_for_each_entry(pos,head,member)\
	for((pos=(CLIENT_FRND *)list_entry((head)->next,CLIENT_FRND,member));\
		(((&pos->member))!=(head));pos=(CLIENT_FRND *)list_entry((pos)->member.next,CLIENT_FRND,member))

#define INIT_CSTAT 0
#define LOGIN_CSTAT 1
#define MAX_MSG_LEN 512
#define MSG_SENDING 0
#define MSG_RCVING 1

/* y用户本身的结构体 */
typedef struct whoami {
	int w_id;
	char w_name[MAXNAME_LEN];
	char w_pass[MAX_USERPASSWD_LEN];
	int w_sockfd;//user socketfd
	int w_cstat;//user state: login or init 
	int msg_count;//msg count
	struct list_head s_msghd;//msg head
	struct list_head w_flisthd;//all friends user list head
	int w_fndcnt;//friend count
	char w_buf[MAX_MSG_LEN];//消息发送缓冲区
	char w_msgstat;//msg state: sending0 rcving1
	char w_msgtype;//expected msg type
}MYSELF;

extern struct whoami myself;

int init_user_struct(MYSELF* m);//初始化用户状态
int user_list_add(FRND_STAT*fs,short count);
int user_list_del(FRND_STAT* fs, short count);
int user_list_update(FRND_STAT* fs, short count);
int user_list_output(FRND_STAT* fs, short count);


#endif // !CLIENT_USER_H
