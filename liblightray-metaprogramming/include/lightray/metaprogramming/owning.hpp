#pragma once

namespace lightray::mtp
{
    // signifies that the variable has sole ownership on the resource held by T.
    template <typename T>
    using owning = T;

} // namespace lightray::mtp
