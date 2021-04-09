#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
    :InterruptHandler(0x21, manager), dataport(0x60), commandport(0x64)
{
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE); // activate interrupts
    commandport.Write(0x20); // get current state

    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60);
    dataport.Write(status);

    dataport.Write(0xF4); // activates keyboard

}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{

    uint8_t key = dataport.Read();
    if(key < 0x80){
        switch(key){
            
            case 0x45 : case 0xC5 : case 0xFA : break;

            default:
                if(key >= 0x10 and key <= 0x1b){
                    char temp[2] = " "; 
                    temp[0] = "qwertyuiop[]"[key-0x10];
                    printf(temp);
                }
                if(key >= 0x02 and key <= 0x0d){
                    char temp[2] = " "; 
                    temp[0] = "1234567890-="[key-0x02];
                    printf(temp);
                }
                if(key >= 0x1e and 0x28 >= key){
                    char temp[2] = " "; 
                    temp[0] = "asdfghjkl;'"[key-0x1e];
                    printf(temp);
                }
                if(key >= 0x2c and key <= 0x36){
                    char temp[2] = " "; 
                    temp[0] = "zxcvbnm,./"[key-0x2c];
                    printf(temp);
                }
                switch(key){
                    case 0x1c:
                        printf("\n");
                        break;
                    case 0x0e:
                        printf("\b");
                        break;
                    case 0x2b:
                        printf("\\");
                        break;
                    case 0x39:
                        printf(" ");
                        break;
                    default:
                        break;
                }
                
        }
    }

    return esp;
}

