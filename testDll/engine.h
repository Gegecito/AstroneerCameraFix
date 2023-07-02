#pragma once

#include <vector>
#include "fnamepool.h"
#include <future>
#include <Windows.h>


class UObject {
public:
    void** VFTable;
    uint32_t ObjectFlags;
    uint32_t InternalIndex;
    struct uclass* ClassPrivate;
    struct FName NamePrivate;
    UObject* OuterPrivate;

    std::string GetName();
    std::string GetFullName();
};


class ufield : public UObject {
public:
    char Unknnowndata_28[0x8];
};

class ustruct : public ufield {
public:
    char pad_30[0x10];
    ustruct* SuperStruct;
    char Unknowndata_48[0x68];
};

class uclass : public ustruct {
public:
    char unknwondata[0x180];
};

template <class T>
class TArray
{
public:
	T** Objects;
	int Count;
	int Size;
};


class TUObjectArray {
public:
    BYTE** Objects;
    BYTE* PreAllocatedObjects;
    uint32_t MaxElements;
    uint32_t NumElements;
    uint32_t MaxChunks;
    uint32_t NumChunks;
    UObject* GetObjectPtr(uint32_t id) const;
    UObject* FindObject(const char* name) const;
    UObject* FindObjectByString(const char* name);
};
