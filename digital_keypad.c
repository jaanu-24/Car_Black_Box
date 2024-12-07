#include <xc.h>
#include "digital_keypad.h"

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
    // TRISB = TRISB | 0X3F; -> (RB7 RB6 - 0) (RB5 RB4 - 1) (RB3 RB2 RB1 RB0 - 1)
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    
    if (mode == LEVEL_DETECTION) // No SWitch is pressed
    {
        return KEYPAD_PORT & INPUT_LINES; // PORTB & 0X3F
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once) //  One SWitch is pressed
        {
            once = 0;
            
            return KEYPAD_PORT & INPUT_LINES; // PORTB & SW's
        }
        else if ((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED) // All Released state
        {
            once = 1;
        }
    }
    
    return ALL_RELEASED;
}
