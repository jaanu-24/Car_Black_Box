#include <xc.h>

void init_adc(void)
{
    /* Selecting Right Justification */
    ADFM = 1; // A/D RESULT FORMAT SELECT BIT
    
    /* Starting the ADC Module */
    ADON = 1; // A/D ON BIT
}

unsigned short read_adc(void)
{
    unsigned short adc_reg_val; // 2bytes
    
    /* Start the ADC conversion */
    GO = 1; // A/D CONVERSION STATUS BIT
    
    /* Wait for the conversion to complete */
    while (nDONE); //nDONE = 0 GO = 0
    
   // xx yyyy yyyy
    adc_reg_val = (ADRESH << 8) | ADRESL;  // 11 1111 1111 -> 1023
                 // xx 0000 0000    yyyy yyyy
    return adc_reg_val; // 0 to 1023
    
}
