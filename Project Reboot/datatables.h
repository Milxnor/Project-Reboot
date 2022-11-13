#pragma once

#include "structs.h"

namespace DataTables
{
	template <typename RowDataType = uint8_t>
	static TMap<FName, RowDataType*> GetRowMap(UObject* DataTable)
	{
		static auto RowStructOffset = DataTable->GetOffset("RowStruct");
		return *(TMap<FName, RowDataType*>*)(__int64(DataTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap
	}

	uint8_t* FindRow(UObject* DataTable, const std::string& RowName);
	uint8_t* FindRow(UObject* DataTable, FName RowName);
}