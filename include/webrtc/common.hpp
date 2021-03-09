#ifndef WEBRTC_COMMON_H_
#define WEBRTC_COMMON_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// replacements for WebRTC's various assert-like macros
#define RTC_COMPILE_ASSERT(expr) static_assert(expr, #expr)
#define RTC_DCHECK(expr) assert(expr)
#define RTC_DCHECK_GT(a, b) assert((a) > (b))
#define RTC_DCHECK_LT(a, b) assert((a) < (b))
#define RTC_DCHECK_LE(a, b) assert((a) <= (b))

// from webrtc/base/sanitizer.h
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define RTC_NO_SANITIZE(what) __attribute__((no_sanitize(what)))
#endif
#endif
#ifndef RTC_NO_SANITIZE
#define RTC_NO_SANITIZE(what)
#endif

#define arraysize(a) (sizeof(a) / sizeof *(a))


#endif  // WEBRTC_COMMON_H_