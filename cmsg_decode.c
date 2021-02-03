#include "im_client.h"

/*
对收到的消息统一处理
处理流程：获取消息的类型，子类型，消息的长度
判断消息的长度是否合法

switch语句对不同的消息进行不同的处理
decode_reg_resp();

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
		if (dec_reg_resp()) {
			handle_reg_resp();
		}
		break;
	default:
		break;
	}

}