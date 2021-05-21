#ifndef __MYOS__GUI__DESKTOP_H
#define __MYOS__GUI__DESKTOP_H

#include <drivers/mouse.h>
#include <gui/widget.h>

namespace myos{
    namespace gui{
        class Desktop : public CompositeWidget, public drivers::MouseEventHandler{
            protected:
                common::uint32_t MouseX;
                common::uint32_t MouseY;

            public:
                Desktop(common::int32_t w, common::int32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);
                ~Desktop();

                void Draw(common::GraphicContext* gc);

                void OnMouseDown(myos::common::uint8_t button);
                void OnMouseUp(myos::common::uint8_t button);
                void OnMouseMove(int, int);
        };
    }
}


#endif
