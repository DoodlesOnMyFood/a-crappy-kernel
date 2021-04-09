#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
    :InterruptHandler(0x2C, manager), dataport(0x60), commandport(0x64)
{

    offset = 0;
    buttons = 0;
    uint16_t * VideoMemory = (uint16_t*)0xb8000;
    x = 40;
    y = 12;

    VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)  //Init position of cursor
                          | ((VideoMemory[80*12+40] & 0x0f00) << 4)
                          | (VideoMemory[80*12+40] & 0x00ff);

    commandport.Write(0xA8); // activate interrupts
    commandport.Write(0x20); // get current state

    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();

}

MouseDriver::~MouseDriver()
{
}

void printf(char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    static uint16_t * VideoMemory = (uint16_t*)0xb8000;

    uint8_t status = commandport.Read();
    if(!(status & 0x20)) // commandport must have 6 bit on for read.
        return esp;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    if(!offset){ // if buff is full process buff
        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //return color of position x,y
                              | ((VideoMemory[80*y+x] & 0x0f00) << 4)
                              | (VideoMemory[80*y+x] & 0x00ff);

        x += buffer[1];
        if(x < 0) x = 0;
        if(x > 79) x = 79;
        y -= buffer[2];
        if(y < 0) y = 0;
        if(y > 24) y = 24;

        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //flip color of position x,y
                              | ((VideoMemory[80*y+x] & 0x0f00) << 4)
                              | (VideoMemory[80*y+x] & 0x00ff);
    }

    for(uint8_t i = 0; i < 3; i++){
        if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))){
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //flip color of position x,y
                                  | ((VideoMemory[80*y+x] & 0x0f00) << 4)
                                  | (VideoMemory[80*y+x] & 0x00ff);
            
        }
    }

    buttons = buffer[0];

    return esp;
}

