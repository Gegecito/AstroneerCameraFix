#include "fnamepool.h"


std::string FName::GetName()
{
	auto entry = NamePool->Allocator.GetById(Index);
	auto name = entry->String();
	if (Number > 0)
	{
		name += "_" + std::to_string(Number);
	}
	auto pos = name.rfind('/');
	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}
	return name;
}

std::string FNameEntry::String()
{
	if (Header.bIsWide) { return std::string(); }
	return { AnsiName, Header.Len };
}
FNamePool* NamePool = 0;