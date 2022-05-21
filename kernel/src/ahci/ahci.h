#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../BasicRenderer.h"
#include "../pci.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"
#include "../memory/heap.h"

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define SATA_SIG_ATAPI 0xeb140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xc33c0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000

typedef enum {
	None = 0,
	SATA = 1,
	SEMB = 2,
	PM = 3,
	SATAPI = 4,
} PortType;

typedef struct {
	uint32_t commandListBase;
        uint32_t commandListBaseUpper;
        uint32_t fisBaseAddress;
        uint32_t fisBaseAddressUpper;
        uint32_t interruptStatus;
        uint32_t interruptEnable;
        uint32_t cmdSts;
        uint32_t rsv0;
        uint32_t taskFileData;
        uint32_t signature;
        uint32_t sataStatus;
        uint32_t sataControl;
        uint32_t sataError;
        uint32_t sataActive;
        uint32_t commandIssue;
        uint32_t sataNotification;
        uint32_t fisSwitchControl;
        uint32_t rsv1[11];
        uint32_t vendor[4];
} HBAPort;

typedef struct {
	uint32_t hostCapability;
	uint32_t globalHostControl;
	uint32_t interrupt;
	uint32_t portsImplemented;
	uint32_t version;
	uint32_t cccControl;
	uint32_t cccPorts;
	uint32_t enclosureManagementLocation;
       	uint32_t enclosureManagementControl;
       	uint32_t hostCapabilitiesExtended;
       	uint32_t biosHandoffCtrlSts;
       	uint8_t rsv0[0x74];
       	uint8_t vendor[0x60];
       	HBAPort ports[1];
} HBAMemory;

typedef struct {
	uint8_t commandFISLenght:5;
	uint8_t atapi:1;
	uint8_t write:1;
	uint8_t prefretchable:1;

	uint8_t reset:1;
	uint8_t bits:1;
	uint8_t clearBusy:1;
	uint8_t rsv0:1;
	uint8_t portMultiplier:4;

	uint16_t prdtLength;
	uint32_t prdbCount;
	uint32_t commandTableBaseAddress;
	uint32_t commandTableBaseAddressUpper;
	uint32_t rsv1[4];
} HBACommandHeader;

typedef struct {
	HBAPort* hbaPort;
	PortType portType;
	uint8_t* buffer;
	uint8_t portNumber;
} Port;

typedef struct {
	PCIDeviceHeader* PCIBaseAddress;
	HBAMemory* ABAR;
	Port* ports[32];
	uint8_t portCount;
} AHCIDriver;

void AHCIDriverConstructor(AHCIDriver* ahciDriver, PCIDeviceHeader* pciBaseAddress);
void AHCIDriverDistructor(AHCIDriver* ahciDriver);
void AHCIProbePorts(AHCIDriver* ahciDriver);
void AHCIPortConfigure(Port* port);
void AHCIPortStopCMD(Port* port);
void AHCIPortStartCMD(Port* port);

