/*
	ShowInfo by Yoti
*/

#include <taihen.h>
#include <vitasdk.h>
#include <stdio.h>
#include "blit.h"

/*
	sceAppMgrAppParamGetString

	Params:
	__6 0x06 CONTENT_ID
	__7 0x07 NP_COMMUNICATION_ID
	__8 0x08 CATEGORY
	__9 0x09 TITLE
	_10 0x0a STITLE
	_12 0x0c TITLE_ID
	100 0x64 CONTENT_ID (same as 6)
	101 0x65 TITLE (same as 9)
*/
/*
	sceAppMgrAppParamGetInt

	Params:
	_1 0x01 ???
	_2 0x02 ???
	_3 0x03 APP_VER
	_4 0x04 ???
	_5 0x05 ???
	11 0x0b SAVEDATA_MAX_SIZE
	13 0x0d ???
	14 0x0e ???
	16 0x10 ???

	Errors:
	80800009 PARAM_DOES_NOT_EXIST?
	80801042 WRONG_PARAM_TYPE?
*/

int sceAppMgrAppParamGetInt(int pid, int param, int *value);

int show_info = 0;
static SceUID g_hooks[1];

void asciify(char from[], char to[]) {
	for (int i = 0; i < strlen(from); i++) {
		if (from[i] <= 0x7F) {
			if (from[i] == 0x0A) // new line
				sprintf(to, "%s%c", to, 0x20);
			else if (from[i] >= 0x20)
				sprintf(to, "%s%c", to, from[i]);
		} else if (from[i] <= 0xDF) {
			if ((from[i] != 0xC2) && (from[i+1] != 0xAE)) // copyright
				sprintf(to, "%s%c", to, '#');
			i++;
		} else if (from[i] <= 0xEF) {
			if ((from[i] != 0xE2) && (from[i+1] != 0x84) && (from[i+2] != 0xA2)) // trademark
				sprintf(to, "%s%c", to, '#');
			i++;
			i++;
		} else if (from[i] <= 0xFF) {
			sprintf(to, "%s%c", to, '#');
			i++;
			i++;
			i++;
		}
	}
}

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf0(const SceDisplayFrameBuf *pParam, int sync) {
	if (show_info) {

	int i, y = 0, k = 32;
	int len, ret, num;
	char str[255] = {0};
	char buf[255] = {0};

	blit_set_frame_buf(pParam);
	blit_set_color(0x00FFFFFF, 0x000000FF); // ABGR
	blit_string(0, k + 0, "ShowInfo");
	blit_string(16 * 56, k + 0, "v0.6");

	for (i = 0; i < 128; i++) {
		if (y % 2 == 0)
			blit_set_color(0x00FFFFFF, 0x00FF0000); // ABGR
		else
			blit_set_color(0x00FFFFFF, 0x000000FF); // ABGR

		ret = 0;
		memset(str, 0, sizeof(str));
		ret = sceAppMgrAppParamGetString(0, i, str, sizeof(str));
		len = strlen(str);

		if (ret == 0) { // string
			memset(buf, 0, sizeof(buf));
			asciify(str, buf);
			memset(str, 0, sizeof(str));
			sprintf(str, "0x%02x|%s|%i", i, buf, len);
			blit_string(16 * 1, k + 16 + y * 16, str);
			y++;
		}

		ret = 0;
		num = 0;
		ret = sceAppMgrAppParamGetInt(0, i, &num);

		if (ret == 0) { // number
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "0x%02x|0x%08X/%i", i, num, num);
			blit_string(16 * 1, k + 16 + y * 16, buf);
			y++;
		}
	}

	blit_string(0, k + 16 * (y + 1), "USER");
	blit_string(16 * 56, k + 16 * (y + 1), "Yoti");

	} return TAI_CONTINUE(int, ref_hook0, pParam, sync);
}

void _start() __attribute__ ((weak, alias ("module_start")));

int module_start(SceSize argc, const void *args) {
	g_hooks[0] = taiHookFunctionImport(&ref_hook0,
		TAI_MAIN_MODULE,
		TAI_ANY_LIBRARY,
		0x7A410B64, // sceDisplaySetFrameBuf
		sceDisplaySetFrameBuf0);

	if (g_hooks[0] >= 0) show_info = 1;

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);

	return SCE_KERNEL_STOP_SUCCESS;
}
