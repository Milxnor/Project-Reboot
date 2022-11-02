#include "events.h"

bool Events::HasEvent()
{
	return false;
}

void Events::StartEvent()
{
	if (Fortnite_Version == 6.21)
	{
		UObject* BF = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
		UFunction* Func = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.ButterflySequence");
		BF->ProcessEvent(Func);
	}
}