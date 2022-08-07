extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef DEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef DEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

#include <UselessFenixUtils.h>
#include "Geom.h"
#include "FireStorage.h"
#include "Settings.h"
#include <array>

auto get_bound_vertexes(const global_bounds_t& bounds)
{
	RE::NiPoint3 n1{ bounds.Normals.entry[0][0], bounds.Normals.entry[1][0], bounds.Normals.entry[2][0] };
	RE::NiPoint3 n2{ bounds.Normals.entry[0][1], bounds.Normals.entry[1][1], bounds.Normals.entry[2][1] };
	RE::NiPoint3 n3{ bounds.Normals.entry[0][2], bounds.Normals.entry[1][2], bounds.Normals.entry[2][2] };
	return std::array<RE::NiPoint3, 8>{
		bounds.Base + n1 + n2 + n3,
		bounds.Base - n1 + n2 + n3,
		bounds.Base - n1 - n2 + n3,
		bounds.Base + n1 - n2 + n3,
		bounds.Base + n1 + n2 - n3,
		bounds.Base - n1 + n2 - n3,
		bounds.Base - n1 - n2 - n3,
		bounds.Base + n1 - n2 - n3
	};
}

template <glm::vec4 Color = Colors::RED>
void draw_bounds(const global_bounds_t& bounds, float update_period)
{
	auto verts = get_bound_vertexes(bounds);
	const int dur = static_cast<int>(update_period * 1000);
	const float wide = 5.0f;

	auto draw_ = [=](int u, int v) {
		draw_line<Color>(verts[u], verts[v], wide, dur);
	};

	draw_(0, 1);
	draw_(0, 3);
	draw_(2, 1);
	draw_(2, 3);

	draw_(4, 5);
	draw_(4, 7);
	draw_(6, 5);
	draw_(6, 7);

	draw_(0, 4);
	draw_(1, 5);
	draw_(2, 6);
	draw_(3, 7);
}

template <glm::vec4 Color = Colors::RED>
void draw([[maybe_unused]] RE::Actor* a, [[maybe_unused]] RE::TESObjectREFR* refr, [[maybe_unused]] float update_period)
{
#ifdef DEBUG
	//draw_line<Color>(FiresStorage::get_bounds_center(refr), a->GetPosition(), 5.0f, static_cast<int>(update_period) * 1000);
	draw_line<Color>(refr->GetPosition(), a->GetPosition(), 5.0f, static_cast<int>(update_period) * 1000);
	draw_bounds<Color>(get_refr_bounds(refr), update_period);
#endif  // DEBUG
}

class PlayerCharacterHook
{
public:
	static void Hook()
	{
		_Update = REL::Relocation<uintptr_t>(REL::ID(RE::VTABLE_PlayerCharacter[0])).write_vfunc(0xad, Update);
	}

private:
	static void Update(RE::PlayerCharacter* a, float delta) {
		_Update(a, delta);

		auto player = RE::PlayerCharacter::GetSingleton();
		//auto player_pos = player->GetPosition();
		//auto player_bounds = player->GetBaseObject();
		draw_bounds(get_refr_bounds(player), 0.0f);
		RE::TES::GetSingleton()->ForEachReference([](RE::TESObjectREFR& refr) {
			if (!refr.IsDisabled() && refr.GetBaseObject() && refr.GetBaseObject()->formID == 0x17) {
				draw_bounds(get_refr_bounds(&refr), 0.0f);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		});

		DebugAPI_IMPL::DebugAPI::Update();
	}

	static inline REL::Relocation<decltype(Update)> _Update;
};

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		PlayerCharacterHook::Hook();

		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	if (!g_messaging) {
		logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 10);

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	return true;
}
