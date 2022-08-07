#include "FireStorage.h"
#include <filesystem>

namespace FiresStorage
{
	Storage fires_storage;

	local_bounds_t get_refr_bounds(RE::TESObjectREFR* a, float scale)
	{
		return fires_storage.get_refr_bounds(a, scale);
	}

	void rotateSkyrim_M_Player(RE::NiMatrix3& ans, RE::TESObjectREFR* p)
	{
		auto rot = RE::NiPoint3(0,0, p->GetAngleZ());
		RE::NiMatrix3 m;
		m.EulerAnglesToAxesZXY(rot);
		ans = m * ans;
	}

	local_bounds_t get_refr_bounds(RE::TESObjectREFR* a)
	{
		auto ans = get_refr_bounds(a, a->GetScale());

		if (auto bone = a->Get3D()) {
			ans.Normals = bone->world.rotate * ans.Normals;
			ans.Base = bone->world.rotate * ans.Base;
		}

		return ans;
	}

	local_bounds_t get_npc_bounds(RE::TESObjectREFR* p)
	{
		constexpr float DOWN = 5.0f;
		const RE::NiMatrix3 PLAYER_BOUNDS{ { 25.0f, 0, 0 }, { 0, 17.0f, 0 }, { 0, 0, 60.0f + DOWN } };
		auto scale = p->GetScale();
		local_bounds_t ans;
		ans.Base = RE::NiPoint3{ 0, 0, PLAYER_BOUNDS.entry[2][2] - DOWN } * scale;
		ans.Normals = PLAYER_BOUNDS * scale;
		rotateSkyrim_M_Player(ans.Normals, p);
		return ans;
	}
}
