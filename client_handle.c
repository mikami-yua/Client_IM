#include"im_client.h"

int handle_reg_resp(REG_RESP* r) {
	myself.w_id = r->re_id;
	printf("client:recv reg msg from server,uid=%d\n", r->re_id);

	if (r->re_id == -1) {
		fprintf(stderr, "%s register falied,reason:%s", myself.w_name, r->re_reason);
	}
	return;
}

int handle_login_resp(LOGIN_RESP* l) {
	printf("client:recv login resp from server\n");

	if (l->lg_stat == LOGIN_OK) {
		myself.w_cstat = LOGIN_CSTAT;
		memeset(myself.w_name, 0, MAXNAME_LEN);
		strncpy(myself.w_name, l->lg_name, MAXNAME_LEN - 1);
		return 0;
	}
	fprintf(stderr, "user %s login failed reason:%s\n", myself.w_name, l->lg_reason);
	return -1;
}

int handle_fmgt_resp(unsigned short stype, FRND_RESP* fr, FRND_STAT* fs, short count) {
	printf("client:recv fmgt resp ,stype=%d,count=%d\n", stype, count);
	/*
	#define F_LREG 0x01//好友状态
	#define F_ADD 0x02//增加
	#define F_DEL 0x03//删除
	#define F_ALET 0x04//展示所有好友
	#define F_STAT 0x05//状态
	*/
	switch (stype)
	{
	case F_LREG:
	case F_ADD:
		user_list_add(fs, count);
		break;
	case F_DEL:
		user_list_del(fs, count);
		break;
	case F_ALET:
		user_list_output(fs, count);
		break;
	case F_STAT:
		user_list_update(fs, count);
		break;
	default:
		break;
	}
}

int handle_chat_msg_resp(CHAT_MSG* c, CHAT_RESP* cr) {
	/*
	其他用户发送给自己的
	自己发送给别人，服务器的反馈发送是否成功
	*/
	if (cr) {
		return handle_chat_resp(cr);
	}
	return handle_chat_msg(c);
}

int handle_chat_resp(CHAT_RESP* c) {
	printf("client:recv chat msg from server\n");
	if (c->c_stat == SND_OK) {
		printf("client:chat msg send ok\n");
		return 0;
	}
	fprintf(stderr, "chat msg send err,reason:%s", c->c_reason);
	return -1;
}

int handle_chat_msg(CHAT_MSG* c) {
	msg_dechiper(c->ch_msg, CIPHER_KEY);//解密到原来物理的位置
	printf("client:recv chat msg from client %d:%s\n",c->ch_sid,c->ch_msg);
	client_log_save(c->ch_msg,c->ch_sid,c->ch_rid);
	return 0;
}