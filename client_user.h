#ifndef CLIENT_USER_H
#define CLIENT_USER_H

#include"msg_type.h"

#include"list.h"

struct client_friend {
	char c_name[MAXNAME_LEN];
	int c_id;
	struct list_head c_node;
};

#define ufrnd_for_each_entry(pos,head,member)\
	for(pos=(CLIEND_FRND *)list_entry(HEAD->next,CLIEND_FRND,member);\
		pos->member!=(head);\
		pos=(CLIEND_FRND *)list_entry(pos->member.next,CLIEND_FRND,member))

#define INIT_CSTAT 0
#define LOGIN_CSTAT 1
#define MAX_MSG_LEN 512
#define MSG_SENDING 0
#define MSG_RCVING 1


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
	char w_buf[MAX_MSG_LEN];
	char w_msgstat;//msg state: sending0 rcving1
	char w_msgtype;//expected msg type
}MYSELF;

extern struct MYSELF myself;

int init_user_struct(MYSELF* m);
int user_list_add(MYSELF* m);



#endif // !CLIENT_USER_H
