/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "fboss/agent/hw/test/HwTestStatUtils.h"

#include "fboss/agent/HwSwitch.h"
#include "fboss/agent/SwitchStats.h"

#include <folly/logging/xlog.h>

#include <algorithm>

namespace facebook::fboss {

void updateHwSwitchStats(HwSwitch* hw) {
  SwitchStats dummy;
  hw->updateStats(&dummy);
}

uint64_t getPortOutPkts(const HwPortStats& portStats) {
  return *portStats.outUnicastPkts__ref() + *portStats.outMulticastPkts__ref() +
      *portStats.outBroadcastPkts__ref();
}

uint64_t getPortOutPkts(const std::map<PortID, HwPortStats>& port2Stats) {
  return std::accumulate(
      port2Stats.begin(),
      port2Stats.end(),
      0UL,
      [](auto sum, const auto& portIdAndStats) {
        return sum + getPortOutPkts(portIdAndStats.second);
      });
}

} // namespace facebook::fboss
