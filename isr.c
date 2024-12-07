#include <xc.h>
#include "main.h"

extern unsigned char return_time, sec;

void __interrupt() isr(void) {
    static unsigned int count = 0;

    if (TMR2IF == 1) {
        if (++count == 1250) // 1250 -> 1 sec , 625 -> 0.5 sec , 6250 -> 5 sec
        {
            if (sec != 0) {
                sec--;
            } else if (return_time != 0) {
                return_time--;
            }
            count = 0;
        }

        TMR2IF = 0;
    }
}
