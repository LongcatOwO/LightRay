#pragma once

namespace lightray::mtp
{
    /*
     * Utility type which inherits from all its template paramters.
     *
     * Author: P. Lutchanont
     */
    template <typename... Ts>
    struct inherit_from : Ts... {};

} // namespace lightray::mtp
