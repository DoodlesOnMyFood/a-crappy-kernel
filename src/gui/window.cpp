#include <gui/window.h>
using namespace myos::common;
using namespace myos::gui;

Window::Window(Widget* parent, common::int32_t x,common::int32_t y,common::int32_t w,common::int32_t h,common::uint8_t r,common::uint8_t g,common::uint8_t b)
:CompositeWidget(parent, x, y, w, h, r, g, b), Dragging(false)
{}
Window::~Window(){}
void Window::OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button){
    Dragging = button == 1;
    CompositeWidget::OnMouseDown(x,y,button);
}
void Window::OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button){
    Dragging = false;
    CompositeWidget::OnMouseUp(x,y,button);
}
void Window::OnMouseMove(common::int32_t x, common::int32_t y, common::int32_t new_x, common::int32_t new_y){
    if(Dragging){
        this->x += new_x - x;
        this->y += new_y - y;
    }
    CompositeWidget::OnMouseMove(x, y, new_x, new_y);
}
