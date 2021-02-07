#include"im_client.h"
//备份单个消息
void client_log_save(char* msg, int from, int to) {
	FILE* fp;
	char buf[MAX_LINE_LEN];
	time_t ticks;//time_t 这种类型就是用来存储从1970年到现在经过了多少秒
	ticks = time(NULL);
	int n = 0;//记录写了多少字符

	fp = fopen(CLIENT_LOG_FILE, "a+");
	if (fp == NULL) {
		perror("client-log:open error\n");
		return;
	}
	//写到缓冲区
	n += _snprintf(buf + n, sizeof(buf) - n, "%d/", from);
	n += _snprintf(buf + n, sizeof(buf) - n, "%d/", to);
	n += _snprintf(buf + n, sizeof(buf) - n, "%.24s/", ctime(&ticks));
	n += _snprintf(buf + n, sizeof(buf) - n, "%s\n", msg);

	buf[n] = '\0';

	fputs(buf, fp);
	fclose(fp);
}
//导出消息备份
void client_log_export(FILE* sfp, FILE* dfp, int from, int to) {
	char buf[MAX_LINE_LEN];
	char* token = NULL, * next_token = NULL, seps[] = "/";
	//strtoks_s()将一个字符串，按照seps[] = "/";分割，分成若干个东西
	char* p[MSG_FIELDS] = { NULL };//用来存储4个字段

	int msg_src, msg_dst, i;

	memset(buf, 0, sizeof(buf));
	while (fgets(buf, MAX_LINE_LEN, sfp) != NULL)//把源文件中的内容读到buf中
	{
		token = strtok_s(buf, seps, &next_token);//token得到第一部分
		for (int i = 0; i < MSG_FIELDS; i++)//for循环把一行的记录读完
		{
			p[i] = token;
			token = strtok_s(NULL, seps, &next_token);
		}
		if (from) {
			msg_src = atoi(p[0]);//atoi()作用“10”=》10
		}
		if (to) {
			msg_dst = atoi(p[1]);
		}
		if ((!from && !to) //from和to均为0 需要保存全部的消息
			|| (from && !to && msg_src == from)//from不为0 to为0 且msg_src==from 过滤来自某个用户发送的消息
			|| (!from && to && msg_dst == to)//from为0 to不为0 且msg_dst==to 过滤发送至某个用户的消息
			|| (from && to && msg_dst == to && msg_src == from)) //from与to都不为0 &&msg_dst==to &&msg_src==from 过滤发送者是from，且结束者是to的消息
		{
			fprintf(dfp, "client log:%sTo:%s, Time:%s, msg:%s", p[0], p[1], p[2], p[3]);
			memset(buf, 0, sizeof(buf));
		}
	}
}