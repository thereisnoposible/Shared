#pragma once
#include <functional>
#include <set>
class Initialer
{
public:
    Initialer();
    ~Initialer();

    void mgr_Initial();

    virtual void Initial();
    virtual void SetInit(); //can't overload
    void SetCallBack(std::function<void()> callback);

private:
    std::set<Initialer*> inital_;
    std::function<void()> callback_;
};

extern Initialer initial_mgr;