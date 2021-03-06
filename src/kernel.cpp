#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitask.h>

//#define GRAPHICS_MODE


using namespace myos;
using namespace myos::common;
using namespace myos::gui;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

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

void taskA(){
    while(1)
        printf("A");
}
void taskB(){
    while(1)
        printf("B");
}

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

    uint32_t memUpper = ((uint32_t*)multiboot_structure)[2];
    
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (memUpper * 1024) - heap - 10*1024);
    void * allocated = memoryManager.malloc(1024);
    printf("heap : 0x");
    printfHex((heap>>24) & 0xff);
    printfHex((heap>>16) & 0xff);
    printfHex((heap>>8) & 0xff);
    printfHex((heap) & 0xff);
    
    printf("\nallocated : 0x");
    printfHex(((size_t)allocated>>24) & 0xff);
    printfHex(((size_t)allocated>>16) & 0xff);
    printfHex(((size_t)allocated>>8) & 0xff);
    printfHex(((size_t)allocated) & 0xff);
    printf("\n");

    TaskManager taskManager;
//    Task task1(&gdt, taskA);
//    Task task2(&gdt, taskB);
//    taskManager.AddTask(&task1);
//    taskManager.AddTask(&task2);

    InterruptManager interrupts(0x20, &gdt, &taskManager);
    printf("Initializing HardWare, Stage 1\n");
#ifdef GRAPHICS_MODE
    Desktop desktop(320, 200, 0x0,0x0,0xa8);
#endif
    DriverManager drvManager;
    PrintKeyboardEventHandler kbhandler;
    MouseToConsole mousehandler;

#ifdef GRAPHICS_MODE
    KeyboardDriver keyboard(&interrupts, &desktop);
#else
    KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif
    drvManager.AddDriver(&keyboard);
#ifdef GRAPHICS_MODE
    MouseDriver mouse(&interrupts, &desktop);
#else
    MouseDriver mouse(&interrupts, &mousehandler);
#endif
    drvManager.AddDriver(&mouse);
    PeripheralComponentInterconnectController PCIcontroller;
    PCIcontroller.SelectDrivers(&drvManager, &interrupts);
#ifdef GRAPHICS_MODE
    VideoGraphicsArray vga;
#endif

    printf("Initializing HardWare, Stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing HardWare, Stage 3\n");

#ifdef GRAPHICS_MODE
    vga.SetMode(320,200,8);

    Window win1(&desktop, 10, 10, 20, 20, 0xa8, 0, 0);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, 0, 0xa8, 0);
    desktop.AddChild(&win2);
#endif
    interrupts.Activate();

    while(1){
#ifdef GRAPHICS_MODE
        desktop.Draw(&vga);
#endif
    }
}
