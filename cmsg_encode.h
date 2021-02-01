#ifndef CMSG_ENCODE_H
#define CMSG_ENCODE_H

#include"im_client.h"

int init_reg_struct(REG_MSG* r, MYSELF* m);
void init_msg_hdr(MSG_HDR* h, unsigned char type, unsigned char stype, int n);
int init_login_struct(LOGIN_MSG* i, MYSELF* m);
int enc_login_msg(char *buf,int *n,unsigned char stype);
int init_chat_struct(CHAT_MSG *c,MYSELF *m,char *msg,int rid);
int enc_chat_msg(char* buf, int* n, unsigned char stype, int rid, char* msg);
int init_logout_struct(LOGOUT_MSG* c, MYSELF* m);
int enc_logout_msg(char* buf, int* n, unsigned char stype);
int init_fmgt_struct(FRND_OP* f, MYSELF* m, unsigned char stype, int* id, int cnt);
int enc_fmgt_msg(char* buf, int *n, unsigned char stype, int* id, int cnt);


#endif // !CMSG_ENCODE_H
