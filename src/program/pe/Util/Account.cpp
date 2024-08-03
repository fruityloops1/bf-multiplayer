#include "pe/Util/Account.h"
#include "al/Util/Account.h"
#include "util/modules.hpp"

namespace pe {
namespace util {

    nn::account::Uid getCurrentUserUid()
    {
        al::tryInitAccount();
        return al::getUid();
    }

} // namespace util
} // namespace pe
