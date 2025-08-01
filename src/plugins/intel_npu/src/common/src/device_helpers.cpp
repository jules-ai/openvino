// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "intel_npu/common/device_helpers.hpp"

#include "openvino/core/except.hpp"

namespace intel_npu {

bool utils::isNPUDevice(const uint32_t deviceId) {
    // bits 26-24 define interface type
    // 000 - IPC
    // 001 - PCIe
    // 010 - USB
    // 011 - ethernet
    constexpr uint32_t INTERFACE_TYPE_SELECTOR = 0x7000000;
    uint32_t interfaceType = (deviceId & INTERFACE_TYPE_SELECTOR);
    return (interfaceType == 0);
}

uint32_t utils::getSliceIdBySwDeviceId(const uint32_t swDevId) {
    // bits 3-1 define slice ID
    // right shift to omit bit 0, thus slice id is stored in bits 2-0
    // apply b111 mask to discard anything but slice ID
    uint32_t sliceId = (swDevId >> 1) & 0x7;
    return sliceId;
}

std::string utils::getPlatformByDeviceName(const std::string& deviceName) {
    const auto platformPos = deviceName.rfind('.');
    const auto platformName = (platformPos == std::string::npos) ? deviceName : deviceName.substr(0, platformPos);

    return platformName;
}

std::string utils::getCompilationPlatform(const std::string_view platform,
                                          const std::string& deviceId,
                                          std::vector<std::string> availableDevicesNames) {
    // Platform parameter has a higher priority than deviceID
    if (platform != ov::intel_npu::Platform::AUTO_DETECT) {
        return std::string(platform);
    }

    // Get compilation platform from deviceID
    if (!deviceId.empty()) {
        return utils::getPlatformByDeviceName(deviceId);
    }

    // Automatic detection of compilation platform
    if (availableDevicesNames.empty()) {
        OPENVINO_THROW("No NPU devices were found.");
    }

    return utils::getPlatformByDeviceName(availableDevicesNames.at(0));
}

}  // namespace intel_npu
