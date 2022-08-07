#pragma once

namespace FiresStorage
{
	struct local_bounds_t
	{
		RE::NiMatrix3 Normals;  // cols
		RE::NiPoint3 Base;      // offset from fig center
	};

	class Storage
	{
		static constexpr inline RE::NiPoint3 DEFAULT_BOUND_ANGLE = { 0.0f, 0.0f, 0.0f };

	public:
		local_bounds_t get_refr_bounds([[maybe_unused]] RE::TESObjectREFR* a, float scale)
		{
			RE::NiPoint3 Rot;
			auto Mid = (a->GetBoundMin() + a->GetBoundMax()) * 0.5f;
			auto ToMax = a->GetBoundMax() - Mid;
			const RE::NiMatrix3 M = RE::NiMatrix3{ { ToMax.x, 0, 0 }, { 0, ToMax.y, 0 }, { 0, 0, ToMax.z } };
			return { M * scale, Mid * scale };
		}
	};

	local_bounds_t get_refr_bounds(RE::TESObjectREFR* a);
	local_bounds_t get_npc_bounds(RE::TESObjectREFR* p);
}
using local_bounds_t = FiresStorage::local_bounds_t;
