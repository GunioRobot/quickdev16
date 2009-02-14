/******************************************************************************
 
 efsl Demo-Application for Philips LPC2138 ARM7TDMI-S
 
 Copyright (c) 2005
 Martin Thomas, Kaiserslautern, Germany <mthomas@rhrk.uni-kl.de>
 
 *****************************************************************************/

#include <string.h>

#include "lpc214x.h"
#include "lpc_config.h"

#include "uart.h"

#include "startup.h"
#include "efs.h"
#include "ls.h"
#include "mkfs.h"
#include "interfaces/efsl_dbg_printf_arm.h"

#define rprintf efsl_debug_printf_arm

#define BAUD 115200

#define BIT(x) ((unsigned long)1<<x)

// 1st LED on Keil MCB2130
#define LED1PIN  	10
#define LED1BIT     BIT(LED1PIN)
#define LEDDIR      IODIR0
#define LEDSET      IOSET0
#define LEDCLR      IOCLR0
static char LogFileName[] = "dummy.log";

/*
static void systemInit(void)
{
	// --- enable and connect the PLL (Phase Locked Loop) ---
	// a. set multiplier and divider
	PLLCFG = MSEL | (1<<PSEL1) | (0<<PSEL0);
	// b. enable PLL
	PLLCON = (1<<PLLE);
	// c. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	// d. wait for PLL lock (PLOCK bit is set if locked)
	while (!(PLLSTAT & (1<<PLOCK)));
	// e. connect (and enable) PLL
	PLLCON = (1<<PLLE) | (1<<PLLC);
	// f. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	
	// --- setup and enable the MAM (Memory Accelerator Module) ---
	// a. start change by turning of the MAM (redundant)
	MAMCR = 0;	
	// b. set MAM-Fetch cycle to 3 cclk as recommended for >40MHz
	MAMTIM = MAM_FETCH;
	// c. enable MAM 
	MAMCR = MAM_MODE;
	
	// --- set VPB speed ---
	VPBDIV = VPBDIV_VAL;
	
	// --- map INT-vector ---
	#if defined(RAM_RUN)
	  MEMMAP = MEMMAP_USER_RAM_MODE;
	#elif defined(ROM_RUN)
	  MEMMAP = MEMMAP_USER_FLASH_MODE;
	#else
	#error RUN_MODE not defined!
	#endif
}
*/
static void gpioInit(void)
{
    LEDSET = BIT(LED1PIN);      // set Bit = LED off (active low)
    LEDDIR |= BIT(LED1PIN);     // define LED-Pin as output
} static void ledToggle(void)
{
    static unsigned char state = 0;
    state = !state;
    if (state)
        LEDCLR = BIT(LED1PIN);  // set Bit = LED on
    else
        LEDSET = BIT(LED1PIN);  // set Bit = LED off (active low)
}


#if 0
static void hang(void)
{
    while (1);
}


#endif                          /*  */
EmbeddedFileSystem efs;
EmbeddedFile filer, filew;
DirList list;
unsigned short e;
unsigned char buf[513];
int main(void)
{
    int ch;
    int8_t res;
    Initialize();
    gpioInit();
    uart0Init(UART_BAUD(BAUD), UART_8N1, UART_FIFO_8);  // setup the UART
    uart0Puts("\r\nMMC/SD Card Filesystem Test (P:LPC2148 L:EFSL)\r\n");

    /* init efsl debug-output */
    efsl_debug_devopen_arm(uart0Putch);
    ledToggle();
    rprintf("CARD init...");
    if ((res = efs_init(&efs, 0)) != 0) {
        rprintf("failed with %i\n", res);
    }
    rprintf("Press Command: d r a \n");
    while (1) {
        if ((ch = uart0Getch()) >= 0) {
            uart0Puts("You pressed : ");
            uart0Putch(ch);
            uart0Puts("\r\n");
            if (ch == 'M') {
                rprintf("Creating FS\n");
                fs_umount(&efs.myFs);
                mkfs_makevfat(&efs.myPart);
            }
            if (ch == 'd') {
                rprintf("Directory of 'root':\n");
                ls_openDir(&list, &(efs.myFs), "/");
                while (ls_getNext(&list) == 0) {
                    list.currentEntry.FileName[LIST_MAXLENFILENAME - 1] = '\0';
                    rprintf("%s ( %li bytes )\n", list.currentEntry.FileName, list.currentEntry.FileSize);
                }
            }
            if (ch == 'r') {
                if (file_fopen(&filer, &efs.myFs, LogFileName, 'r') == 0) {
                    rprintf("File %s open. Content:\n", LogFileName);
                    while ((e = file_read(&filer, 512, buf)) != 0) {
                        buf[e] = '\0';
                        uart0Puts((char *) buf);
                    } rprintf("\n");
                    file_fclose(&filer);
                }
            }
            if (ch == 'a') {
                if (file_fopen(&filew, &efs.myFs, LogFileName, 'a') == 0) {
                    rprintf("File %s open for append. Appending...", LogFileName);
                    strcpy((char *) buf, "Append Dummy Data to File\r\n");
                    if (file_write(&filew, strlen((char *) buf), buf) == strlen((char *) buf)) {
                        rprintf("ok\n\n");
                    } else {
                        rprintf("fail\n\n");
                    }
                    file_fclose(&filew);
                    fs_umount(&efs.myFs);
                }
            }
            ledToggle();
        }
    }
    return 0;                   /* never reached */
}
