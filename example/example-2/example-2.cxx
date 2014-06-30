#include <iostream>

#include <wonder_rabbit_project/wonderland/message.hxx>

namespace
{
  using namespace wonder_rabbit_project::wonderland::message;
  
  class example_1_t;
  class example_2_t;
  class example_3_t;
  
  class example_1_t
    : public messagable_t
  {
  public:
    virtual auto name() const -> std::string { return "example_1_t"; }
    virtual auto some_number() const -> int { return 1; }
  };
  
  class example_2_t
    : public example_1_t
  {
  public:
    auto name() const -> std::string override { return "example_2_t"; }
    virtual auto some_number() const -> int { return 2; }
  };
  
  class example_3_t
    : public example_2_t
  {
  public:
    auto name() const -> std::string override { return "example_3_t"; }
    virtual auto some_number() const -> int { return 3; }
  };

  template<class T_from>
  auto hello(const std::shared_ptr<T_from>& from)
    -> message_t::functor_t
  {
    return [&from](const reciever_t* r)
    {
      std::string to_name;
      
      if( auto to = dynamic_cast<const example_3_t*>(r) )
        to_name = to->name();
      else if( auto to = dynamic_cast<const example_2_t*>(r) )
        to_name = to->name();
      else if( auto to = dynamic_cast<const example_1_t*>(r) )
        to_name = to->name();
      else
        throw std::logic_error("cannot dynamic_cast to example_?_t.");
      
      std::cerr
        << "** hello, to " << to_name
        << " from "        << from->name()
        << " **\n"
        ;
    };
  }
  
}

auto main()
  -> int
{
  auto server = std::make_shared<server_t>();
  
  auto e1 = std::make_shared<example_1_t>();
  auto e2 = std::make_shared<example_2_t>();
  auto e3 = std::make_shared<example_3_t>();
  
  // [1] e1 --> e1, hello, async
  std::cerr << "\n[1]\n";
  e1->send_message(e1, hello(e1));
  std::cerr << "something something something...\n";
  e1->invoke_messages();
  
  // [2] e1 --> e1, hello, sync
  std::cerr << "\n[2]\n";
  e1->send_message
  ( e1
  , hello<example_1_t>(e1)
  , message_t::true_predicator()
  );
  
  // [3] e1 --> e2, hello, async
  std::cerr << "\n[3]\n";
  e1->send_message(e2, hello(e1));
  e2->invoke_messages();
  
  // create account on the server
  server->contract_message_reciever(e1);
  server->contract_message_reciever(e2);
  server->contract_message_reciever(e3);
  
  // [4] e1 --> server, hello
  std::cerr << "\n[4]\n";
  e1->send_message(server, hello(e2));
  
  server->invoke_messages();
  
  e1->invoke_messages();
  e2->invoke_messages();
  e3->invoke_messages();
  
  // [5] e1 --> server, hello, odd number predicate, sync
  std::cerr << "\n[5]\n";
  e1->send_message
  ( server
  , hello(e3)
  , [](const reciever_t* const r)
    {
      int number;
      
      if( auto to = dynamic_cast<const example_3_t*>(r) )
        number = to->some_number();
      else if( auto to = dynamic_cast<const example_2_t*>(r) )
        number = to->some_number();
      else if( auto to = dynamic_cast<const example_1_t*>(r) )
        number = to->some_number();
      else
        throw std::logic_error("cannot dynamic_cast to example_?_t.");
      
      auto result = bool(number % 2);
      
      std::cerr << "++ predicate --> " << std::boolalpha << result << "++\n";
      
      return result;
    }
  );
  
}
