#ifndef  CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

int dec_reg_resp(REG_RESP *r);
int handle_login_resp(LOGIN_RESP* l);
int handle_fmgt_resp(unsigned short stype,FRND_RESP *fr,FRND_STAT *fs ,short count);
int handle_chat_msg_resp(CHAT_MSG *c , CHAT_RESP *cr);//对聊天消息或聊天响应的处理
int handle_chat_resp(CHAT_RESP *c);
int handle_chat_msg(CHAT_MSG *c);
#endif // ! CLIENT_HANDLE_H

