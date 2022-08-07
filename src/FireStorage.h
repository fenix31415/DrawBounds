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
			RE::NiPoint3 Mid, ToMax, Rot;
			// default 100x100x40, h=10
			const float W_DIV2 = 50.0f, H_DIV2 = 40.0f, z0 = 10.0f;
			const RE::NiMatrix3 M = RE::NiMatrix3{ { W_DIV2, 0, 0 }, { 0, W_DIV2, 0 }, { 0, 0, H_DIV2 } };
			return { M * scale, RE::NiPoint3{ 0, 0, -z0 } };
		}
	};

	local_bounds_t get_refr_bounds(RE::TESObjectREFR* a);
	local_bounds_t get_npc_bounds(RE::TESObjectREFR* p);
}
using local_bounds_t = FiresStorage::local_bounds_t;
