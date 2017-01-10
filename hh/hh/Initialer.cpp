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
    std::vector<Initialer*> temp;
    for (; it != initial_mgr.inital_.end(); ++it)
    {
        temp.push_back(*it);
    }

    for (int32 i = 0; i < (int32)temp.size(); i++)
    {
        temp[i]->Initial();
    }
}

void Initialer::Initial()
{
    SetInit();
}
void Initialer::SetInit()
{
    auto res = initial_mgr.inital_.find(this);
    if (res == initial_mgr.inital_.end())
        return;

    initial_mgr.inital_.erase(this);

    if (initial_mgr.inital_.size() == 0 && initial_mgr.callback_ != nullptr)
        initial_mgr.callback_();
}
void Initialer::SetCallBack(std::function<void()> callback)
{
    callback_ = callback;
}