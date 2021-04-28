#include "mouse.h"

MouseEventHandler::MouseEventHandler(){
}

void MouseEventHandler::OnActivate(){}
void MouseEventHandler::OnMouseUp(uint8_t button){}
void MouseEventHandler::OnMouseDown(uint8_t button){}
void MouseEventHandler::OnMouseMove(int a, int b){}





MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
    :InterruptHandler(0x2C, manager), dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::Activate(){

    offset = 0;
    buttons = 0;

    commandport.Write(0xA8); // activate interrupts
    commandport.Write(0x20); // get current state

    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

void printf(char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{

    uint8_t status = commandport.Read();
    if(!(status & 0x20)) // commandport must have 6 bit on for read 
        return esp;

    buffer[offset] = dataport.Read();

    if(!handler) return esp; // handler must exist.

    offset = (offset + 1) % 3;

    if(!offset){ // if buff is full process buff
        if(buffer[1] || buffer[2]){
            handler->OnMouseMove(buffer[1], -buffer[2]);
        }

        for(uint8_t i = 0; i < 3; i++){
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))){
//                VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //flip color of position x,y
//                                      | ((VideoMemory[80*y+x] & 0x0f00) << 4)
//                                      | (VideoMemory[80*y+x] & 0x00ff);
               
                if(buttons & (0x1 << 1)){
                    handler->OnMouseUp(i+1);
                }else{
                    handler->OnMouseDown(i+1);
                }
            }
        }
        buttons = buffer[0];
    }

    return esp;
}

