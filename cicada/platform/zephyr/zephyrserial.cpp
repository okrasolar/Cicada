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

#include "cicada/platform/zephyr/zephyrserial.h"

#include <zephyr.h>
#include <drivers/uart.h>

#include "cicada/irq.h"
#include <cstdint>

using namespace Cicada;

ZephyrSerial::ZephyrSerial(char* readBuffer, char* writeBuffer, Size readBufferSize,
        Size writeBufferSize, const char *devName) :
    BufferedSerial(readBuffer, writeBuffer, readBufferSize, writeBufferSize)
{
    uart_dev = device_get_binding(devName);
}

bool ZephyrSerial::setSerialConfig(uint32_t baudRate, uint8_t dataBits)
{
    struct uart_config uart_conf;

    uart_config_get(uart_dev, &uart_conf);

    if (baudRate < 50 || baudRate > 4500000) {
        return false;
    }

    uart_conf.baudrate = baudRate;

    switch (dataBits) {
        case 8:
            uart_conf.data_bits = UART_CFG_DATA_BITS_8;
            break;
        case 9:
            uart_conf.data_bits = UART_CFG_DATA_BITS_9;
            break;
        default:
            return false;
    }

    return uart_configure(uart_dev, &uart_conf) == 0;
}

bool ZephyrSerial::open()
{
    return true;
}

void ZephyrSerial::close() {;}

bool ZephyrSerial::isOpen()
{
    return true;
}

const char* ZephyrSerial::portName() const
{
    return NULL;
}

bool ZephyrSerial::rawRead(uint8_t& data)
{
    return uart_poll_in(uart_dev, &data) == 0;
}

bool ZephyrSerial::rawWrite(uint8_t data)
{
    if (tx_complete) {
        tx_complete = false;
        uart_poll_out(uart_dev, data);
        return true;
    }
    return false;
}

void ZephyrSerial::startTransmit()
{
    uart_irq_callback_user_data_set(uart_dev, handleInterrupt, this);

    /*
     * This will enable the TC (transmission complete) interrupt, not TXE (transmit data register
     * empty) interrupt.
     * TXE would be more suitable, but is not supported by Zephyr driver at the moment.
     */
    uart_irq_tx_enable(uart_dev);
}

void ZephyrSerial::handleInterrupt(void *user_data)
{
    ZephyrSerial *instance = (ZephyrSerial *)user_data;

    uart_irq_update(instance->uart_dev);
    instance->tx_complete = uart_irq_tx_complete(instance->uart_dev);

    instance->transferToAndFromBuffer();
}

bool ZephyrSerial::writeBufferProcessed() const
{
    return _writeBuffer.bytesAvailable() == 0 && tx_complete;
}
