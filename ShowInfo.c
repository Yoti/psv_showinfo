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

	_6 CONTENT_ID
	_8 CATEGORY
	_9 TITLE
	10 STITLE
	12 TITLE_ID
*/
/*
	sceAppMgrAppParamGetInt

	_3 ?
	11 SAVEDATA_MAX_SIZE?
	13 ?
	16 ?
*/

int sceAppMgrAppParamGetInt(int pid, int param, int *value); // fake proto

int show_info = 1;
static SceUID g_hooks[1];

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf0(const SceDisplayFrameBuf *pParam, int sync) {
	if (show_info) {

	int i, y = 0, x, len, num;
	char tmp[15] = {0};
	char buf[255] = {0};

	blit_set_frame_buf(pParam);
	blit_set_color(0x00FFFFFF, 0x1F0000FF); // ABGR
	blit_string(0, 0, "ShowInfo");
	blit_string(16 * 56, 0, "v0.5");

	for (i = 0; i < 256; i++) {
		if (y % 2 == 0)
			blit_set_color(0x00FFFFFF, 0x1FFF0000); // ABGR
		else
			blit_set_color(0x00FFFFFF, 0x1F0000FF); // ABGR

		memset(buf, 0, sizeof(buf));
		sceAppMgrAppParamGetString(0, i, buf, sizeof(buf));
		len = strlen(buf);

		if (len > 0) {
/*
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "%02i|%s (%i)", i, buf, len);
			blit_string(16 * 1, 16 + y * 16, buf);
*/

			// выводим номер строки и разделитель
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%02i|", i);
			blit_string(16 * 1, 16 + y * 16, tmp);

			// вписываем основной текст
			blit_string(16 * 4, 16 + y * 16, buf);

			// дополняем строку точками
			for (x = 4 + len; x < 54; x++)
				blit_string(16 * x, 16 + y * 16, ".");

			// дописываем разделитель и размер
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "|%04i", len);
			blit_string(16 * 54, 16 + y * 16, tmp);

			y++;
		} else {
			num = 0;
			sceAppMgrAppParamGetInt(0, i, &num);

			if (num != 0) {
				// выводим номер строки и разделитель
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "%02i|", i);
				blit_string(16 * 1, 16 + y * 16, tmp);

				// вписываем основной текст
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "0x%08x", num);
				blit_string(16 * 4, 16 + y * 16, tmp);
				
				y++;
			}
		}
	}

	blit_string(0, 16 * (y + 1), "USER");
	blit_string(16 * 56, 16 * (y + 1), "Yoti");

	} return TAI_CONTINUE(int, ref_hook0, pParam, sync);
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
