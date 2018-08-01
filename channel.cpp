#include "channel.h"
#include <cassert>
class Flit;
Channel::Channel()
{
}

void Channel::initialize()
{

}

Flit* Channel::popFlitFromStack()
{
    Flit* tmp = pre.flitsStack.front();
    pre.flitsStack.pop_front();
    return tmp;
}

void Channel::evaluate()
{
    while(!flitsStack.empty())
    {
        Flit* f = flitsStack.front();
        f->currentChannelId = channelId;
        pre.flitsStack.push_front(f);
        flitsStack.pop_back();
    }
}

bool Channel::empty() const
{
    return pre.flitsStack.empty();
}
