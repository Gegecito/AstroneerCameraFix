#pragma once
#include <string>
#include <iostream>


struct FNameEntryHandle {
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 66535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntryHeader
{
	uint16_t bIsWide : 1;
	uint16_t LowerCaseProbeHash : 5;
	uint16_t Len : 10;
};

struct FNameEntry {
	FNameEntryHeader Header;
	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

	std::string String();

	inline int32_t GetId() const
	{
		return *(uint16_t*)&Header;
	}
};


struct FNameEntryAllocator
{
	unsigned char frwLock[0x8];
	int32_t CurrentBlock;
	int32_t CurrentByteCursor;
	uint8_t* Blocks[8192];

	inline int32_t NumBlocks() const
	{
		return CurrentBlock + 1;
	}
	inline FNameEntry* GetById(int32_t key) const
	{
		int block = key >> 16;
		int offset = (uint16_t)key;

		if (!IsValidIndex(key, block, offset))
			return reinterpret_cast<FNameEntry*>(Blocks[0] + 0);

		return reinterpret_cast<FNameEntry*>(Blocks[block] + ((uint64_t)offset * 0x02));

	}
	inline bool IsValidIndex(int32_t key) const
	{
		uint32_t block = key >> 16;
		uint16_t offset = key;
		return IsValidIndex(key, block, offset);
	}

	inline bool IsValidIndex(int32_t key, uint32_t block, uint16_t offset) const
	{
		return (key >= 0 && block < NumBlocks() && offset * 0x02 < 0x1FFFE);
	}
};

struct FNamePool
{
	FNameEntryAllocator Allocator;
	uint32_t AnsiCount;
	uint32_t WideCount;
};

struct FName {
	uint32_t Index;
	uint32_t Number;
	FName() :
		Index(0),
		Number(0)
	{ }

	FName(int32_t i) :
		Index(i),
		Number(0)
	{ }
	std::string GetName();
};

extern FNamePool* NamePool;


