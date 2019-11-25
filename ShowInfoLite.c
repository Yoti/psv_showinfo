/*
	ShowInfo Lite by Yoti
*/

#include <taihen.h>
#include <vitasdk.h>
#include <stdio.h>
#include "blit.h"

int sceAppMgrAppParamGetInt(int pid, int param, int *value);

int APP_VER = 0;
char STITLE[53] = {0};
char TITLE_ID[10] = {0};
char SHOW_STR[255] = {0};
char FINAL_STR[255] = {0};

static SceUID g_hooks[1];

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf0(const SceDisplayFrameBuf *pParam, int sync) {
	blit_set_frame_buf(pParam);
	blit_set_color(0x00FFFFFF, 0x00000000); // ABGR
	blit_string(0, 16 * 33, FINAL_STR);

	return TAI_CONTINUE(int, ref_hook0, pParam, sync);
}

void _start() __attribute__ ((weak, alias ("module_start")));

int module_start(SceSize argc, const void *args) {
	int i;
	g_hooks[0] = taiHookFunctionImport(&ref_hook0,
		TAI_MAIN_MODULE,
		TAI_ANY_LIBRARY,
		0x7A410B64, // sceDisplaySetFrameBuf
		sceDisplaySetFrameBuf0);

	if (g_hooks[0] >= 0) {
		sceAppMgrAppParamGetString(0, 10, STITLE, sizeof(STITLE));
		sceAppMgrAppParamGetString(0, 12, TITLE_ID, sizeof(TITLE_ID));
		sceAppMgrAppParamGetInt(0, 3, &APP_VER);
		sprintf(SHOW_STR, "[%s] %s (v%i", TITLE_ID, STITLE, APP_VER);
		for (i = 0; i < strlen(SHOW_STR) - 2; i++)
			FINAL_STR[i] = SHOW_STR[i];
		sprintf(FINAL_STR, "%s.%c%c)",
			FINAL_STR,
			SHOW_STR[strlen(SHOW_STR) - 2],
			SHOW_STR[strlen(SHOW_STR) - 1]
		);
	}

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);

	return SCE_KERNEL_STOP_SUCCESS;
}
