#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct __attribute__((packed)) {
	unsigned char Signature[8];
	uint8_t Checksum;
	uint8_t OEMId[6];
	uint8_t Revision;
	uint32_t RSDTAddress;
	uint32_t Length;
	uint64_t XSDTAddress;
	uint8_t ExtendedChecksum;
	uint8_t Reserved[3];
} RSDP2;

typedef struct __attribute__((packed)) {
	unsigned char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	uint8_t OEMId[6];
	uint8_t OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} SDTHeader;

typedef struct __attribute__((packed)) {
	SDTHeader Header;
	uint64_t Reserved;
} MCFGHeader;

typedef struct __attribute__((packed)) {
	uint64_t BaseAddress;
	uint16_t PCISegGroup;
	uint8_t StartBus;
	uint8_t EndBus;
	uint32_t Reserved;
} DeviceConfig;

void* FindTableACPI(SDTHeader* sdtHeader, char* signature);
