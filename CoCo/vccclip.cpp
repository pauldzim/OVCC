#include "../clip/clip.h"

extern "C" {

char *vccclip(void)
{
	clip::set_text("Hello World");
	return NULL;
}

}
