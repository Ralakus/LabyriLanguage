#pragma once

#ifndef LAB_WINDOWS

#define LAB_ANSI_COLOR_RED     "\x1b[31m"
#define LAB_ANSI_COLOR_GREEN   "\x1b[32m"
#define LAB_ANSI_COLOR_YELLOW  "\x1b[33m"
#define LAB_ANSI_COLOR_BLUE    "\x1b[34m"
#define LAB_ANSI_COLOR_MAGENTA "\x1b[35m"
#define LAB_ANSI_COLOR_CYAN    "\x1b[36m"
#define LAB_ANSI_COLOR_RESET   "\x1b[0m"

#else 

#define LAB_ANSI_COLOR_RED    
#define LAB_ANSI_COLOR_GREEN  
#define LAB_ANSI_COLOR_YELLOW 
#define LAB_ANSI_COLOR_BLUE   
#define LAB_ANSI_COLOR_MAGENTA
#define LAB_ANSI_COLOR_CYAN   
#define LAB_ANSI_COLOR_RESET  

#endif

extern void* lab_print_stream();
//    ^ returns current print stream for print, notice, and success
extern void* lab_error_stream();
//    ^ returns current print stream for warn and error
extern int   lab_retarget_print(void* target);
//    ^ retargets print output
extern int   lab_retarget_error(void* target);
//    ^ retargets error output

extern void lab_print    (const char* fmt, ...);
extern void lab_println  (const char* fmt, ...);
extern void lab_notice   (const char* fmt, ...);
extern void lab_noticeln (const char* fmt, ...);
extern void lab_warn     (const char* fmt, ...);
extern void lab_warnln   (const char* fmt, ...);
extern void lab_error    (const char* fmt, ...);
extern void lab_errorln  (const char* fmt, ...);
extern void lab_success  (const char* fmt, ...);
extern void lab_successln(const char* fmt, ...);