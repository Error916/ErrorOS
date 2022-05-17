#include "pci.h"
#include "ahci/ahci.h"
#include "memory/heap.h"

void EnumerateFunction(uint64_t deviceAddress, uint64_t function){
	uint64_t offset = function << 12;

	uint64_t functionAddress = deviceAddress + offset;
	MapMemory(&GlobalPTM, (void*)functionAddress, (void*)functionAddress);

	PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

	if(pciDeviceHeader->DeviceID == 0) return;
	if(pciDeviceHeader->DeviceID == 0xffff) return;

	Print(GlobalRenderer, GetVendorName(pciDeviceHeader->VendorID));
	Print(GlobalRenderer, " / ");
	Print(GlobalRenderer, GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
	Print(GlobalRenderer, " / ");
	Print(GlobalRenderer, DeviceClasses[pciDeviceHeader->Class]);
	Print(GlobalRenderer, " / ");
	Print(GlobalRenderer, GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->Subclass));
	Print(GlobalRenderer, " / ");
	Print(GlobalRenderer, GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->Subclass, pciDeviceHeader->ProgIF));
	Next(GlobalRenderer);

	switch(pciDeviceHeader->Class){
		case 0x01: // mass storage controller
			switch(pciDeviceHeader->Subclass){
				case 0x06: // SATA
					switch(pciDeviceHeader->ProgIF){
						case 0x01: // AHCI 1.0 device
							AHCIDriver *ahciDriver = (AHCIDriver*)malloc(sizeof(AHCIDriver));
							AHCIDriverConstructor(ahciDriver, pciDeviceHeader);
					}
			}
	}
}

void EnumerateDevice(uint64_t busAddress, uint64_t device){
	uint64_t offset = device << 15;

	uint64_t deviceAddress = busAddress + offset;
	MapMemory(&GlobalPTM, (void*)deviceAddress, (void*)deviceAddress);

	PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

	if(pciDeviceHeader->DeviceID == 0) return;
	if(pciDeviceHeader->DeviceID == 0xffff) return;

	for(uint64_t function = 0; function < 8; ++function){
		EnumerateFunction(deviceAddress, function);
	}
}

void EnumerateBus(uint64_t baseAddress, uint64_t bus){
	uint64_t offset = bus << 20;

	uint64_t busAddress = baseAddress + offset;
	MapMemory(&GlobalPTM, (void*)busAddress, (void*)busAddress);

	PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

	if(pciDeviceHeader->DeviceID == 0) return;
	if(pciDeviceHeader->DeviceID == 0xffff) return;

	for(uint64_t device = 0; device < 32; ++device){
		EnumerateDevice(busAddress, device);
	}
}

void EnumeratePCI(MCFGHeader* mcfg){
	int entries = ((mcfg->Header.Length) - sizeof(MCFGHeader)) / sizeof(DeviceConfig);
	for(int t = 0; t < entries; ++t){
		DeviceConfig* newDeviceConfig = (DeviceConfig*)((uint64_t)mcfg + sizeof(MCFGHeader) + (sizeof(DeviceConfig) * t));
		for(uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; ++bus){
			EnumerateBus(newDeviceConfig->BaseAddress, bus);
		}
	}
}
