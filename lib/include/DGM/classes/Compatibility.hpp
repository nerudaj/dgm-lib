#pragma once

#define ANDROID

#ifdef ANDROID
	#define NODISCARD_RESULT
#else
	#define NODISCARD_RESULT [[nodiscard]]
#endif