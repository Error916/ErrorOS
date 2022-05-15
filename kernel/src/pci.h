#pragma once
#include <stdint.h>
#include "acpi.h"
#include "paging/PageTableManager.h"
#include "cstr.h"
#include "BasicRenderer.h"

typedef struct {
	uint16_t VendorID;
	uint16_t DeviceID;
	uint16_t Command;
	uint16_t Status;
	uint8_t RevisionID;
	uint8_t ProgIF;
	uint8_t Subclass;
	uint8_t Class;
	uint8_t CacheLineSize;
	uint8_t LatencyTimer;
	uint8_t HeaderType;
	uint8_t BIST;
} PCIDeviceHeader;

void EnumeratePCI(MCFGHeader* mcfg);

extern const char* DeviceClasses[];


const char* GetVendorName(uint16_t vendorID);
const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);
const char* MassStorageControllerSubclassName(uint8_t subclassCode);
const char* SerialBusControllerSubclassName(uint8_t subclassCode);
const char* BridgeDeviceSubclassName(uint8_t subclassCode);
const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);
const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);
