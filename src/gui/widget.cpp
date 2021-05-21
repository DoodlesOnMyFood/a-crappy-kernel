#include <gui/widget.h>

using namespace myos::common;
using namespace myos::gui;



Widget::Widget(Widget* parent, int32_t x,int32_t y,int32_t w,int32_t h,uint8_t r,uint8_t g,uint8_t b):parent(parent), x(x), y(y), w(w), h(h), r(r), g(g), b(b), Focusable(true), KeyboardEventHandler() {}

Widget::~Widget(){}

void Widget::GetFocus(Widget* widget){
    if(parent) parent->GetFocus(widget);
}
void Widget::ModelToScreen(int32_t &x, int32_t& y){
    if(parent)
        parent->ModelToScreen(x,y);
    x += this->x;
    y += this->y;
}


void Widget::Draw(GraphicContext* gc){
    int X = 0;
    int Y = 0;
    ModelToScreen(X,Y);
    gc->FillRectangle(X, Y, w, h, r, g, b);
}

bool Widget::ContainsCoordiate(int32_t x, int32_t y){
    return this->x <= x && this->y <= y && this->x + this->w > x && this->y + this->h > y;
}

void Widget::OnMouseDown(int32_t x, int32_t y,uint8_t button){
    if(Focusable) GetFocus(this);
}
void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button){}
void Widget::OnMouseMove(int32_t x, int32_t y, int32_t new_x, int32_t new_y){}




CompositeWidget::CompositeWidget(Widget* parent, int32_t x,int32_t y,int32_t w,int32_t h,uint8_t r,uint8_t g,uint8_t b):Widget(parent, x, y, w, h, r, g, b), focusedChild(0), numChildren(0){}
CompositeWidget::~CompositeWidget(){}

void CompositeWidget::GetFocus(Widget* widget){
    this->focusedChild = widget;
    if(parent)
        parent->GetFocus(this);
}

bool CompositeWidget::AddChild(Widget* child){
    if(numChildren == 100)
        return false;
    children[numChildren++] = child;
    return true;
}


void CompositeWidget::Draw(GraphicContext* gc){
    Widget::Draw(gc);
    for(int i = numChildren-1; i >= 0; i--){
        children[i]->Draw(gc);
    }
}
void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button){
    for(int i = 0; i < numChildren; i++){
        if(children[i]->ContainsCoordiate(x - this->x, y - this->y))
            children[i]->OnMouseDown(x - this ->x, y - this->y, button);
    }
}
void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button){
    for(int i = 0; i < numChildren; i++){
        if(children[i]->ContainsCoordiate(x - this->x, y - this->y))
            children[i]->OnMouseUp(x - this ->x, y - this->y, button);
    }
}
void CompositeWidget::OnMouseMove(int32_t x, int32_t y, int32_t new_x, int32_t new_y){
    int firstchild = -1;
    for(int i = 0; i < numChildren; i++){
        if(children[i]->ContainsCoordiate(x - this->x, y - this->y)){
            children[i]->OnMouseMove(x - this->x, y - this->y, new_x - this->x, new_y - this->y);
            firstchild = i;
            break;
        }
    }
    for(int i = 0; i < numChildren; i++){
        if(children[i]->ContainsCoordiate(new_x - this->x, new_y - this->y)){
            if(firstchild != i)
                children[i]->OnMouseMove(x - this ->x, y - this->y, new_x - this->x, new_y - this->y);
            break;
        }
    }

}
void CompositeWidget::OnKeyUp(char key){
    if(focusedChild)
        focusedChild->OnKeyUp(key);
}
void CompositeWidget::OnKeyDown(char key){
    if(focusedChild)
        focusedChild->OnKeyDown(key);
}

