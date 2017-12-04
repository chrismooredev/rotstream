#ifndef _H_VERSION
#define _H_VERSION

#ifdef __linux
#define BUILD_LINUX
#elif _WIN32
#define BUILD_WIN32
#endif

#ifdef USE_CMAKE_VERSION
#define VERSION_MAJOR ${VERSION_MAJOR}
#define VERSION_MINOR ${VERSION_MINOR}
#endif

#endif