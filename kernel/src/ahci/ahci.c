#include "ahci.h"

void AHCIPortStopCMD(Port* port){
	port->hbaPort->cmdSts &= ~HBA_PxCMD_ST;
	port->hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

	while(true){
		if(port->hbaPort->cmdSts & HBA_PxCMD_FR) continue;
		if(port->hbaPort->cmdSts & HBA_PxCMD_CR) continue;

		break;
	}
}

void AHCIPortStartCMD(Port* port){
	while(port->hbaPort->cmdSts & HBA_PxCMD_CR);

	port->hbaPort->cmdSts |= HBA_PxCMD_FRE;
	port->hbaPort->cmdSts |= HBA_PxCMD_ST;
}

void AHCIPortConfigure(Port* port){
	AHCIPortStopCMD(port);

	void* newBase = RequestPage(&GlobalAllocator);
	port->hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
	port->hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
	memset((void*)(port->hbaPort->commandListBase), 0, 1024);

	// we waste a little bit of memory for convinience
	void* fisBase = RequestPage(&GlobalAllocator);
	port->hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
	port->hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
	memset(fisBase, 0, 256);

	HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)port->hbaPort->commandListBase + ((uint64_t)port->hbaPort->commandListBaseUpper << 32));

	for(int i = 0; i < 32; ++i){
		cmdHeader[i].prdtLength = 8;

		void* cmdTableAddress = RequestPage(&GlobalAllocator);
		uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
		cmdHeader[i].commandTableBaseAddress = (uint32_t)address;
		cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
		memset(cmdTableAddress, 0, 256);
	}

	AHCIPortStartCMD(port);
}

void AHCIDriverConstructor(AHCIDriver* ahciDriver, PCIDeviceHeader* pciBaseAddress){
	ahciDriver->portCount = 0;
	ahciDriver->PCIBaseAddress = pciBaseAddress;
	Print(GlobalRenderer, "AHCI Driver Istance Initialized");
	Next(GlobalRenderer);

	ahciDriver->ABAR = (HBAMemory*)((PCIHeader0*)pciBaseAddress)->BAR5;

	MapMemory(&GlobalPTM, ahciDriver->ABAR, ahciDriver->ABAR);
	AHCIProbePorts(ahciDriver);

	for(int i = 0; i < ahciDriver->portCount; ++i){
		Port* port = ahciDriver->ports[i];
		AHCIPortConfigure(port);
	}

}

void AHCIDriverDistructor(AHCIDriver* ahciDriver){}

PortType AHCICheckPortType(HBAPort* port){
	uint32_t sataStatus = port->sataStatus;
	uint8_t interfacePowerManagment = (sataStatus >> 8) & 0b111;
	uint8_t deviceDetection = sataStatus  & 0b111;

	if(deviceDetection != HBA_PORT_DEV_PRESENT) return None;
	if(interfacePowerManagment != HBA_PORT_IPM_ACTIVE) return None;

	switch(port->signature){
		case SATA_SIG_ATAPI:
			return SATAPI;
		case SATA_SIG_ATA:
			return SATA;
		case SATA_SIG_SEMB:
			return SEMB;
		case SATA_SIG_PM:
			return PM;
		default:
			return None;
	}
}

void AHCIProbePorts(AHCIDriver* ahciDriver){
	uint32_t portsImplemented = ahciDriver->ABAR->portsImplemented;

	for(int i = 0; i < 32; ++i){
		if(portsImplemented & (1 << i)){
			PortType portType = AHCICheckPortType(&ahciDriver->ABAR->ports[i]);

			if(portType == SATA || portType == SATAPI){
				ahciDriver->ports[ahciDriver->portCount] = (Port*)malloc(sizeof(Port));
				ahciDriver->ports[ahciDriver->portCount]->portType = portType;
				ahciDriver->ports[ahciDriver->portCount]->hbaPort = &ahciDriver->ABAR->ports[i];
				ahciDriver->ports[ahciDriver->portCount]->portNumber = ahciDriver->portCount;
				ahciDriver->portCount++;
			}
		}
	}
}
