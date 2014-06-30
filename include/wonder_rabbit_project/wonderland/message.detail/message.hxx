#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <stdexcept>

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace message
    {
      class sender_t;
      class reciever_t;
      
      class message_t
        : public std::enable_shared_from_this<message_t>
      {
      public:
        using type           = message_t;
        using shared_t       = std::shared_ptr<message_t>;
        using weak_t         = std::weak_ptr<message_t>;
        
        using functor_t    = std::function<void(const reciever_t*)>;
        using predicator_t = std::function<bool(const reciever_t* const)>;
      
        enum class launch_e
          : std::uint_fast8_t
        { async, sync };
        
        enum class relay_e
          : std::uint_fast8_t
        { direct = 1
        , relay  = direct << 1
        , direct_and_relay = direct | relay
        };
        
      private:
        predicator_t _predicator;
        functor_t    _functor;
        launch_e     _launch;
      
      public:
        explicit message_t
        ( const functor_t&   functor_
        , const predicator_t& predicator_
        , const launch_e      launch_
        )
          : _predicator(predicator_)
          , _functor(functor_)
          , _launch(launch_)
        { }
        
        auto functor() const
          -> const functor_t&
        { return _functor; }
        
        auto function(const reciever_t* reciever)
          -> void
        { _functor(reciever); }
        
        auto predicator() const
          -> const predicator_t&
        { return _predicator; }
        
        auto predicate(const reciever_t* const reciever) const
          -> bool
        { return _predicator(reciever); }
        
        auto launch() const
          -> const launch_e
        { return _launch; }
        
        static auto nothing_functor()
          -> functor_t
        { return [](const reciever_t*){ }; }
          
        static auto true_predicator()
          -> predicator_t
        { return [](const reciever_t* const){ return true; }; }
      };
      
      template<class T = void>
      static auto to_string(const message_t::launch_e l)
        -> std::string
      {
        switch(l)
        { case message_t::launch_e::async: return "message_t::launch_e::async";
          case message_t::launch_e::sync : return "message_t::launch_e::sync";
        }
        throw std::logic_error(std::string("unknown message_t::launch_e value") + std::to_string(uint_fast8_t(l)));
      }
      
    }
  }
}
