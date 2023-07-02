#include "pch.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <future>
#include "engine.h"
#include <Windows.h>
#include "fnamepool.h"


uint64_t ModuleBase = 0;

void MyThread();
HMODULE module;


char* ScanBasic(const char* pattern, const  char* mask, char* begin, intptr_t size)
{
	intptr_t patternLen = strlen(mask);

	for (int i = 0; i < size; i++)
	{
		bool found = true;
		for (int j = 0; j < patternLen; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (begin + i);
		}
	}
	return nullptr;
}


char* ScanInternal(const char* pattern, const  char* mask, char* begin, intptr_t size)
{
	char* match{ nullptr };
	MEMORY_BASIC_INFORMATION mbi{};

	for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
	{
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

		match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

		if (match != nullptr)
		{
			break;
		}
	}
	return match;
}


uintptr_t GetAddress(std::vector<uintptr_t> multiPointer, std::size_t typeSize) {
	if (multiPointer.empty())
		return 0;

	if (multiPointer.size() == 1) {
		if (IsBadReadPtr(reinterpret_cast<void*>(multiPointer.front()), typeSize))
			return 0;
		return multiPointer.front();
	}
	if (multiPointer.size() == 2) {
		if (IsBadReadPtr(reinterpret_cast<void*>(multiPointer.front() + multiPointer.back()), typeSize))
			return 0;
		return multiPointer.front() + multiPointer.back();
	}

	else {
		uintptr_t baseAddress = multiPointer[0];
		uintptr_t address;

		for (size_t currentOffsetIndex = 1; currentOffsetIndex < multiPointer.size() - 1; ++currentOffsetIndex) {
			uintptr_t currentOffset = multiPointer[currentOffsetIndex];
			address = baseAddress + currentOffset;

			if (IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(uintptr_t)))
				return 0;

			// else if pointer is good, dereference it, save as base address and continue
			baseAddress = *reinterpret_cast<uintptr_t*>(address);
		}

		// After dereferencing all offsets except last one, add the last offset
		address = baseAddress + multiPointer.back();

		// If the end address is bad, return 0
		if (IsBadReadPtr(reinterpret_cast<void*>(address), typeSize))
			return 0;

		return address;
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    module = hModule;
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        std::thread(MyThread).detach();
    }
    return TRUE;
}

void MyThread() {
	ModuleBase = (uint64_t)GetModuleHandleA(0);

	// for getting the gobjects address
	const char* internalSendPattern{ "\x48\x8B\x0D\x00\x00\x00\x00\x4C\x8B\x04\xD1\x48\x98" };
	const char* mask{ "xxx????xxxxxx" };
	uintptr_t Entry = reinterpret_cast<uintptr_t>(ScanInternal(internalSendPattern, mask, reinterpret_cast<char*>(ModuleBase), 0xF000000));
	uintptr_t relative = *(int*)(Entry + 0x3) + Entry + 0x7;
	TUObjectArray* GObjects = (TUObjectArray*)relative;
	
	// for getting FNames address

	const char* internalSendPattern2{ "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\xC0\xC6\x05\x00\x00\x00\x00\x01\x48\x8B\x44\x24\x38" };
	const char* mask2{ "xxx????x????xxxxx????xxxxxx" };
	uintptr_t Entry2 = reinterpret_cast<uintptr_t>(ScanInternal(internalSendPattern2, mask2, reinterpret_cast<char*>(ModuleBase), 0xF000000));
	uintptr_t relative2 = *(int*)(Entry2 + 0x3) + Entry2 + 0x7;
	NamePool = (FNamePool*)relative2;

	for (uint32_t i = 0; i < GObjects->NumElements; i++) {
		auto Object = GObjects->GetObjectPtr(i);
		if (Object)
		{
			auto Namee = Object->GetName();
			auto FullName = Object->GetFullName();
			if (Namee == "YawControlModifier" && FullName.find("PersistentLevel.CameraSpaceWalkingRig_C") != std::string::npos)
			{	
				auto address = (uintptr_t)Object + 0x118;
				float* cool = (float*)address;
				if (*cool == 0.002000000095f) *cool = 0.0f;
			}
		}
		
	}
	for (uint32_t i = 0; i < GObjects->NumElements; i++) {
		auto Object = GObjects->GetObjectPtr(i);
		if (Object)
		{
			auto Namee = Object->GetName();
			auto FullName = Object->GetFullName();
			if (Namee == "PitchControlModifier" && FullName.find("PersistentLevel.CameraSpaceWalkingRig_C") != std::string::npos)
			{
				auto address = (uintptr_t)Object + 0x118;
				float* cool = (float*)address;
				if (*cool == 0.002000000095f) *cool = 0.0f;
			}
		}

	}
	for (uint32_t i = 0; i < GObjects->NumElements; i++) {
		auto Object = GObjects->GetObjectPtr(i);
		if (Object)
		{
			auto Namee = Object->GetName();
			auto FullName = Object->GetFullName();
			if (Namee == "FixedDistanceModifier" && FullName.find("PersistentLevel.CameraSpaceWalkingRig_C") != std::string::npos)
			{
				auto address = (uintptr_t)Object + 0x0F8;
				float* cool = (float*)address;
				*cool = 1.0f;
			}
		}

	}
    FreeLibraryAndExitThread(module, 0);
}
