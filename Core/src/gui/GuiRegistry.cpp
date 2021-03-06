
#include "GuiRegistry.h"

#include "event/Events.h"
#include "ui/Display.h"

#include <limits>

core::GuiRegistry::GuiRegistry(
	const AssetRegistry & assets, const ModuleRegistry & modules, const Display & display, 
	EventBus & bus, Scene & scene
) : m_assets(assets), m_modules(modules), m_display(display), m_scene(scene)
{
	constexpr auto priority = std::numeric_limits<int>::min();
	m_displayResize = bus.add<DisplayResize>([this](auto & event) { resizeGuis(event.m_size); });
	m_mouseMove = bus.add<MouseMove>(priority, [this](auto & event) { processEvent(event); });
	m_mousePress = bus.add<MousePress>(priority, [this](auto & event) { processEvent(event); });
	m_mouseRelease = bus.add<MouseRelease>(priority, [this](auto & event) { processEvent(event); });
	m_keyPress = bus.add<KeyPress>(priority, [this](auto & event) { processEvent(event); });
	m_keyRelease = bus.add<KeyRelease>(priority, [this](auto & event) { processEvent(event); });
	m_keyUnichar = bus.add<KeyUnichar>(priority, [this](auto & event) { processEvent(event); });

	m_root = m_scene.createNode(Scene::DEFAULT_CAMERA);
}

bool core::GuiRegistry::open(const util::File & file)
{
	if (!file.exists() || m_guis.find(file) != m_guis.end())
		return false;

	auto gui = std::make_unique<Gui>(m_assets, m_modules, file);
	gui->resize(m_display.getSize());
	auto node = m_scene.createRender("", "", [gui = gui.get()]() { gui->render(); }, m_root);
	m_scene.setFullscreenLayer(node, FullscreenLayer::GUI);

	m_guis[file] = std::move(gui);
	m_nodes[file] = node;
	return true;
}
bool core::GuiRegistry::close(const util::File & file)
{
	if (m_guis.find(file) == m_guis.end())
		return false;

	m_scene.detach(m_nodes[file], m_root);

	m_guis.erase(file);
	m_nodes.erase(file);
	return true;
}

void core::GuiRegistry::process()
{
	for (auto & [_, gui] : m_guis)
		gui->process();
}

void core::GuiRegistry::resizeGuis(const glm::vec2 & size)
{
	for (auto & [_, gui] : m_guis)
		gui->resize(size);
}
