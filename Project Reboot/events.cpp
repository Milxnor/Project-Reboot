#include "events.h"

bool Events::HasEvent()
{

}

void Events::StartEvent()
{
	if (Fortnite_Version == 6.21)
	{
		UObject* BF = FindObject("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
		UFunction* Func = FindObject<UFunction>("Function /Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.ButterflySequence");
		BF->ProcessEvent(Func);
	}
}