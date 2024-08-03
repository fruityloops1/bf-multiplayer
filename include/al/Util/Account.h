#pragma once

// idk where this belongs its the last thing in al/Library

#include "nn/account.h"

namespace al {

void tryInitAccount();
nn::account::Uid getUid();
nn::account::UserHandle getUserHandle();

} // namespace al
