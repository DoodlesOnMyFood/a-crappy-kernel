#include <drivers/keyboard.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


KeyboardEventHandler::KeyboardEventHandler(){
}

void KeyboardEventHandler::OnKeyDown(char){
}

void KeyboardEventHandler::OnKeyUp(char){
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
    :InterruptHandler(0x21, manager), dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);
void printfHex(uint8_t);

void KeyboardDriver::Activate(){

    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE); // activate interrupts
    commandport.Write(0x20); // get current state

    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60);
    dataport.Write(status);

    dataport.Write(0xF4); // activates keyboard
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{

    uint8_t key = dataport.Read();

    if(handler == 0) return esp;

    if(key < 0x80){
        switch(key){
            
            case 0x45 : case 0xC5 : case 0xFA : break;

            default:
                if(key >= 0x10 and key <= 0x1b){
                    char temp; 
                    temp = "qwertyuiop[]"[key-0x10];
                    handler->OnKeyDown(temp);
                }
                if(key >= 0x02 and key <= 0x0d){
                    char temp; 
                    temp = "1234567890-="[key-0x02];
                    handler->OnKeyDown(temp);
                }
                if(key >= 0x1e and 0x28 >= key){
                    char temp; 
                    temp = "asdfghjkl;'"[key-0x1e];
                    handler->OnKeyDown(temp);
                }
                if(key >= 0x2c and key <= 0x36){
                    char temp; 
                    temp = "zxcvbnm,./"[key-0x2c];
                    handler->OnKeyDown(temp);
                }
                switch(key){
                    case 0x1c:
                        handler->OnKeyDown('\n');
                        break;
                    case 0x0e:
                        handler->OnKeyDown('\b');
                        break;
                    case 0x2b:
                        handler->OnKeyDown('\\');
                        break;
                    case 0x39:
                        handler->OnKeyDown(' ');
                        break;
                    default:
                        /*
                        printf("Keyboard 0x");
                        printfHex(key);
                        printf(" ");
                        */
                        break;
                }
                
        }
    }

    return esp;
}

