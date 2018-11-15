
#include "cyusb.h"
#include <stdio.h>

int main(void){
	cyusb_ctx_t cyusb_ctx;

	cyusb_init(&cyusb_ctx);
	cyusb_find(&cyusb_ctx);
	cyusb_exit(&cyusb_ctx);

	return 0;
}
