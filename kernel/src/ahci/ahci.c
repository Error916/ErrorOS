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

		port->buffer = (uint8_t*)RequestPage(&GlobalAllocator);
		memset(port->buffer, 0, 0x1000);

		AHCIPortRead(port, 0, 4, port->buffer);
		for(int i = 0; i < 1024; ++i){
			PutCharS(GlobalRenderer, port->buffer[i]);
		}
		Next(GlobalRenderer);
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

bool AHCIPortRead(Port* port, uint64_t sector, uint32_t sectorCount, void* buffer){
	/* Spin Lock to make the function thread safe*/
	uint64_t spin = 0;
	while((port->hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) ++spin;
	if(spin == 1000000) return false; // port hung

	uint32_t sectorL = (uint32_t)sector;
	uint32_t sectorH = (uint32_t)(sector >> 32);

	port->hbaPort->interruptStatus  = (uint32_t) -1; // clear pending interrupt bits

	HBACommandHeader* cmdHeader = (HBACommandHeader*)port->hbaPort->commandListBase;
	cmdHeader->commandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
	cmdHeader->write = 0; // this is a read
	cmdHeader->prdtLength = 1;

	HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
	memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));

	commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
	commandTable->prdtEntry[0].byteCount = (sectorCount << 9) - 1; // 512 bytes per sector
	commandTable->prdtEntry[0].interruptOnCompletion = 1; // TODO: implement this interrupt

	FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

	cmdFIS->fisType = FIS_TYPE_REG_H2D;
	cmdFIS->commandControl = 1; // command
	cmdFIS->command = ATA_CMD_READ_DMA_EX;

	cmdFIS->lba0 = (uint8_t)sectorL;
	cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
	cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
	cmdFIS->lba3 = (uint8_t)sectorH;
	cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
	cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

	cmdFIS->deviceRegister = 1 << 6; // LBA mode

	cmdFIS->countLow = sectorCount & 0xff;
	cmdFIS->countHigh = (sectorCount >> 8) & 0xff;

	port->hbaPort->commandIssue = 1;

	/* blocking function untill we make the os asyncronus */
	while(true){
		if((port->hbaPort->commandIssue == 0)) break;
		if(port->hbaPort->interruptStatus & HBA_PxIS_TFES) return false; // read insucesfull
	}

	if(port->hbaPort->interruptStatus & HBA_PxIS_TFES) return false; // be sure that we dont jump last check

	return true;
}

bool AHCIPortWrite(Port* port, uint64_t sector, uint32_t sectorCount, void* buffer){
	/* Spin Lock to make the function thread safe*/
	uint64_t spin = 0;
	while((port->hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) ++spin;
	if(spin == 1000000) return false; // port hung

	uint32_t sectorL = (uint32_t)sector;
	uint32_t sectorH = (uint32_t)(sector >> 32);

	port->hbaPort->interruptStatus  = (uint32_t) -1; // clear pending interrupt bits

	HBACommandHeader* cmdHeader = (HBACommandHeader*)port->hbaPort->commandListBase;
	cmdHeader->commandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
	cmdHeader->write = 1; // this is a write
	cmdHeader->prdtLength = 1;

	HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
	memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));

	commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
	commandTable->prdtEntry[0].byteCount = (sectorCount << 9) - 1; // 512 bytes per sector
	commandTable->prdtEntry[0].interruptOnCompletion = 1; // TODO: implement this interrupt

	FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

	cmdFIS->fisType = FIS_TYPE_REG_H2D;
	cmdFIS->commandControl = 1; // command
	cmdFIS->command = ATA_CMD_WRITE_DMA_EX;

	cmdFIS->lba0 = (uint8_t)sectorL;
	cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
	cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
	cmdFIS->lba3 = (uint8_t)sectorH;
	cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
	cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

	cmdFIS->deviceRegister = 1 << 6; // LBA mode

	cmdFIS->countLow = sectorCount & 0xff;
	cmdFIS->countHigh = (sectorCount >> 8) & 0xff;

	port->hbaPort->commandIssue = 1;

	/* blocking function untill we make the os asyncronus */
	while(true){
		if((port->hbaPort->commandIssue == 0)) break;
		if(port->hbaPort->interruptStatus & HBA_PxIS_TFES) return false; // read insucesfull
	}

	if(port->hbaPort->interruptStatus & HBA_PxIS_TFES) return false; // be sure that we dont jump last check

	return true;
}
