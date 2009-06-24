#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>         /* for _delay_ms() */


#include "config.h"
#include "sram.h"
#include "uart.h"
#include "debug.h"

void system_init(void)
{
    /*-------------------------------------------------*/
    
    DDRA =  0x00;
    PORTA =  0x00;
    
    /*-------------------------------------------------*/
    
    DDRC |=     ( (1 << AVR_ADDR_LATCH_PIN)	    
                | (1 << AVR_ADDR_SCK_PIN)	    
                | (1 << AVR_ADDR_SER_PIN)	    
                | (1 << AVR_ADDR_LOAD_PIN)	    
                | (1 << AVR_ADDR_DOWN_PIN)	    
                | (1 << AVR_ADDR_UP_PIN));
    
    DDRC &=     ~ (1 << SNES_WR_PIN);	    
 
    PORTC &=    ~((1 << AVR_ADDR_LATCH_PIN)	    
                | (1 << AVR_ADDR_SCK_PIN));
 
    
    PORTC |=    ( (1 << AVR_ADDR_DOWN_PIN)	    
                | (1 << AVR_ADDR_UP_PIN)
                | (1 << AVR_ADDR_LOAD_PIN)	    
                | (1 << SNES_WR_PIN));
    /*-------------------------------------------------*/
    
    DDRB |=     ( (1 << AVR_RD_PIN)	            
                | (1 << AVR_WR_PIN) 
                | (1 << AVR_CS_PIN)	            
                | (1 << SNES_IRQ_PIN));
    
    PORTB |=    ( (1 << AVR_RD_PIN)	            
                | (1 << AVR_WR_PIN) 
                | (1 << AVR_CS_PIN)	            
                | (1 << SNES_IRQ_PIN));
    /*-------------------------------------------------*/
                	        
    
    DDRD |=     ( (1 << AVR_SNES_SW_PIN)	            
                | (1 << HI_LOROM_SW_PIN) 
                | (1 << SNES_WR_EN_PIN));

    PORTD |=    (1 << HI_LOROM_SW_PIN);
                
    PORTD &=    ~((1 << AVR_SNES_SW_PIN) 
                | (1 << SNES_WR_EN_PIN));
    /*-------------------------------------------------*/
                	    
    
}   


void sreg_set(uint32_t addr)
{
    uint8_t i = 24;
#ifdef DEBUG_SREG
    printf("sreg_set: addr=0x%08lx",addr);
#endif
    while(i--) {
        if ((addr & ( 1L << i))){
#ifdef DEBUG_SREG
            printf("1");
#endif
            AVR_ADDR_SER_PORT |= ( 1 << AVR_ADDR_SER_PIN);
        } else {
            AVR_ADDR_SER_PORT &= ~( 1 << AVR_ADDR_SER_PIN);
#ifdef DEBUG_SREG
            printf("0");
#endif
            
        }
        AVR_ADDR_SCK_PORT |= (1 << AVR_ADDR_SCK_PIN);
        AVR_ADDR_SCK_PORT &= ~(1 << AVR_ADDR_SCK_PIN);
    }
#ifdef DEBUG_SREG
    printf("\n");
#endif
    AVR_ADDR_LATCH_PORT |= (1 << AVR_ADDR_LATCH_PIN);
    AVR_ADDR_LATCH_PORT &= ~(1 << AVR_ADDR_LATCH_PIN);
    
    counter_load();
    
}


void sram_bulk_read_start(uint32_t addr)
{
#ifdef DEBUG_SRAM
    printf("sram_bulk_read_start: addr=0x%08lx\n\r", addr);
#endif
    avr_data_in();

    AVR_CS_PORT &= ~(1 << AVR_CS_PIN);

    AVR_WR_PORT |= (1 << AVR_WR_PIN);
    AVR_RD_PORT |= (1 << AVR_RD_PIN);

    sreg_set(addr);

    AVR_RD_PORT &= ~(1 << AVR_RD_PIN);
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

}

inline void sram_bulk_read_next(void)
{
    AVR_RD_PORT |= (1 << AVR_RD_PIN);
    counter_up();
    AVR_RD_PORT &= ~(1 << AVR_RD_PIN);

    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

}


inline uint8_t sram_bulk_read(void)
{
    uint8_t byte;

    byte = AVR_DATA_PIN;
    AVR_RD_PORT |= (1 << AVR_RD_PIN);
    return byte;
}

void sram_bulk_read_end(void)
{
#ifdef DEBUG_SRAM
    printf("sram_bulk_read_end:");
#endif
    AVR_CS_PORT |= (1 << AVR_CS_PIN);
    avr_data_in();
}

uint8_t sram_read(uint32_t addr)
{
    uint8_t byte;
#ifdef DEBUG_SRAM
    printf("sram_read: addr=0x%08lx\n\r", addr);
#endif
    
    avr_data_in();
    
    AVR_CS_PORT &= ~(1 << AVR_CS_PIN);

    AVR_WR_PORT |= (1 << AVR_WR_PIN);
    AVR_RD_PORT |= (1 << AVR_RD_PIN);
    
    sreg_set(addr);
    
    AVR_RD_PORT &= ~(1 << AVR_RD_PIN);
    
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    
    byte = AVR_DATA_PIN;

    AVR_RD_PORT |= (1 << AVR_RD_PIN);
    AVR_CS_PORT |= (1 << AVR_CS_PIN);
    
    avr_data_in();
    return byte;

}

void sram_bulk_write_start(uint32_t addr)
{
#ifdef DEBUG_SRAM
    printf("sram_bulk_write_start: addr=0x%08lx\n\r", addr);
#endif

    avr_data_out();

    AVR_CS_PORT &= ~(1 << AVR_CS_PIN);
    AVR_WR_PORT |= (1 << AVR_WR_PIN);
    AVR_RD_PORT |= (1 << AVR_RD_PIN);

    sreg_set(addr);

    AVR_WR_PORT &= ~(1 << AVR_WR_PIN);
}

inline void sram_bulk_write_next(void)
{
    AVR_RD_PORT |= (1 << AVR_RD_PIN);

    counter_up();

    AVR_WR_PORT &= ~(1 << AVR_WR_PIN);
}

inline void sram_bulk_write( uint8_t data)
{
    AVR_DATA_PORT = data;

    AVR_WR_PORT |= (1 << AVR_WR_PIN);
}

void sram_bulk_write_end(void)
{
#ifdef DEBUG_SRAM
    printf("sram_bulk_write_end:");
#endif
    AVR_CS_PORT |= (1 << AVR_CS_PIN);

    avr_data_in();
}


void sram_write(uint32_t addr, uint8_t data)
{

#ifdef DEBUG_SRAM
    printf("sram_write: addr=0x%08lx data=%x\n\r", addr, data);
#endif

    avr_data_out();
    
    AVR_CS_PORT &= ~(1 << AVR_CS_PIN);
    AVR_WR_PORT |= (1 << AVR_WR_PIN);
    AVR_RD_PORT |= (1 << AVR_RD_PIN);
    
    sreg_set(addr);
    
    AVR_WR_PORT &= ~(1 << AVR_WR_PIN);
    AVR_DATA_PORT = data;
        
    AVR_WR_PORT |= (1 << AVR_WR_PIN);
    AVR_CS_PORT |= (1 << AVR_CS_PIN);
    
    avr_data_in();
}


void sram_bulk_copy(uint32_t addr, uint8_t * src, uint32_t len)
{

    uint32_t i;
    uint8_t *ptr = src;
#ifdef DEBUG_SRAM
    printf("sram_copy: addr=0x%08lx src=0x%p len=%li\n\r", addr,src,len);
#endif
    sram_bulk_write_start(addr);
    for (i = addr; i < (addr + len); i++){
        sram_bulk_write(*ptr++);
        sram_bulk_write_next();
    }
    sram_bulk_write_end();
}

void sram_bulk_read_buffer(uint32_t addr, uint8_t * dst, uint32_t len)
{

    uint32_t i;
    uint8_t *ptr = dst;
#ifdef DEBUG_SRAM
    printf("sram_bulk_read_buffer: addr=0x%08lx dst=0x%p len=%li\n\r", addr,dst,len);
#endif
    sram_bulk_read_start(addr);
    for (i = addr; i < (addr + len); i++) {
        *ptr = sram_bulk_read();
        sram_bulk_read_next();
        ptr++;
    }
    sram_bulk_read_end();
}

void sram_bulk_clear(uint32_t addr, uint32_t len){
    uint32_t i;
#ifdef DEBUG_SRAM
    printf("sram_bulk_clear: addr=0x%08lx len=%li\n\r", addr,len);
#endif
    sram_bulk_write_start(addr);
    for (i = addr; i < (addr + len); i++) {
        if (0 == i % 0xfff)
#ifdef DEBUG_SRAM
        printf("sram_bulk_clear: addr=0x%08lx\n\r", i);
#endif
        sram_bulk_write(0xff);
        sram_bulk_write_next();
    }
    sram_bulk_write_end();
}

void sram_clear(uint32_t addr, uint32_t len)
{
    uint32_t i;
#ifdef DEBUG_SRAM
    printf("sram_clear: addr=0x%08lx len=%li\n\r", addr,len);
#endif
    for (i = addr; i < (addr + len); i++) {
        if (0 == i % 0xfff)
#ifdef DEBUG_SRAM
    printf("sram_clear: addr=0x%08lx\n\r", i);
#endif
        sram_write(i, 0x00);
    }
}

void sram_copy(uint32_t addr, uint8_t * src, uint32_t len)
{

    uint32_t i;
    uint8_t *ptr = src;
#ifdef DEBUG_SRAM
    printf("sram_copy: addr=0x%08lx src=0x%p len=%li\n\r", addr,src,len);
#endif
    for (i = addr; i < (addr + len); i++)
        sram_write(i, *ptr++);
}

void sram_read_buffer(uint32_t addr, uint8_t * dst, uint32_t len)
{

    uint32_t i;
    uint8_t *ptr = dst;
#ifdef DEBUG_SRAM
    printf("sram_read_buffer: addr=0x%08lx dst=0x%p len=%li\n\r", addr,dst,len);
#endif
    for (i = addr; i < (addr + len); i++) {
        *ptr = sram_read(i);
        ptr++;
    }
}

uint8_t sram_check(uint8_t * buffer, uint32_t len)
{
    uint16_t cnt;
#ifdef DEBUG_SRAM
    printf("sram_check: len=%li\n\r",len);
#endif
    for (cnt = 0; cnt < len; cnt++)
        if (buffer[cnt])
            return 1;
    return 0;
}
