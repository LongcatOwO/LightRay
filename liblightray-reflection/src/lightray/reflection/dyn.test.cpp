#include <exception>
#include <memory>
#include <sstream>

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
    void speak(std::ostream& os) const;

    LIGHTRAY_REFL_TYPE(namespace(::), basic_prototype, (), 
        (func, eat, (id_accessor, interface_proxy((void)((int)(food))())))
        (func, speak, (id_accessor, interface_proxy((void)((std::ostream&)(os))(const))))
    )

}; // struct prototype

struct basic_cat
{
private:
    int _food = {};

public:
    void eat(int food) { _food += food / 2; }
    void speak(std::ostream& os) const { println(os, "I ate {} food MEOW!", _food); }

}; // struct basic_cast

struct basic_dog
{
private:
    int _food = {};

public:
    void eat(int food) { _food += food; }
    void speak(std::ostream& os) const { println(os, "I ate {} food WOOF!", _food); }

}; // struct basic_dog

auto main() -> int
{
    test_case_database tests;

    lr_test_case(tests, basic_test)
    {
        const dyn<basic_prototype> cat = std::make_unique<basic_cat>();
        std::ostringstream output;
        cat.speak(output);
        assert_true(output.str() == "I ate 0 food MEOW!\n", "");
    };

    tests.execute_all([](const std::exception& e){ println("{}", e.what()); });

} // fn main
