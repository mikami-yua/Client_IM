#include"im_client.h"

/*
初始化注册消息的结构体
*/
int init_reg_struct(REG_MSG* r, MYSELF* m) {
	memset(r, 0, sizeof(*r));
	memcpy(r->r_name, m->w_name, MAXNAME_LEN);
	strncpy(r->r_passwd, m->w_pass, MAX_USERPASSWD_LEN);
	return sizeof(REG_MSG);
}
/*
unsigned char type 
unsigned char stype
*/
void init_msg_hdr(MSG_HDR* h, unsigned char type, unsigned char stype, int n) {
	h->msg_type = htons(type << 8 | stype);//确定消息是否是服务器转发
	h->msg_len = htons(n);

}

/*初始化登录的结构
先清空，把id写入，把passwd写入
*/
int init_login_struct(LOGIN_MSG* i, MYSELF* m) {
	memset(i, 0, sizeof(*i));
	i->lg_id = htonl(m->w_id);//host to net
	strcpy(i->lg_pass, m->w_pass, MAX_USERPASSWD_LEN-1);
	return sizeof(LOGIN_MSG);
}

/*对登录消息的封装
char* buf 消息的缓冲区
 int* n 详细的长度
 unsigned char stype消息的子类型
*/
int enc_login_msg(char* buf, int* n, unsigned char stype) {

	/*
	把buf转换为MSG_HDR*类型，再取msg_data（消息长度的内容只是一个占位符），整个内容再转换为LOGIN_MSG*
	*/
	*n = init_login_struct((LOGIN_MSG*)(((MSG_HDR*)buf)->msg_data), &myself);
	init_msg_hdr((MSG_HDR *)buf,MSG_LOGIN,stype,sizeof(LOGIN_MSG));

	*n += sizeof(MSG_HDR);
	return *n;
}

/*
服务器通过handle_client_msg()获取消息的类型
服务器通过dec_login_msg()对登陆的消息进行解析
*/
int dec_login_msg(char* buf, int n, LOGIN_MSG** l) {
	if (n != sizeof(LOGIN_MSG)) {
		printf("serv:login msg len fail to check!\n");
		return -1;
	}
	printf("serv:recv login msg,len=%d\n",n);
	//解析消息
	*l = (LOGIN_MSG*)(buf + sizeof(MSG_HDR));
	(*l)->lg_id = ntohl((*l)->lg_id);
	printf("serv:recv login msg,len=%d,id=%d",n,(*l)->lg_id);
	return 0;
}

/*
初始化聊天的结构体
*/
int init_chat_struct(CHAT_MSG* c, MYSELF* m, char* msg, int rid) {
	memset(c, 0, sizeof(*c));
	c->ch_sid = htonl(m->w_id);//把myself中的id通过大小段转换，赋值给chatmsg的sendid
	c->ch_rid = htonl(rid);

	/*加密的消息*/
	msg_enchiper(msg, CIPHER_KEY);
	strcpy(c->ch_msg, msg);//把加密后的数据放到chat中的msg部分
	return sizeof(*c) + strlen(msg)+1;
}

/*
对消息进行封装
*/
int enc_chat_msg(char* buf,int *n , unsigned char stype, int rid, char* msg) {
	*n = init_chat_struct((CHAT_MSG*)(((MSG_HDR*)buf)->msg_data), &myself, msg, rid);
	init_msg_hdr((MSG_HDR*)buf, MSG_CHAT, stype, *n);//对msghead的初始化
	*n += sizeof(MSG_HDR);
	return *n;
}

/*
初始化登出的结构体
*/
int init_logout_struct(LOGOUT_MSG* c, MYSELF* m) {
	memset(c, 0, sizeof(*c));
	c->lg_id = htonl(m->w_id);//把myself中的id通过大小段转换，赋值给chatmsg的sendid
	strncpy(c->lg_pass, m->w_pass, MAX_USERPASSWD_LEN - 1);
	return sizeof(LOGOUT_MSG);
}
/*
封装等处的msg
*/
int enc_logout_msg(char* buf,int *n ,unsigned char stype) {
	init_login_struct((LOGOUT_MSG*)(((MSG_HDR*)buf)->msg_data), &myself);
	init_msg_hdr((MSG_HDR*)buf, MSG_LOGOUT, stype, sizeof(LOGOUT_MSG));
	*n = sizeof(LOGOUT_MSG) + sizeof(MSG_HDR);
	return *n;
}
/*
好友管理
*/
int init_fmgt_struct(FRND_OP* f, MYSELF* m, unsigned char stype, int* id, int cnt) {
	/*
	#define F_LREG 0x01//好友状态
	#define F_ADD 0x02//增加
	#define F_DEL 0x03//删除
	#define F_ALET 0x04//展示所有好友
	#define F_STAT 0x05//状态
	*/
	int* p = f->f_fid;
	f->f_id = htonl(m->w_id);
	for (int i = 0; i < cnt; i++, p++,id++) {
		*p = htonl(*id);
	}
	return sizeof(FRND_OP) + sizeof(int) * cnt;
}
/*
封装
*/
int enc_fmgt_msg(char* buf, int *n, unsigned char stype, int* id, int cnt) {
	*n = init_fmgt_struct((FRND_OP*)((MSG_HDR*)buf)->msg_data, &myself, stype, id, cnt);
	init_msg_hdr((MSG_HDR*)buf, MSG_FRNDMGT, stype, *n);
	*n += sizeof(MSG_HDR);
	return *n;
}
