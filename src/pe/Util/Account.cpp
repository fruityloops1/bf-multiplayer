#include "pe/Util/Account.h"
#include "al/Util/Account.h"

namespace pe {
namespace util {

    nn::account::Uid getCurrentUserUid()
    {
        al::tryInitAccount();
        return al::getUid();
    }

} // namespace util
} // namespace pe
