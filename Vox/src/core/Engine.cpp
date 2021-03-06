
#include "Engine.h"

#include "allegro/Allegro.h"
#include "allegro/EventSource.h"
#include "asset/AssetRegistry.h"
#include "core/MainLoop.h"
#include "core/Resources.h"
#include "core/Setup.h"
#include "ecs/ECS.h"
#include "event/EventBus.h"
#include "event/EventQueue.h"
#include "gui/GuiRegistry.h"
#include "opengl/UBORegistry.h"
#include "scene/Scene.h"
#include "script/ModuleRegistry.h"
#include "state/StateManager.h"
#include "ui/Display.h"

#include "world/Universe.h"

#include "world/Query.h"

struct core::Engine::Impl
{
	Impl() = delete;
	Impl(Engine & engine);
	Impl(const Impl &) = delete;
	Impl(Impl &&) = delete;
	~Impl() = default;

	Impl & operator=(const Impl &) = delete;
	Impl & operator=(Impl &&) = delete;

	// ...

	Allegro m_allegro;
	Display m_display;
	UBORegistry m_ubos;

	AssetRegistry m_assets;
	MainLoop m_loop;
	EventBus m_bus;
	EventQueue m_queue;
	ModuleRegistry m_modules;
	StateManager m_states;
	ECS m_ecs;
	Scene m_scene;
	GuiRegistry m_guis;

	vox::Universe m_universe;
};

core::Engine::Impl::Impl(Engine & engine) :
	m_allegro(),
	m_display({ 640u, 480u }, false),
	m_ubos(),
	m_assets(),
	m_loop([&](auto t, auto dt) { engine.process(t, dt); }, [&](float pf) { engine.render(pf); }),
	m_bus(),
	m_queue(m_bus),
	m_modules(),
	m_states(engine),
	m_ecs(),
	m_scene(m_assets, m_display, m_ubos),
	m_guis(m_assets, m_modules, m_display, m_bus, m_scene),
	m_universe(engine.getDataFolder().folder(res::path::UNIVERSE), m_scene)
{
	m_queue.add(getDisplayEventSource(m_display));
	m_queue.add(getKeyboardEventSource());
	m_queue.add(getMouseEventSource());
}

// ...

core::Engine::Engine() : m_impl(std::make_unique<Impl>(*this)) {}
core::Engine::~Engine() = default;

void core::Engine::initialize()
{
	setupAssets(getAssets(), getDataFolder(), getUBOs());
	setupComponents(getECS());
	setupSystems(getECS());
	setupUBOs(getUBOs());
	setupModules(getModules(), *this);

	m_impl->m_universe.prepare(getAssets());
}
void core::Engine::start()
{
	m_impl->m_loop.start(60.0, 60.0);
}
void core::Engine::stop()
{
	m_impl->m_loop.stop();
}

void core::Engine::process(double t, double dt)
{
	m_impl->m_scene.process();
	m_impl->m_queue.process();
	m_impl->m_states.process();
	m_impl->m_ecs.process(t, dt);
	m_impl->m_universe.process();
	m_impl->m_guis.process();
}
void core::Engine::render(float pf)
{
	m_impl->m_scene.render(pf);
}

util::Folder core::Engine::getDataFolder() const { return "data"; }

core::AssetRegistry & core::Engine::getAssets() { return m_impl->m_assets; }
core::Display & core::Engine::getDisplay() { return m_impl->m_display; }
core::ECS & core::Engine::getECS() { return m_impl->m_ecs; }
core::EventBus & core::Engine::getEventBus() { return m_impl->m_bus; }
core::GuiRegistry & core::Engine::getGuiRegistry() { return m_impl->m_guis; }
core::ModuleRegistry & core::Engine::getModules() { return m_impl->m_modules; }
core::Scene & core::Engine::getScene() { return m_impl->m_scene; }
core::StateManager & core::Engine::getStates() { return m_impl->m_states; }
core::UBORegistry & core::Engine::getUBOs() { return m_impl->m_ubos; }

vox::Universe & core::Engine::getUniverse() { return m_impl->m_universe; }
