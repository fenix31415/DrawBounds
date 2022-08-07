#include "Geom.h"
#include "FireStorage.h"

global_bounds_t get_npc_bounds(RE::TESObjectREFR* p)
{
	auto local = FiresStorage::get_npc_bounds(p);
	return global_bounds_t(local, p->GetPosition());
}

global_bounds_t get_refr_bounds(RE::TESObjectREFR* p)
{
	auto local = FiresStorage::get_refr_bounds(p);
	return global_bounds_t(local, p->GetPosition());
}
