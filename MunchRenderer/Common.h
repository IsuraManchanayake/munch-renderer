#pragma once

#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <chrono>

#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b)                                                              \
  do {                                                                         \
    if (!(b)) {                                                                \
      OutputDebugStringA("Assert: " #b "\n");                                  \
    }                                                                          \
  } while (0)
#else
#define Assert(b)
#endif // DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

template <class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease) {
  if (*ppInterfaceToRelease != NULL) {
    (*ppInterfaceToRelease)->Release();

    (*ppInterfaceToRelease) = NULL;
  }
}

template <typename Resolution> struct Timer {
  static auto current() {
    return std::chrono::duration_cast<Resolution>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }
};