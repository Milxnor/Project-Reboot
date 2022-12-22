#pragma once

#include "structs.h"

namespace DataTables
{
	template <typename RowDataType = uint8_t>
	static TMap<FName, RowDataType*> GetRowMap(UObject* DataTable)
	{
		static auto RowStructOffset = FindOffsetStruct("Class /Script/Engine.DataTable", "RowStruct");

		/* static auto CurveTableClass = FindObject("/Script/Engine.CurveTable");

		if (DataTable->IsA(CurveTableClass))
		{
			return *(TMap<FName, RowDataType*>*)(__int64(DataTable));
		}
		else
		{
			static auto RowStructOffset = DataTable->GetOffset("RowStruct");
			return *(TMap<FName, RowDataType*>*)(__int64(DataTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap
		} */

		return *(TMap<FName, RowDataType*>*)(__int64(DataTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap
	}

	uint8_t* FindRow(UObject* DataTable, const std::string& RowName);
	uint8_t* FindRow(UObject* DataTable, FName RowName);
}