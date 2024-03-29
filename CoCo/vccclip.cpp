#include "../clip/clip.h"
#include "defines.h"
#include "keyboard.h"
#include "tcc1014graphicsAGAR.h"
#include "xdebug.h"

using namespace clip;

extern "C"
{
#ifdef COPY_PASTE
void HandleCopy(void);
void HandlePaste(void);

extern void DoKeyBoardEvent(unsigned short, unsigned short, unsigned short, unsigned short);
static unsigned char PasteBuf[82*24+1];		// 80 +2 for a CR/LF per line
static unsigned int PBIndex, PBLen;
static char KeyDown = 0;
static char CharDelay = 0;
static char FirstDelay = 0;
#endif
}

#ifdef COPY_PASTE
bool ClipboardSet(char *text, size_t len)
{
	lock l;

	if (!l.locked() || !text || !len)
		return false;
	l.clear();
	return l.set_data(text_format(), text, len);
}

size_t ClipboardGet(char *ret_text, size_t ret_size)
{
	size_t len;
	lock l;

	if (!l.locked() || !ret_text || !ret_size)
		return 0;
	len = l.get_data_length(text_format());
	if (!len)
		return 0;
	if (len > ret_size)
		len = ret_size;
	if (!(l.get_data(text_format(), ret_text, ret_size)))
		return 0;
	ret_text[len - 1] = 0;
	return len - 1;	// -1 because of the terminating 0
}

void HandleCopy(void)
{
	if (SBIndex != 0)
	{
		SelectBuf[SBIndex] = 0;
		// XTRACE("%s\n", SelectBuf);
		if (Clipped == 0)
		{
			ClipboardSet((char *)SelectBuf, SBIndex);
			Clipped = 1;
		}
		SBIndex = 0;
	}
}

void HandlePaste(void)
{
	if (!Pasting)
		return;

	// Need a delay between each character
	if (++CharDelay < 2)
		return;

	// XTRACE("Pasting: PBIndex %i, PBLen %i\n", PBIndex, PBLen);
	CharDelay = 0;
	if (PBIndex == PBLen)
	{
		size_t Len = ClipboardGet((char *)PasteBuf, sizeof(PasteBuf));

		if (Len > sizeof(PasteBuf) - 1)
		{
			Len = sizeof(PasteBuf) - 1;
			PasteBuf[sizeof(PasteBuf) - 1] = 0;
		}
		PBIndex = 0;
		PBLen = Len;
		if (PBLen == 0)
			Pasting = 0;
		// XTRACE("ClipboardGet: PBLen %i\n", PBLen);
		FirstDelay = 0;
		return;
	}

	// Need an additional delay before pasting the first character for some reason
	if (FirstDelay < 12)
	{
		FirstDelay++;
		return;
	}

	if (PBIndex < PBLen)
	{
		unsigned char Key = PasteBuf[PBIndex];

		switch (Key)
		{
		case AG_KEY_A - 0x20 ... AG_KEY_Z - 0x20:
			Key += 0x20;
#ifdef DARWIN
		case AG_KEY_EXCLAIM ... AG_KEY_AMPERSAND:
		case AG_KEY_LEFTPAREN ... AG_KEY_PLUS:
		case AG_KEY_COLON:
		case AG_KEY_LESS:
		case AG_KEY_GREATER ... AG_KEY_AT:
		case AG_KEY_CARET ... AG_KEY_UNDERSCORE:
		case 0x7b ... 0x7e: // LEFTBRACE ... TILDE
#endif
				    // fake a shift key
			DoKeyBoardEvent(0, AG_KEY_LSHIFT, (KeyDown ? kEventKeyUp : kEventKeyDown), 0);
			// XTRACE("faked a SHIFT key\n");
			break;
		default:
			break;
		}
		if (!KeyDown)
		{
			DoKeyBoardEvent(Key, Key, kEventKeyDown, 0);
			KeyDown = 1;
		}
		else
		{
			DoKeyBoardEvent(Key, Key, kEventKeyUp, 0);
			KeyDown = 0;
			// Don't need a delay after key-up
			CharDelay++;
			if (++PBIndex == PBLen)
			{
				PBIndex = PBLen = 0;
				Pasting = 0;
			}
		}
	}
}
#endif
