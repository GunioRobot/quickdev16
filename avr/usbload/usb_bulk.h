/*
 * =====================================================================================
 *
 * ________        .__        __    ________               ____  ________
 * \_____  \  __ __|__| ____ |  | __\______ \   _______  _/_   |/  _____/
 *  /  / \  \|  |  \  |/ ___\|  |/ / |    |  \_/ __ \  \/ /|   /   __  \
 * /   \_/.  \  |  /  \  \___|    <  |    `   \  ___/\   / |   \  |__\  \
 * \_____\ \_/____/|__|\___  >__|_ \/_______  /\___  >\_/  |___|\_____  /
 *        \__>             \/     \/        \/     \/                 \/
 *              ___.
 *  __ __  _____\_ |__
 * |  |  \/  ___/| __ \
 * |  |  /\___ \ | \_\ \
 * |____//____  >|___  /
 *            \/     \/
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
 
 
#ifndef __USB_BULK_H__
#define __USB_BULK_H__


uint8_t usbFunctionWrite(uint8_t * data, uint8_t len);
uint8_t usbFunctionRead(uint8_t * data, uint8_t len);

#endif
