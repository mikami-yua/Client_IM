#ifndef IM_CLIENT_H
#define IM_CLIENT_H
#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#include<time.h>
#include"pthread.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"pthreadVC2.lib")

#include "client_user.h"
#include "cmsg_encode.h"
#include"list.h"
#include"msg_type.h"
#include"msg_cipher.h"
#include"client_config.h"
#include"client_handle.h"
#include"client_log.h"
#include"cmsg_decode.h"
#include"cuser_cmd.h"



#endif // !IM_CLIENT_H
