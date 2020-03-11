/*
 * Copyright (C) 2020 Martin Jäger / Libre Solar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef ZEPHYRSERIAL_H_
#define ZEPHYRSERIAL_H_

#include <zephyr.h>

#include "cicada/bufferedserial.h"

#define Size Cicada::Size

/*!
 * UART driver for STM32 micro controllers, using HAL.
 *
 * *NOTE:* If you use more than one UART port in your program, you need to set
 * the preprocessor define `E_MULTITON_MAX_INSTANCES` to the number of ports
 * used. You can set this macro with the -D compiler argument, for example
 * `-DE_MULTITON_MAX_INSTANCES=2`.
 *
 * In the UART's IRQ handler, get the instance with `ZephyrSerial::getInstance()`
 * and call it's `handleInterrupt()` function. Example:
 * ```
 *     void USART3_IRQHandler()
 *     {
 *         static ZephyrSerial* instance = ZephyrSerial::getInstance(USART3);
 *         instance->handleInterrupt();
 *     }
 * ```
 */

class ZephyrSerial : public Cicada::BufferedSerial
{
public:

    /*!
     * Constructor with user supplied buffers for read/write buffers.
     *
     * \param readBuffer user supplied buffer for data arriving at the serial line
     * \param writeBuffer user supplied buffer to store data before being sent on the serial line
     * \param readBufferSize size of the read buffer
     * \param writeBufferSize size of the write buffer
     * \param devName Zephyr UART/USART device name
     */
    ZephyrSerial(char* readBuffer, char* writeBuffer, Size readBufferSize, Size writeBufferSize,
        const char *devName);

    virtual bool open() override;
    virtual bool isOpen() override;
    virtual bool setSerialConfig(uint32_t baudRate, uint8_t dataBits) override;
    virtual void close() override;
    virtual const char* portName() const override;
    virtual bool rawRead(uint8_t& data) override;
    virtual bool rawWrite(uint8_t data) override;
    virtual void startTransmit() override;
    virtual bool writeBufferProcessed() const override;

    static void handleInterrupt(void *instance);

    bool tx_complete = false;

private:
    // Private constructors to avoid copying
    ZephyrSerial(const ZephyrSerial&);
    ZephyrSerial& operator=(const ZephyrSerial&);

    //void init(USART_TypeDef* uartInstance);

    static ZephyrSerial* instance[E_MULTITON_MAX_INSTANCES];

    struct device *uart_dev;

    uint8_t _flags;
    //IRQn_Type _uartInterruptInstance;
};

#endif
