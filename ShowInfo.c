#include <taihen.h>
#include <vitasdk.h>
#include "blit.h"

/*
	X: 960px/16ppc=60chars
	Y: 544px/16ppc=34chars
	*: PPC = Pixels Per Char
*/

static int TITLE_ID_X = 60 * 16 - 9 * 16;
static int TITLE_ID_Y = 34 * 16 - 1 * 16;
static SceUID g_hooks[1];

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf0(const SceDisplayFrameBuf *pParam, int sync) {
	char TITLE_ID_T[10] = {0};
	sceAppMgrAppParamGetString(0, 12, TITLE_ID_T, 10);

	blit_set_frame_buf(pParam);
	blit_set_color(0x00FFFFFF, 0x00FF0000); // ABGR
	blit_string(TITLE_ID_X, TITLE_ID_Y, TITLE_ID_T);

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
