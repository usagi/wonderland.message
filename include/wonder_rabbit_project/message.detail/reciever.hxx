#pragma once

#include <memory>
#include <queue>
#include <mutex>

namespace wonder_rabbit_project
{
  namespace message
  {
    class reciever_t
    {
      friend class server_t;
      
      using message_spool_t     = std::queue<message_t::shared_t>;
      
      message_spool_t _message_spool;
      std::mutex      _message_spool_mutex;
      
    public:
      using type     = reciever_t;
      using shared_t = std::shared_ptr<type>;
      
      virtual auto recieve_message(message_t::shared_t&& message)
        -> void
      {
        std::lock_guard<std::mutex> lock(_message_spool_mutex);
        if(message->predicate(this))
          switch(message->launch())
          { case message_t::launch_e::async:
              _message_spool.push(std::move(message));
              break;
            case message_t::launch_e::sync:
              message->function(this);
          }
      }
      
      virtual auto invoke_messages()
        -> void
      {
        std::lock_guard<std::mutex> lock(_message_spool_mutex);
        while(!_message_spool.empty())
        {
          if(_message_spool.front()->predicate(this))
            _message_spool.front()->function(this);
          _message_spool.pop();
        }
      }
    };
  }
}