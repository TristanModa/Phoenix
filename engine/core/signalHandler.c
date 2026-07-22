#include "signalHandler.h"

#ifdef LINUX
#error Not implemented
#elif WIN32
#include <windows.h>
#endif

#include "logger.h"

#ifdef LINUX

#elif WIN32
static LONG WINAPI win32CrashHandler(PEXCEPTION_POINTERS exceptionInfo) {
    DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
    switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            fprintf(stderr, "Error: Segmentation Fault (Access Violation)\n");
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            fprintf(stderr, "Error: Integer Division by Zero\n");
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            fprintf(stderr, "Error: Illegal Instruction\n");
            break;
        case EXCEPTION_STACK_OVERFLOW:
            fprintf(stderr, "Error: Stack Overflow\n");
            break;
        default:
            fprintf(stderr, "Unknown Exception Code: 0x%lX\n", exceptionCode);
            break;
    }

    exit(exceptionCode);
    return EXCEPTION_CONTINUE_SEARCH ;
}
#endif

void SignalHandler_init() {
#ifdef LINUX

#elif WIN32
    PVOID handler = AddVectoredExceptionHandler(1, win32CrashHandler);
    if (!handler) {
        perror("Failed to register crash handler.\n");
        exit(1);
    }
#endif
}