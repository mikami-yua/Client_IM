#include"im_client.h"

int dec_reg_resp(REG_RESP* r) {
	myself.w_id = r->re_id;
	printf("client:recv reg msg from server,uid=%d\n", r->re_id);

	if (r->re_id == -1) {
		fprintf(stderr, "%s register falied,reason:%s", myself.w_name, r->re_reason);
	}
	return ;
}