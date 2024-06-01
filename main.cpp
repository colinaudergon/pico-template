#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "wavReader.h"
#include "audioDriver.h"
#include "sevenSegController.h"
#include "userInterface.h"
#include "indexLedsManager.h"
#include "utils.hpp"
#define AUDIO_PIN 0

using namespace wavReader;

/*Classes*/
AudioDriver driver;
WavReader wr;
SevenSegController sevSegCtrl;
UserInterface ui;
IndexLedsManager indexLeds;

/*Global variables*/
uint count;
int val = 0;
uiState state;
bool newDataAvailable = false;

/*Audio interface constante*/
#define BUFFER_SIZE 512
uint8_t buffer[BUFFER_SIZE];
uint8_t audio_buffer_1[BUFFER_SIZE];
// uint8_t buffer_2[BUFFER_SIZE];

/*Seven segment controller Pin*/
#define SHIFT_REG_CLK 22
#define SHIFT_REG_LATCH 21
#define SHIFTR_REG_DATA_OUT 20

const int DATA[10] = {
    0x3f, // 0
    0x06, // 1
    0x5b, // 2
    0x4f, // 3
    0x66, // 4
    0x6d, // 5
    0x7d, // 6
    0x07, // 7
    0x7f, // 8
    0x67  // 9
};

float detuneAmount;

/*Fills the buffers with corresponding audio data*/
// Doesnt wooooork!!!!
void audioBufferFiller(uint8_t *songsIndex, size_t numberOfSongs)
{
    if (!newDataAvailable)
    {
        /*Goes throught the list of song*/
        for (size_t i = 0; i < numberOfSongs; i++)
        {
            // /*Tests if the file is already opened or not*/
            // if (wr.IsFileOpened(songsIndex[i]))
            // {
                /*If the files is open, the audio content can be read*/
                if (wr.ReadAudioData(songsIndex[i], audio_buffer_1, BUFFER_SIZE) > 0)
                {
                    /*accumulates the audio content, erase data overflowing (probably not necessary)*/
                    buffer[i] += audio_buffer_1[i];
                    if (buffer[i] >= 255)
                    {
                        buffer[i] = 254;
                    }
                    newDataAvailable = true;
                }
            // }
            // else
            // {
            //     /*If end of file, file as been read, if not, files need to be opened*/
            //     /*If not the end of the file, it needs to be opened before any reading*/
            //         if(wr.OpenWave(i)<0)
            //         {
            //             printf("Failed to open file\n"); // Debug
            //         }
            // }
        }
    }
    
}

/*Audio playback methods*/
void audioCallback(uint8_t *toFillBuffer, uint8_t *freeBuffer, size_t size)
{
    if (newDataAvailable)
    {
        for (size_t i = 0; i < size; i++)
        {
            /*Fills the toFillBuffer with scaled audio data*/
            toFillBuffer[i] = buffer[i] / 4;
            /*Reset the content of buffer */
            buffer[i] = 0;
        }
    }
    newDataAvailable = false;

    driver.AcknoledgedIrq();

    /*Polls user interface*/
    state = ui.Poll();

    /*Displays selected bank*/
    sevSegCtrl.ShiftData(DATA[static_cast<uint8_t>(map_uint16_t(state.adcValues[0], 0, 4094, 0, 9))]);

    /*Detune the audio*/
    // detuneAmount = map_float(state.adcValues[1], 0, 4094, 50, -1);
    // driver.Detune(detuneAmount);

    // state
    /*Check system state*/
}

#define CURRENTAUDIO 1

int main()
{
    stdio_init_all();
    set_sys_clock_khz(176000, true);

    sleep_ms(1000);
    sleep_ms(1000);
    sleep_ms(1000);

    wr.Init(BUFFER_SIZE);
    sevSegCtrl.Init(SHIFTR_REG_DATA_OUT, SHIFT_REG_LATCH, SHIFT_REG_CLK);

    uint8_t btnPin[1] = {15};
    ui.Init(2, btnPin, 1);

    uint ledPins[4] = {10, 11, 12, 13};
    indexLeds.Init(ledPins, 4);

    if (wr.OpenWave(CURRENTAUDIO) < 0)
    {
        printf("Error opening wav file\n");
    }


    driver.init(AUDIO_PIN, 1, BUFFER_SIZE);
    driver.start(&audioCallback);

    uint8_t songsIndex[2] = {CURRENTAUDIO};//, 1, 1};
    while (true)
    {
        ui.Run();

        // Check if the buffer needs updating

        // audioBufferFiller(songsIndex, 1);

        // if (wr.ReadAudioData(CURRENTAUDIO, buffer_1, BUFFER_SIZE) > 0)
        // {

        //     if (wr.ReadAudioData(0, buffer_2, BUFFER_SIZE) > 0)
        //     {
        //         for (size_t i = 0; i < BUFFER_SIZE; i++)
        //         {
        //             buffer[i] = (buffer_1[i] + buffer_2[i]) / 2;
        //             if (buffer[i] >= 255)
        //             {
        //                 buffer[i] = 254;
        //             }
        //         }
        //     }
        //     else
        //     {
        //         wr.CloseWave(0);
        //         if (wr.OpenWave(0) < 0)
        //         {
        //             printf("Error opening wav file\n");
        //         }
        //         for (size_t i = 0; i < BUFFER_SIZE; i++)
        //         {
        //             buffer[i] = buffer_1[i];
        //         }
        //     }

        // }
        // }
    }
}
