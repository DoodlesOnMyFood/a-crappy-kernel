#ifndef __MYOS__GUI__WIDGET_H
#define __MYOS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>
    
namespace myos{

    namespace gui{
        
        class Widget : public drivers::KeyboardEventHandler {
            protected:
                Widget* parent;
                common::int32_t x;
                common::int32_t y;
                common::int32_t w;
                common::int32_t h;

                common::uint8_t r;
                common::uint8_t g;
                common::uint8_t b;

                bool Focusable;


            public:
                Widget(Widget* parent, common::int32_t x,common::int32_t y,common::int32_t w,common::int32_t h,common::uint8_t r,common::uint8_t g,common::uint8_t b);
                ~Widget();

                virtual void GetFocus(Widget* widget);
                virtual void ModelToScreen(common::int32_t &x, common::int32_t& y);
                virtual bool ContainsCoordiate(common::int32_t x, common::int32_t y);


                virtual void Draw(common::GraphicContext* gc);
                virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
                virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
                virtual void OnMouseMove(common::int32_t x, common::int32_t y, common::int32_t new_x, common::int32_t new_y);
        };

        class CompositeWidget : public Widget{
            private:
                Widget* children[100];
                common::int32_t numChildren;
                Widget* focusedChild;

            public:
                CompositeWidget(Widget* parent, common::int32_t x,common::int32_t y,common::int32_t w,common::int32_t h,common::uint8_t r,common::uint8_t g,common::uint8_t b);
                ~CompositeWidget();

                virtual void GetFocus(Widget* widget);
                virtual bool AddChild(Widget* child);


                virtual void Draw(common::GraphicContext* gc);
                virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
                virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
                virtual void OnMouseMove(common::int32_t x, common::int32_t y, common::int32_t new_x, common::int32_t new_y);
                virtual void OnKeyUp(char);
                virtual void OnKeyDown(char);
        };
    
    }
}


#endif
