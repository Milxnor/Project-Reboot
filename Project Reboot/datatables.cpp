#include "datatables.h"
#include "helper.h"

uint8_t* DataTables::FindRow(UObject* DataTable, const std::string& RowName)
{
	auto RowNameWide = std::wstring(RowName.begin(), RowName.end());
	FString RowNameFStr = RowNameWide.c_str();
	auto Ret = DataTables::FindRow(DataTable, Helper::Conversion::StringToName(RowNameFStr));
	RowNameFStr.Free();
	return Ret;
}

uint8_t* DataTables::FindRow(UObject* DataTable, FName RowName)
{
	auto RowMap = GetRowMap(DataTable);

	auto RowMapData = RowMap.Pairs.Elements.Data;

	for (int i = 0; i < RowMapData.Num(); i++)
	{
		auto CurrentRow = RowMapData.At(i);
		auto CurrentRowValue = CurrentRow.ElementData.Value;

		if (!CurrentRowValue.First.ComparisonIndex || !CurrentRowValue.Second)
			continue;

		if (CurrentRowValue.First.ComparisonIndex == RowName.ComparisonIndex)
			return CurrentRowValue.Second;
	}

	return nullptr;
}