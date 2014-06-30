#pragma once

#include <list>
#include "reciever.hxx"

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace message
    {
      class server_t
        : public reciever_t
      {
        using recievers_t = std::list<std::weak_ptr<reciever_t>>;
        
        recievers_t _message_recievers;
        std::mutex  _message_recievers_mutex;
        
      public:
        auto recieve_message(message_t::shared_t&& message)
          -> void override
        {
          std::lock_guard<std::mutex> lock_recievers(_message_recievers_mutex);
          
          switch ( message -> launch() )
          {
            case message_t::launch_e::async:
              {
                std::lock_guard<std::mutex> lock_spool(_message_spool_mutex);
                _message_spool.push(std::move(message));
              }
              break;
              
            case message_t::launch_e::sync:
              for(const auto& weak_reciever : _message_recievers)
              {
                auto shared_reciever = weak_reciever.lock();
                
                std::lock_guard<std::mutex> lock_reciever(shared_reciever->_message_spool_mutex);
                
                if(message -> predicate(shared_reciever.get()))
                  message -> function(shared_reciever.get());
              }
          }
        }
        
        auto invoke_messages()
          -> void override
        {
          std::lock_guard<std::mutex> lock_recievers(_message_recievers_mutex);
          std::lock_guard<std::mutex> lock_spool(_message_spool_mutex);
          
          while( ! _message_spool.empty() )
          {
            for( const auto& weak_reciever : _message_recievers )
            {
              const auto& message = _message_spool.front();
              const auto shared_reciever = weak_reciever.lock();
              
              std::lock_guard<std::mutex> lock(shared_reciever->_message_spool_mutex);
              
              if( message -> predicate( shared_reciever.get() ) )
                shared_reciever -> _message_spool.push( message );
            }
            _message_spool.pop();
          }
        }
        
        auto contract_message_reciever(const reciever_t::shared_t& reciever)
          -> void
        { 
          std::lock_guard<std::mutex> lock_recievers(_message_recievers_mutex);
          _message_recievers.push_back(reciever);
        }
        
        auto dissolve_message_reciever(const reciever_t::shared_t& reciever)
          -> void
        {
          std::lock_guard<std::mutex> lock_recievers(_message_recievers_mutex);
          
          _message_recievers.remove_if
          ( [&reciever](const std::weak_ptr<reciever_t> w)
            { return w.lock() == reciever; }
          );
        }
        
      };
    }
  }
}