#include <windows.h>
#include "shttpd.h"

/*
 * To compile, do:
 * rc dialog.rc
 * cl example_win32_dialog_app.c dialog.res user32.lib shttpd.lib /MD
 */

#define	LISTENING_PORT	8080
#define	ID_TIMER	22

static BOOL CALLBACK
DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static struct shttpd_ctx *ctx;

	switch (msg) {

	case WM_CLOSE:
		KillTimer(hDlg, ID_TIMER);
		DestroyWindow(hDlg);
		break;

	case WM_TIMER:
		shttpd_poll(ctx, 0);
		break;

	case WM_INITDIALOG:
		ctx = shttpd_init(NULL, NULL);
		shttpd_listen(ctx, LISTENING_PORT, 0);
		SetTimer(hDlg, ID_TIMER, 250, NULL);
		break;
	default:
		break;
	}

	return FALSE;
}

int WINAPI
WinMain(HINSTANCE h, HINSTANCE prev, char *cmdline, int show)
{
	return DialogBox(h, MAKEINTRESOURCE(100), NULL, DlgProc);
}
