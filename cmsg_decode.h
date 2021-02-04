#pragma once
#ifndef CMSG_DECODE_H
#define CMSG_DECODE_H

int dec_server_resp(char *buf,int n);//解析来自服务器的回复
int dec_reg_resp(char *buf,int n,REG_RESP **lr);
int dec_login_resp(char *buf ,int n,LOGIN_RESP **l);
int dec_fmgt_resp(char *buf,int n,FRND_RESP **fr,FRND_STAT **fs,short *count);
int dec_chat_msg_resp(char *buf, int n, CHAT_RESP **cr,CHAT_MSG **cm);

#endif // !CMSG_DECODE_H

