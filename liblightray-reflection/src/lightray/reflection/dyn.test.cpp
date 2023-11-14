#include <exception>
#include <format>
#include <memory>
#include <string>

#include <lightray/debug/assertion.hpp>
#include <lightray/debug/print.hpp>
#include <lightray/debug/test_case.hpp>

#include <lightray/reflection/dyn.hpp>
#include <lightray/reflection/gen_meta.hpp>



using namespace lightray;
using namespace lightray::debug;
using lightray::debug::println;
using namespace lightray::mtp;
using namespace lightray::refl;


struct basic_prototype
{
    void eat(int food);
    std::string speak() const;

    LIGHTRAY_REFL_TYPE(namespace(::), basic_prototype, (), 
        (func, eat, (id_accessor, interface_proxy((void)((int)(food))())))
        (func, speak, (id_accessor, interface_proxy((std::string)()(const))))
    )

}; // struct prototype

struct basic_cat
{
private:
    int _food = {};

public:
    void eat(int food) { _food += food / 2; }
    std::string speak() const { return std::format("I ate {} food MEOW!", _food); }

}; // struct basic_cast

struct basic_dog
{
private:
    int _food = {};

public:
    void eat(int food) { _food += food; }
    std::string speak() const { return std::format("I ate {} food WOOF!", _food); }

}; // struct basic_dog

int main()
{
    test_case_database tests;

    lr_test_case(tests, test_basic)
    {
        dyn<basic_prototype, true> cat = std::make_unique<basic_cat>();
        dyn<basic_prototype, true> dog = std::make_unique<basic_dog>();

        assert_true(cat.speak() == "I ate 0 food MEOW!", "");
        assert_true(dog.speak() == "I ate 0 food WOOF!", "");

        cat.eat(30);
        dog.eat(20);

        assert_true(cat.speak() == "I ate 15 food MEOW!", "");
        assert_true(dog.speak() == "I ate 20 food WOOF!", "");

        cat = dog;

        assert_true(cat.speak() == "I ate 20 food WOOF!", "");
    };

    tests.execute_all([](const std::exception& e){ println("{}", e.what()); });

} // fn main
