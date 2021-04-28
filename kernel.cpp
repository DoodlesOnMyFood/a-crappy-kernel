#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "driver.h"

void printf(char* str)
{
    static uint16_t * VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;

    if(str[0] == '\b'){
        if(x){
            x--;
            printf(" ");
            x--;
            return;
        }else if(y){
            y--;
            x = 80;
            printf(" ");
            x = 80;
            return;
        }
    }


    for(int i = 0; str[i] != '\0'; ++i){
        switch(str[i]){
            case '\n':
                y++;
                x=0;
                break;

            default: 
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
        }
        if( x >= 80){
            y++;
            x = 0;
        }
    
        if( y >= 25){
            for(y = 0; y<25; y++){
                for(x = 0; x <80; x++){
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }

            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t key){
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xf];
    foo[1] = hex[key & 0xf];
    printf(foo);
}

class PrintKeyboardEventHandler : public KeyboardEventHandler{
    public :
        void OnKeyDown(char c){
            char* foo = " ";
            foo[0] = c;
            printf(foo);
        }
};

class MouseToConsole : public MouseEventHandler{
    int8_t x, y;
    public:
        MouseToConsole(){
            uint16_t * VideoMemory = (uint16_t*)0xb8000;
            x = 40;
            y = 12;

            VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)  //Init position of cursor
                                  | ((VideoMemory[80*12+40] & 0x0f00) << 4)
                                  | (VideoMemory[80*12+40] & 0x00ff);
        }


        virtual void OnMouseMove(int offset_x, int offset_y){
            static uint16_t * VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //return color of position x,y
                                  | ((VideoMemory[80*y+x] & 0x0f00) << 4)
                                  | (VideoMemory[80*y+x] & 0x00ff);
            x += offset_x;
            if(x > 79) x = 79;
            if(x < 0) x = 0;
            y += offset_y;
            if(y < 0) y = 0;
            if(y > 24) y = 24;
            

            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) //flip color of position x,y
                                  | ((VideoMemory[80*y+x] & 0x0f00) << 4)
                                  | (VideoMemory[80*y+x] & 0x00ff);

        
        }


};


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t multiboot_magic)
{
    printf("Hello World! --- http://www.AlgorithMan.de\n");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);
    printf("Initializing HardWare, Stage 1\n");
    DriverManager drvManager;
    PrintKeyboardEventHandler kbhandler;
    MouseToConsole mousehandler;

    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);
    MouseDriver mouse(&interrupts, &mousehandler);
    drvManager.AddDriver(&mouse);

    printf("Initializing HardWare, Stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing HardWare, Stage 3\n");
    interrupts.Activate();

    while(1);
}
