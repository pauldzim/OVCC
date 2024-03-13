#include "../clip/clip.h"

using namespace clip;

extern "C"
{
bool ClipboardSet(char *text, size_t len);
size_t ClipboardGet(char *ret_text, size_t ret_size);
}

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
