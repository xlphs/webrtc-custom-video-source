#include "stdafx.h"

#include "utils.h"

#include <stdio.h>

std::string int2str(int i) {
	char buffer[11] = { 0 };
	sprintf(buffer, "%d", i);  // NOLINT
	return buffer;
}

std::string size_t2str(size_t i) {
	char buffer[32] = { 0 };
#ifdef WIN32
	// %zu isn't supported on Windows.
	sprintf(buffer, "%Iu", i);  // NOLINT
#else
	sprintf(buffer, "%zu", i);  // NOLINT
#endif
	return buffer;
}
