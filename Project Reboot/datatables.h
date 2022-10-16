#pragma once

#include "structs.h"

namespace DataTables
{
	TMap<FName, uint8_t*> GetRowMap(UObject* DataTable);
	uint8_t* FindRow(UObject* DataTable, const std::string& RowName);
	uint8_t* FindRow(UObject* DataTable, FName RowName);
}