#pragma once

#include "launch/NAVLaunch.h"

namespace av {
namespace launch {

class ManualCompositionLaunch : public NAVLaunch
{
public:
    void launch() override;

};

} // namespace launch
} // namespace nav
