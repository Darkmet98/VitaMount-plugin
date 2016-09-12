/*
	Vitamount Plugin
	Copyright (C) 2016, Darkmet98 and Tomtomdu80

	This code is originaly written for Tomtomdu80
	Forked and adapted by Darkmet98
	¡This plugin needs unsafe homebrew!
	¡Use this plugin for only dev.!
*/

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/power.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/processmgr.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "blit.h"

static uint32_t current_buttons = 0, pressed_buttons = 0;

int holdButtons(SceCtrlData *pad, uint32_t buttons, uint64_t time) {
	if ((pad->buttons & buttons) == buttons) {
		uint64_t time_start = sceKernelGetProcessTimeWide();

		while ((pad->buttons & buttons) == buttons) {
			sceKernelDelayThread(10 * 1000);
			sceCtrlPeekBufferPositive(0, pad, 1);

			pressed_buttons = pad->buttons & ~current_buttons;
			current_buttons = pad->buttons;

			if ((sceKernelGetProcessTimeWide() - time_start) >= time) {
				return 1;
			}
		}
	}

	return 0;
}

int blit_thread(SceSize args, void *argp) {
	sceKernelDelayThread(5 * 1000 * 1000);

	while (1) {
		SceCtrlData pad;
		memset(&pad, 0, sizeof(SceCtrlData));
		sceCtrlPeekBufferPositive(0, &pad, 1);

		pressed_buttons = pad.buttons & ~current_buttons;
		current_buttons = pad.buttons;

		if (holdButtons(&pad, SCE_CTRL_START, 1 * 1000 * 1000)) {
		void *buf = malloc(0x100);
			{
				int i;
				sceKernelDelayThread(0 * 1000 * 1000);
				vshIoUmount(0x200, 0, 0, 0); // Dismount only os0 partition
				vshIoUmount(0x300, 0, 0, 0); // Dismount only vs0 partition
				vshIoUmount(0x400, 0, 0, 0); // Dismount only vd0 partition
				vshIoUmount(0x500, 0, 0, 0); // Dismount only tm0 partition
				vshIoUmount(0x700, 0, 0, 0); // Dismount only ud0 partition
				vshIoUmount(0x800, 0, 0, 0); // Dismount only sa0 partition
				vshIoUmount(0x900, 0, 0, 0); // Dismount only gro0 partition
				vshIoUmount(0xC00, 0, 0, 0); // Dismount only pd0 partition
			
				sceKernelDelayThread(0 * 1000 * 1000);
				_vshIoMount(0x200, 0, 2, buf); // Mount only os0 partition in RW
				_vshIoMount(0x300, 0, 2, buf); // Mount only vs0 partition in RW
				_vshIoMount(0x400, 0, 2, buf); // Mount only vd0 partition in RW
				_vshIoMount(0x500, 0, 2, buf); // Mount only tm0 partition in RW
				_vshIoMount(0x700, 0, 2, buf); // Mount only ud0 partition in RW
				_vshIoMount(0x800, 0, 2, buf); // Mount only sa0 partition in RW
				_vshIoMount(0x900, 0, 2, buf); // Mount only gro0 partition in RW
				_vshIoMount(0xC00, 0, 2, buf); // Mount only pd0 partition in RW
			}
		}

		sceDisplayWaitVblankStart();
	}

	return 0;
}

int _start(SceSize args, void *argp) {
	SceUID thid = sceKernelCreateThread("blit_thread", blit_thread, 0x40, 0x40000, 0, 0, NULL);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, NULL);

	return 0;
}