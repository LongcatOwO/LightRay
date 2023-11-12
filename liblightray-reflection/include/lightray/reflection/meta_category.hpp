#pragma once

#include <lightray/metaprogramming/enum_flag.hpp>

namespace lightray::refl
{
    /*
     * Represents the type of an identifier according to its declaration in C++.
     * For example, 
     *  'struct S;' is a 'type' declaration of 'S'.
     *  'int i;' is a 'variable' declaration of 'i'.
     *
     * Author: P. Lutchanont
     */
    enum struct meta_category
    {
        type                    = 1 << 0,
        variable                = 1 << 1,
        function                = 1 << 2,

        none                    = 0,
        all                     = ~none,

    }; // enum struct category

    // enable enum flag operations on meta_category.
    void lightray_mtp_enum_flag(meta_category);

} // namespace lightray::refl
