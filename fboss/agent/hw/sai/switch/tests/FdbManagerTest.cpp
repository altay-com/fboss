/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/agent/hw/sai/api/SaiApiTable.h"
#include "fboss/agent/hw/sai/fake/FakeSai.h"
#include "fboss/agent/hw/sai/switch/SaiBridgeManager.h"
#include "fboss/agent/hw/sai/switch/SaiFdbManager.h"
#include "fboss/agent/hw/sai/switch/SaiManagerTable.h"
#include "fboss/agent/hw/sai/switch/SaiPortManager.h"
#include "fboss/agent/hw/sai/switch/SaiVlanManager.h"
#include "fboss/agent/hw/sai/switch/tests/ManagerTestBase.h"
#include "fboss/agent/state/MacEntry.h"
#include "fboss/agent/state/MacTable.h"
#include "fboss/agent/state/SwitchState.h"
#include "fboss/agent/state/Vlan.h"
#include "fboss/agent/types.h"

#include <string>

#include <gtest/gtest.h>

using namespace facebook::fboss;

class FdbManagerTest : public ManagerTestBase {
 public:
  void SetUp() override {
    setupStage = SetupStage::PORT | SetupStage::VLAN | SetupStage::INTERFACE;
    ManagerTestBase::SetUp();
    intf0 = testInterfaces[1];
  }

  void checkFdbEntry(const folly::MacAddress& mac, sai_uint32_t metadata = 0) {
    auto vlanId = VlanID(0);
    SaiFdbTraits::FdbEntry entry{1, vlanId, mac};
    auto portHandle = saiManagerTable->portManager().getPortHandle(
        PortID(intf0.remoteHosts[0].id));
    auto expectedBridgePortId = portHandle->bridgePort->adapterKey();
    auto bridgePortId = saiApiTable->fdbApi().getAttribute(
        entry, SaiFdbTraits::Attributes::BridgePortId{});
    EXPECT_EQ(bridgePortId, expectedBridgePortId);
    EXPECT_EQ(
        saiApiTable->fdbApi().getAttribute(
            entry, SaiFdbTraits::Attributes::Metadata{}),
        metadata);
  }

  static folly::MacAddress kMac() {
    return folly::MacAddress{"00:11:11:11:11:11"};
  }

  std::shared_ptr<MacEntry> makeMacEntry(
      folly::MacAddress mac,
      std::optional<sai_uint32_t> classId) {
    std::optional<cfg::AclLookupClass> metadata;
    if (classId) {
      metadata = static_cast<cfg::AclLookupClass>(classId.value());
    }
    return std::make_shared<MacEntry>(
        mac, PortDescriptor(PortID(intf0.remoteHosts[0].id)), metadata);
  }
  void addMacEntry(
      folly::MacAddress mac = kMac(),
      std::optional<sai_uint32_t> classId = std::nullopt) {
    auto macEntry = makeMacEntry(mac, classId);
    auto newState = programmedState->clone();
    auto newMacTable = newState->getVlans()
                           ->getVlan(VlanID(intf0.id))
                           ->getMacTable()
                           ->modify(VlanID(intf0.id), &newState);
    newMacTable->addEntry(macEntry);
    applyNewState(newState);
  }

  TestInterface intf0;
};

TEST_F(FdbManagerTest, addFdbEntry) {
  addMacEntry();
  checkFdbEntry(kMac());
}

TEST_F(FdbManagerTest, addFdbEntryWithClassId) {
  addMacEntry(kMac(), 42);
  checkFdbEntry(kMac(), 42);
}
