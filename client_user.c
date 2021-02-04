#include"im_client.h"
/*
初始化用户的结构体
*/
int init_user_struct(MYSELF* m) {
	memset(m, 0, sizeof(*m));
	m->w_id = -1;
	m->w_sockfd = -1;
	m->w_cstat = INIT_CSTAT;
	m->w_msgstat = MSG_SENDING;

	INIT_LIST_HEAD(&myself.w_flisthd);

	return 0;
}

/*
添加操作
@fs：操作数组
@count:操作数量
*/
int user_list_add(FRND_STAT* fs, short count) {
	struct list_head* pos, * head = &myself.w_flisthd;
	CLIENT_FRND *cf, *cp;
	for (int i = 0; i < count; i++) {
		printf("client:user %d,name %s ,id %d,stat %d\n",
			i, fs[i].fr_name, fs[i].fr_id, fs[i].fr_stat);
	}
	for (int i; i < count; i++) {
		list_for_each(pos, head) {//遍历双链表中的好友，使用指针便宜到结构体的头部
			cp = (CLIENT_FRND*)list_entry(pos, CLIENT_FRND, c_node));
			if (cp->c_id==fs[i].fr_id) {
				strcpy(cp->c_name, fs[i].fr_name);
				cp->c_stat = fs[i].fr_stat;
				break;
			}
		}
		if (pos != head) {
			printf("client:user in the friend list,update,status\n");
			break;
		}
		cf = (CLIENT_FRND *)malloc(sizeof(*cf));
		if (cf == NULL) {
			printf("client: malloc failed\n");
			continue;//加不成功就继续
		}
		printf("client:user not in the friend list,add this user now \n");
		memset(cf, 0, sizeof(*cf));
		strncpy(cf->c_name, fs[i].fr_name, MAXNAME_LEN-1);
		cf->c_id = fs[i].fr_id;
		cf->c_stat = fs[i].fr_stat;
		list_add(&(cf->c_node), head);
	}
	return 0;
}
/*
删除
*/
int user_list_del(FRND_STAT* fs, short count) {
	CLIENT_FRND* cp;
	struct list_head* pos = NULL, * head = &myself.w_flisthd;

	for (int i; i < count; i++) {
		list_for_each(pos, head) {//遍历双链表中的好友，使用指针offset到结构体的头部
			cp = (CLIENT_FRND *)list_entry(pos, CLIENT_FRND, c_node));
			if (cp->c_id == fs[i].fr_id) {//找到了
				
				break;
			}
		}
		if (pos == head) {
			continue;
		}
		cp= (CLIENT_FRND *)list_entry(pos, CLIENT_FRND, c_node ));
		list_del(&(cp->c_node));
		free(cp);
	}
	return 0;
}
int user_list_update(FRND_STAT* fs, short count) {
	for (int i = 0; i < count; i++) {
		printf("client:user %dth,name=%s,id=%d,stat=%d\n", i, fs[i].fr_name, fs[i].fr_id, fs[i].fr_stat);
	}
}
int user_list_output(FRND_STAT* fs, short count) {
	for (int i = 0; i < count ; i++) {
		printf("client:all list msg user=%d,name=%s,id=%d,stat=%d\n", i, fs[i].fr_name, fs[i].fr_id, fs[i].fr_stat);
	}
	return 0;
}


