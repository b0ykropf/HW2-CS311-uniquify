#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/* Documentation for stdarg.h 
http://www.gnu.org/software/libc/manual/html_node/Variable-Arguments-Output.html
*/
int err_exit(char *reason,...){
	va_list args;
        va_start(args, reason);
        vprintf(reason, args);
        va_end(args);
        exit(1);
}
