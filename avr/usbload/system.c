/*
 * =====================================================================================
 *
 * ________        .__        __    ________               ____  ________
 * \_____  \  __ __|__| ____ |  | __\______ \   _______  _/_   |/  _____/
 *  /  / \  \|  |  \  |/ ___\|  |/ / |    |  \_/ __ \  \/ /|   /   __  \
 * /   \_/.  \  |  /  \  \___|    <  |    `   \  ___/\   / |   \  |__\  \
 * \_____\ \_/____/|__|\___  >__|_ \/_______  /\___  >\_/  |___|\_____  /
 *        \__>             \/     \/        \/     \/                 \/
 *
 *                                  www.optixx.org
 *
 *
 *        Version:  1.0
 *        Created:  07/21/2009 03:32:16 PM
 *         Author:  david@optixx.org
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>         /* for _delay_ms() */
#include <avr/interrupt.h>


#include "config.h"
#include "sram.h"
#include "system.h"
#include "uart.h"
#include "debug.h"
#include "info.h"
#include "requests.h"
#include "irq.h"

system_t system;

void system_init(void)
{
    snes_reset_hi();
    snes_reset_off();
    system.reset_line = RESET_OFF;

    snes_irq_hi();
    snes_irq_off();
    system.irq_line = IRQ_OFF;


    snes_wr_disable();
    system.wr_line = WR_DISABLE;

    avr_bus_active();
    system.bus_mode = MODE_AVR;

    snes_lorom();
    system.rom_mode = LOROM;

    system.snes_reset_count = 0;
    system.avr_reset_count = 0;

    system.reset_irq = RESET_IRQ_OFF;

}

void system_send_snes_reset()
{
    info_P(PSTR("Reset SNES\n"));
    cli();
    snes_reset_on();
    snes_reset_lo();
    _delay_ms(2);
    snes_reset_hi();
    snes_reset_off();
    sei();
    system.snes_reset_count++;
}

void system_send_snes_irq()
{
    snes_irq_on();
    snes_irq_lo();
    _delay_us(20);
    snes_irq_hi();
    snes_irq_off();
}

void system_snes_irq_off()
{
    snes_irq_off();
    system.irq_line = IRQ_OFF;
}

void system_snes_irq_on()
{
    snes_irq_on();
    system.irq_line = IRQ_ON;
}


void system_set_bus_avr()
{
    avr_bus_active();
    info_P(PSTR("Activate AVR bus\n"));
    system.bus_mode = MODE_AVR;
}

void system_set_wr_disable()
{
    snes_wr_disable();
    system.wr_line = WR_DISABLE;
    info_P(PSTR("Disable SNES WR\n"));
}

void system_set_wr_enable()
{
    snes_wr_enable();
    system.wr_line = WR_ENABLE;
    info_P(PSTR("Enable SNES WR\n"));
}

void system_set_bus_snes()
{
    snes_bus_active();
    system.bus_mode = MODE_SNES;
    info_P(PSTR("Activate SNES bus\n"));
}

void system_set_rom_mode(usb_transaction_t * usb_trans)
{
    if (usb_trans->req_bank_size == 0x8000) {
        snes_lorom();
        system.rom_mode = LOROM;
        info_P(PSTR("Set SNES lorom \n"));
    } else {
        snes_hirom();
        system.rom_mode = HIROM;
        info_P(PSTR("Set SNES hirom \n"));
    }
}

void system_set_rom_lorom()
{
    snes_lorom();
    system.rom_mode = LOROM;
    info_P(PSTR("Set SNES lorom \n"));
}


void system_set_rom_hirom()
{
    snes_hirom();
    system.rom_mode = HIROM;
    info_P(PSTR("Set SNES hirom \n"));
}

char *system_status_helper(uint8_t val)
{
    if (val)
        return "ON";
    else
        return "OFF";
}

char *system_status_bus(uint8_t val)
{
    if (val)
        return "SNES";
    else
        return "AVR";
}

char *system_status_rom(uint8_t val)
{
    if (val)
        return "HIROM";
    else
        return "LOROM";
}

void system_status()
{
    info_P(PSTR("\nBus Mode       %s\n"), system_status_bus(system.bus_mode));
    info_P(PSTR("Rom Mode       %s\n"), system_status_rom(system.rom_mode));
    info_P(PSTR("Reset Line     %s\n"),
           system_status_helper(system.reset_line));
    info_P(PSTR("IRQ Line       %s\n"), system_status_helper(system.irq_line));
    info_P(PSTR("WR Line        %s\n"), system_status_helper(system.wr_line));
    info_P(PSTR("Reset IRQ      %s\n"), system_status_helper(system.reset_irq));
    info_P(PSTR("SNES Reset     0x%02x\n"), system.snes_reset_count);
    info_P(PSTR("AVR Reset      0x%02x\n"), system.avr_reset_count);
}
