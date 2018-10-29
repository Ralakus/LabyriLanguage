#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

#define _LAB_PRINT_PREFIX   LAB_ANSI_COLOR_RESET  "[ ]: " LAB_ANSI_COLOR_RESET
#define _LAB_NOTICE_PREFIX  LAB_ANSI_COLOR_CYAN   "[-]: " LAB_ANSI_COLOR_RESET
#define _LAB_WARN_PREFIX    LAB_ANSI_COLOR_YELLOW "[*]: " LAB_ANSI_COLOR_RESET
#define _LAB_ERROR_PREFIX   LAB_ANSI_COLOR_RED    "[!]: " LAB_ANSI_COLOR_RESET
#define _LAB_SUCCESS_PREFIX LAB_ANSI_COLOR_GREEN  "[^]: " LAB_ANSI_COLOR_RESET

#define _LAB_PRINT_TEMPLATE(target, exec) va_list args;                       \
                                          va_start(args, fmt);                \
                                          FILE* print_stream = (FILE*)target; \
                                          exec                                \
                                          va_end(args)

static FILE* _g_lab_print_stream = NULL;
static FILE* _g_lab_error_stream = NULL;

void* lab_print_stream() {
    if(_g_lab_print_stream==NULL) {
        _g_lab_print_stream = stdout;
    }
    return _g_lab_print_stream;
}
void* lab_error_stream() {
    if(_g_lab_error_stream==NULL) {
        _g_lab_error_stream = stderr;
    }
    return _g_lab_error_stream;
}
int lab_retarget_print(void* target) {
    if(target==NULL) {
        return 1;
    } else {
        _g_lab_print_stream = (FILE*)target;
        return 0;
    }
}
int lab_retarget_error(void* target) {
    if(target==NULL) {
        return 1;
    } else {
        _g_lab_error_stream = (FILE*)target;
        return 0;
    }
}

void lab_print    (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_PRINT_PREFIX);
    vfprintf(print_stream, fmt, args);

    );
}
void lab_println  (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_PRINT_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");

    );
}
void lab_notice   (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_NOTICE_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void lab_noticeln (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_NOTICE_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void lab_warn     (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_error_stream(),

    fprintf(print_stream,_LAB_WARN_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void lab_warnln   (const char* fmt, ...) { 
    _LAB_PRINT_TEMPLATE(lab_error_stream(),

    fprintf(print_stream,_LAB_WARN_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void lab_error    (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_error_stream(),

    fprintf(print_stream,_LAB_ERROR_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void lab_errorln  (const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_error_stream(),

    fprintf(print_stream,_LAB_ERROR_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void lab_success (const char* fmt, ...) { 
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_SUCCESS_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void lab_successln(const char* fmt, ...) {
    _LAB_PRINT_TEMPLATE(lab_print_stream(),

    fprintf(print_stream,_LAB_SUCCESS_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}