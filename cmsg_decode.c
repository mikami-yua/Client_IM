#include "im_client.h"

/*
对收到的消息统一处理
处理流程：获取消息的类型，子类型，消息的长度
判断消息的长度是否合法

switch语句对不同的消息进行不同的处理

*/
int dec_server_resp(char* buf, int n) {
	unsigned char type;
	REG_RESP* r;//注册回复
	LOGIN_RESP* l;//登录回复
	FRND_RESP* fr;
	FRND_STAT* fs;
	CHAT_RESP* c;
	CHAT_MSG* cm;

	short count;//好友数量
	MSG_HDR* h = (MSG_HDR*)buf;
	short m_type, m_len;//消息类型，消息长度
	unsigned char stype;//消息子类型
	m_type = ntohs(h->msg_type);
	m_len = ntohs(h->msg_len);
	
	stype = m_type & 0xff;//确定消息的子类型

	if (n != sizeof(MSG_HDR) + m_len) {
		printf("client:resp msg len check error\n");
		return -1;
	}

	//计算消息类型
	//ps：好友消息0x03=0000 0011 添加好友 子类型：0x02=0000 0010 short类型占2byte： 0000 0011 0000 0010是msgheader
	type = (m_type >> 8) & 0xff;//m_type/2^8 获取消息的大类型

	switch (type &0x7f)//0x7f=127
	{
	case MSG_REG:
		if (dec_reg_resp(buf, m_len, &r) >= 0) {
			handle_reg_resp(r);
		}
		break;
	case MSG_LOGIN:
		if (dec_login_resp(buf, m_len, &l) >= 0) {
			handle_login_resp(l);
		}
		break;
	case MSG_FRNDMGT:
		if (dec_fmgt_resp(buf, m_len,&fr, &fs,&count) >= 0) {
			handle_fmgt_resp(stype,fr,fs,count);
		}
		break;
	case MSG_CHAT:
		if (dec_chat_msg_resp(buf, m_len, &c,&cm) >= 0) {
			handle_chat_msg_resp(c,cm);
		}
		break;
	default:
		break;
	}

}

/*
解析注册回复的消息
*/
int dec_reg_resp(char* buf, int n, REG_RESP** lr) {
	MSG_HDR* h = (MSG_HDR *)buf;//MSG_HDR中是两个short类型的，+1就转到第三个成员变量
	REG_RESP* r = (REG_RESP*)(h + 1);

	if (n > sizeof(REG_RESP)) {
		if (n > sizeof(REG_RESP) + MAX_ERR_LEN || n != sizeof(REG_RESP) + strlen(r->re_reason) + 1) {
			printf("client:reg resp invalid msg len=%d\n", n);
			return -1;
		}
		else if (n != sizeof(REG_RESP)) {
			printf("client:reg resp invalid msg len=%d\n", n);
			return -1;
		}
		// 0000 0000 1111 1111大段
		// 1111 1111 0000 0000小段
		r->re_id = ntohl(r->re_id);
		*lr = r;
		return 0;
	}
}

/*
解析登录注册回复消息
*/
int dec_login_resp(char* buf, int n, LOGIN_RESP** l) {
	MSG_HDR *h = (MSG_HDR *)buf;
	*l = (LOGIN_RESP*)(h + 1);
	if (n > sizeof(LOGIN_RESP)) {
		if (n > sizeof(LOGIN_RESP) + MAX_ERR_LEN || n != sizeof(LOGIN_RESP) + strlen((*l)->lg_reason)+1) {
			printf("client:reg resp invalid msg len=%d\n", n);
			return -1;
		}
		else if (n != sizeof(LOGIN_RESP)) {
			printf("client:reg resp invalid msg len=%d\n", n);
			return -1;
		}
	}
	(*l)->lg_stat = ntohl((*l)->lg_stat);

	
	return 0;
}

/*
好友管理的消息解析
*/
int dec_fmgt_resp(char* buf, int n, FRND_RESP** fr, FRND_STAT** fs, short *count) {
	MSG_HDR* h = (MSG_HDR*)buf;
	FRND_RESP* f = (FRND_RESP*)(h + 1);
	FRND_STAT* ps = (FRND_STAT*)(f + 1);

	*count = (n - sizeof(FRND_RESP)) / sizeof(FRND_STAT);//？？？？？讲解了个锤子

	if (n > sizeof(FRND_RESP)) {
		if (n-sizeof(FRND_RESP)%sizeof(FRND_STAT)!=0 || count !=ntohs(f->fre_ok_frnd)) {//不是操作成功的数量
			printf("client:fmgt resp invalid msg len=%d,count=%d\n", n,*count);
			return -1;
		}
		else if (n != sizeof(FRND_RESP)) {
			printf("client:fmgt resp invalid msg len=%d\n", n);
			return -1;
		}
	}
	f->fre_stat = ntohs(f->fre_stat);
	*fr = f;
	*fs = ps;

	//对各个好友操作
	for (int i = 0; i < *count; i++,ps++) {//ps指向好友数组，++指向下一个好友
		ps->fr_id = ntohl(ps->fr_id);
		ps->fr_stat = ntohs(ps->fr_stat);
	}
	return 0;


}


/*
聊天消息的解析：可能是来自服务器的发送消息回复，也可能是别的客户端发送的消息
*/
int dec_chat_msg_resp(char* buf, int n, CHAT_RESP** cr, CHAT_MSG** cm) {
	MSG_HDR* h = (MSG_HDR*)buf;
	short msg_type = ntohs(h->msg_type);
	//判断是用户信息还是服务器转发信息
	//SERVER_FORWARD_FLAG:1000 0000 0000 0000  msg_type聊天消息：0x04：0100   &的结果为全0  
	//server的转发在最高位进行|处理 得到两个1其他位全是0 具体讲解p24 25：00  实际收到的是有两个1位的消息
	//服务器回复消息不会在最高位进行或处理，得到一位是1其他是0的消息
	if (msg_type & SERVER_FORWARD_FLAG) {//这里按位与把最高位的1消掉
		printf("client:real chat msg,n=%d", n);
		*cr = NULL;
		*cm = (CHAT_MSG*)(h + 1);
	}
	else {//最后得出的结果为0
		printf("client:chat resp msg,n=%d\n", n);
		*cm = NULL;
		*cr = (CHAT_RESP*)(h + 1);
	}
	
	if (*cr) {//判断是CHAT_RESP的情况
		if (n > sizeof(CHAT_RESP)) {
			if (n > sizeof(CHAT_RESP) + MAX_ERR_LEN || n != sizeof(CHAT_RESP) + strlen((*cr)->c_reason) + 1) {
				printf("client:chat resp invalid msg len=%d\n", n);
				return -1;
			}
			else if (n != sizeof(CHAT_RESP)) {
				printf("client:chat resp invalid msg len=%d\n", n);
				return -1;
			}
		}
	}
	else {//是CHAT_MSG的情况
		if (n > MAX_MSG_SIZE || n < sizeof(CHAT_MSG)) {
			printf("client:chat msg invalid msg len=%d\n", n);
			return -1;
		}
		(*cm)->ch_sid = ntohl((*cm)->ch_sid);
		(*cm)->ch_rid = ntohl((*cm)->ch_rid);
	}

	return 0;
}