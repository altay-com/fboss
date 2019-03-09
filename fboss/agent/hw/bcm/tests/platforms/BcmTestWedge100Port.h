/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "fboss/agent/hw/bcm/tests/platforms/BcmTestPort.h"

namespace facebook {
namespace fboss {
class BcmTestWedge100Port : public BcmTestPort {
 public:
  explicit BcmTestWedge100Port(PortID id);

  LaneSpeeds supportedLaneSpeeds() const override {
    return {
        cfg::PortSpeed::GIGE,
        cfg::PortSpeed::XG,
        cfg::PortSpeed::TWENTYFIVEG,
    };
  }

 private:
  // Forbidden copy constructor and assignment operator
  BcmTestWedge100Port(BcmTestWedge100Port const&) = delete;
  BcmTestWedge100Port& operator=(BcmTestWedge100Port const&) = delete;
};
} // namespace fboss
} // namespace facebook
