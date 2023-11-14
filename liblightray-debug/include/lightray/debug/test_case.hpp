#pragma once

#include <exception>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include "assertion.hpp"
#include "print.hpp"

namespace lightray::debug
{
    struct test_case
    {
        struct stub
        {
        private:
            std::string _name;
            std::source_location _location;

        public:
            stub(
                std::string name,
                const std::source_location& location = std::source_location::current()
            )
            :   _name(std::move(name)),
                _location(location)
            {}

            stub           (const stub& ) = delete;
            stub           (      stub&&) = delete;
            stub& operator=(const stub& ) = delete;
            stub& operator=(      stub&&) = delete;
            ~stub() = default;

            friend test_case operator->*(stub&& s, void (*test)())
            {
                return {std::move(s._name), test, s._location};
            }

        }; // struct stub

    private:
        std::string _name;
        void (*_test)();
        std::source_location _location;

    public:
        test_case(
            std::string name,
            void (*test)(),
            const std::source_location& location = std::source_location::current()
        )
        :   _name(std::move(name)),
            _test(test),
            _location(location)
        {
            assert_true(_test, "test cannot be null!", location);
        }

        const std::string& name() const noexcept { return _name; }
        void execute() const { _test(); }
        const std::source_location& source_location() const noexcept { return _location; }

    }; // struct unit_test

    struct test_case_database
    {
    private:
        std::vector<test_case> _test_cases;

    public:
        
        void add(const test_case& test) noexcept
        {
            _test_cases.push_back(test);
        }

        void add(test_case&& test) noexcept
        {
            _test_cases.push_back(std::move(test));
        }

        test_case_database& operator+=(const test_case& test) noexcept
        {
            add(test);
            return *this;
        }

        test_case_database& operator+=(test_case&& test) noexcept
        {
            add(std::move(test));
            return *this;
        }

        template <typename ExceptionHandler>
        void execute_all(ExceptionHandler&& handler) const noexcept
        {
            for (const auto& test : _test_cases) try
            {
                println("Testing [{}]...", test.name());
                test.execute();
            } 
            catch (const std::exception& e)
            {
                println("Test [{}] failed!", test.name());
                handler(e);
            }
        }

    }; // unit_test_database

} // namespace lightray::debug


#define lr_test_case(database, test_name) \
    ; database += ::lightray::debug::test_case::stub{#test_name}->*[]
