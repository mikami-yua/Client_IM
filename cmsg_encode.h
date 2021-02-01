#ifndef CMSG_ENCODE_H
#define CMSG_ENCODE_H

#include"im_client.h"

int init_reg_struct(REG_MSG* r, MYSELF* m);
void init_msg_hdr(MSG_HDR* h, unsigned char type, unsigned char stype, int n);
int init_login_struct(LOGIN_MSG* i, MYSELF* m);
int enc_login_msg(char *buf,int *n,unsigned char stype);





#endif // !CMSG_ENCODE_H
