
#include "GuiRegistry.h"

#include "gui/GuiLoader.h"

core::GuiRegistry::GuiRegistry(const AssetRegistry & assets, EventBus & bus, Scene & scene)
	: m_assets(assets), m_scene(scene)
{
	m_root = m_scene.createNode(Scene::DEFAULT_CAMERA);
}

bool core::GuiRegistry::open(const util::File & file)
{
	if (!file.exists() || m_guis.find(file) != m_guis.end())
		return false;

	auto gui = std::make_unique<Gui>(file.name());
	gui->load(file, m_assets);

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
