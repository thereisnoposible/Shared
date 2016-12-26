#include "stdafx.h"
#include "Initialer.h"

Initialer initial_mgr;

Initialer::Initialer()
{
    initial_mgr.inital_.insert(this);
}
Initialer::~Initialer()
{
    
}

void Initialer::mgr_Initial()
{
    auto it = initial_mgr.inital_.begin();
    while (it != initial_mgr.inital_.end())
    {
        (*it)->Initial();
        it = initial_mgr.inital_.begin();
    }
}

void Initialer::Initial()
{
    SetInit();
}
void Initialer::SetInit()
{
    initial_mgr.inital_.erase(this);

    if (initial_mgr.inital_.size() == 0 && initial_mgr.callback_ != nullptr)
        initial_mgr.callback_();
}
void Initialer::SetCallBack(std::function<void()> callback)
{
    callback_ = callback;
}