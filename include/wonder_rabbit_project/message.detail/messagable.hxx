#pragma once

#include "reciever.hxx"
#include "sender.hxx"

namespace wonder_rabbit_project
{
  namespace message
  {
    class messagable_t
      : public sender_t
      , public reciever_t
    {
    };
  }
}