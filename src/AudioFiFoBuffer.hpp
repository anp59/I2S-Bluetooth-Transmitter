#pragma once

#include <driver/i2s_std.h>


class AudioFiFoBuffer {
public:
    static constexpr size_t FIFO_SIZE_BYTES = 16384; // Muss Vielfaches von 4 sein (Stereo, 16 Bit)
    static constexpr size_t I2S_BLOCK_SIZE = 512;    // DMA Blockgröße

    AudioFiFoBuffer() : fifoWrite(0), fifoRead(0) {
        memset(fifo, 0, sizeof(fifo));
    }
    void setChannelHandle(i2s_chan_handle_t i2sHandle){
        m_i2s = i2sHandle;
    }

    // Muss zyklisch aufgerufen werden (z. B. aus Task)
    void loopWriteBuf() {
        alignas(4) uint8_t i2sBuf[I2S_BLOCK_SIZE];
        size_t bytesRead = 0;
        if (i2s_channel_read(m_i2s, i2sBuf, I2S_BLOCK_SIZE, &bytesRead, 50) != ESP_OK || bytesRead == 0)
            return;
        if (fifoFree() >= bytesRead) {
            fifoWriteBytes(i2sBuf, bytesRead);
        } else {
            vTaskDelay(100);
            // ESP_LOGW("ResampleBuffer", "FIFO voll, Daten verworfen %i Bytes",  outBytes - fifoFree());
        }
     }

    // Bluetooth Callback: muss exakt "bytes" liefern
    int32_t getData(uint8_t* data, int32_t bytes) {
        while (fifoAvailable() < static_cast<size_t>(bytes)) {
            vTaskDelay(1);
        }
        fifoReadBytes(data, bytes);
        return bytes;
    }

private:
    i2s_chan_handle_t m_i2s;
    uint8_t fifo[FIFO_SIZE_BYTES];
    size_t fifoWrite;
    size_t fifoRead;

    size_t fifoAvailable() const {
        return (fifoWrite + FIFO_SIZE_BYTES - fifoRead) % FIFO_SIZE_BYTES;
    }

    size_t fifoFree() const {
        return FIFO_SIZE_BYTES - fifoAvailable() - 1;
    }

    void fifoWriteBytes(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            fifo[fifoWrite] = data[i];
            fifoWrite = (fifoWrite + 1) % FIFO_SIZE_BYTES;
        }
    }

    void fifoReadBytes(uint8_t* out, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            out[i] = fifo[fifoRead];
            fifoRead = (fifoRead + 1) % FIFO_SIZE_BYTES;
        }
    }
};
