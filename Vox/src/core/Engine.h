#pragma once

#include "io/Folder.h"

#include <memory>

namespace core
{
	class AssetRegistry;
	class Display;
	class ECS;
	class EventBus;
	class GuiRegistry;
	class ModuleRegistry;
	class Scene;
	class StateManager;
	class UBORegistry;
}
namespace vox
{
	class Universe;
}

namespace core
{
	class Engine
	{
	public:
		Engine();
		Engine(const Engine &) = delete;
		Engine(Engine &&) = delete;
		~Engine();

		Engine & operator=(const Engine &) = delete;
		Engine & operator=(Engine &&) = delete;

		void initialize();
		void start();
		void stop();

		// ...

		util::Folder getDataFolder() const;

		AssetRegistry & getAssets();
		Display & getDisplay();
		ECS & getECS();
		EventBus & getEventBus();
		GuiRegistry & getGuiRegistry();
		ModuleRegistry & getModules();
		Scene & getScene();
		StateManager & getStates();
		UBORegistry & getUBOs();

		vox::Universe & getUniverse();

	private:
		void process(double t, double dt);
		void render(float pf);

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}