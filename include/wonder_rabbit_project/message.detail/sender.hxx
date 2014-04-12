#pragma once

#include <memory>

namespace wonder_rabbit_project
{
  namespace message
  {
    class sender_t
    {
      std::weak_ptr<sender_t> _this_weak_ptr;
      
      auto _send_or_post_message
      ( const reciever_t::shared_t&    target_reciever
      , const message_t::functor_t&    functor         = message_t::nothing_functor()
      , const message_t::predicator_t& predicator      = message_t::true_predicator()
      , const message_t::launch_e      launch          = message_t::launch_e::sync
      )
        -> void
      {
        auto message = std::make_shared<message_t>
          ( functor
          , predicator
          , launch
          );
          
        target_reciever -> recieve_message(std::move(message));
      }
      
    public:
      using type     = sender_t;
      using shared_t = std::shared_ptr<type>;
      
      auto send_message
      ( const reciever_t::shared_t&    target_reciever
      , const message_t::functor_t&    functor         = message_t::nothing_functor()
      , const message_t::predicator_t& predicator      = message_t::true_predicator()
      )
        -> void
      { _send_or_post_message(target_reciever, functor, predicator, message_t::launch_e::sync); }
      
      auto post_message
      ( const reciever_t::shared_t&    target_reciever
      , const message_t::functor_t&    functor         = message_t::nothing_functor()
      , const message_t::predicator_t& predicator      = message_t::true_predicator()
      , const message_t::launch_e      launch          = message_t::launch_e::async
      )
        -> void
     { _send_or_post_message(target_reciever, functor, predicator, message_t::launch_e::async); }
       
    };
  }
}