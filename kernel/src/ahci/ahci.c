#include "ahci.h"

void AHCIDriverConstructor(AHCIDriver* ahciDriver, PCIDeviceHeader* pciBaseAddress){
	ahciDriver->PCIBaseAddress = pciBaseAddress;
	Print(GlobalRenderer, "AHCI Driver Istance Initialized");
	Next(GlobalRenderer);

	ahciDriver->ABAR = (HBAMemory*)((PCIHeader0*)pciBaseAddress)->BAR5;

	MapMemory(&GlobalPTM, ahciDriver->ABAR, ahciDriver->ABAR);
	AHCIProbePorts(ahciDriver);
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

			if(portType == SATA){
				Print(GlobalRenderer, "SATA Drive");
				Next(GlobalRenderer);
			} else 	if(portType == SATAPI){
				Print(GlobalRenderer, "SATAPI Drive");
				Next(GlobalRenderer);
			} else {
				Print(GlobalRenderer, "Not Interested");
				Next(GlobalRenderer);

			}
		}
	}
}
