#include"im_client.h"
int init_reg_struct(REG_MSG* r, MYSELF* m) {

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
	strcpy(i->lg_pass, m->w_pass, MAX_USERPASSWD_LEN);
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
