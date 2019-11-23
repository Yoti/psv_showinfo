#include <taihen.h>
#include <vitasdk.h>
#include <stdio.h>
#include "blit.h"

/*
	blit_string

	X: 960px/16ppc=60chars
	Y: 544px/16ppc=34chars
	*: PPC = Pixels Per Char
*/
/*
	sceAppMgrAppParamGetString

	_0 ?
	_1 ?
	_2 ?
	_3 ?
	_4 ?
	_5 ?
	_6 CONTENT_ID
	_7 ?
	_8 CATEGORY
	_9 TITLE
	10 STITLE
	11 ?
	12 TITLE_ID
*/

static SceUID g_hooks[1];

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf0(const SceDisplayFrameBuf *pParam, int sync) {
	int i, y = 0, x, len;
	char tmp[255] = {0};
	char buf[255] = {0};

	blit_set_frame_buf(pParam);

	for (i = 0; i < 65; i++) {
		if (y % 2 == 0)
			blit_set_color(0x00FFFFFF, 0x1F0000FF); // ABGR
		else
			blit_set_color(0x00FFFFFF, 0x1FFF0000); // ABGR

		memset(buf, 0, sizeof(buf));
		sceAppMgrAppParamGetString(0, i, buf, sizeof(buf));
		len = strlen(buf);

		if (len > 0) {
			// выводим номер строки и разделитель
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%02i|", i);
			blit_string(16 * 1, 16 + y * 16, tmp);

			// вписываем основной текст
			blit_string(16 * 4, 16 + y * 16, buf);

			// дополняем строку точками
			for (x = 4 + len; x < 50; x++)
				blit_string(16 * x, 16 + y * 16, ".");

			// дописываем разделитель и размер
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "|%08i", len);
			blit_string(16 * 50, 16 + y * 16, tmp);

			y++;
		}
	}

	return TAI_CONTINUE(int, ref_hook0, pParam, sync);
}

void _start() __attribute__ ((weak, alias ("module_start")));

int module_start(SceSize argc, const void *args) {
	g_hooks[0] = taiHookFunctionImport(&ref_hook0,
		TAI_MAIN_MODULE,
		TAI_ANY_LIBRARY,
		0x7A410B64, // sceDisplaySetFrameBuf
		sceDisplaySetFrameBuf0);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);

	return SCE_KERNEL_STOP_SUCCESS;
}
