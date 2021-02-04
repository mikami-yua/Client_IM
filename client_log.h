#pragma once
#ifndef CLIENT_LOG_H
#define CLIENT_LOG_H
 
/*
聊天记录的结构
	发送者
	接收者
	发送时间
	内容

文本文件保存

功能存储，导入导出
*/

#define FILED_SEPS "/" //消息分隔符
#define MSG_FIELDS 4 //每行的字段个数
#define CLIENT_LOG_FILE "msg_backup.txt"

//备份单个消息
void client_log_save(char* msg, int from, int to);
//导出消息备份
void client_log_export(FILE* sfp, FILE* dfp, int from, int to);



#endif // !CLIENT_LOG_H

