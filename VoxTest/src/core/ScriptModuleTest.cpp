
#include "core/ScriptModules.h"

#include "core/Engine.h"
#include "gui/GuiRegistry.h"
#include "mock/MockAssetRegistry.h"
#include "mock/MockUBORegistry.h"
#include "script/ModuleRegistry.h"
#include "script/Script.h"
#include "state/State.h"
#include "state/StateManager.h"

#include <functional>

#include "Context.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	/**
		The mock state accepts a callback which should be invoked every tick while the state is
		active. If the specified number of cycles ticks by without the callback terminating the
		engine, the test is considered failed.
	*/
	class MockState : public core::State
	{
	public:
		MockState(int cycles, const std::function<void()> & callback) : m_cycles(cycles), m_callback(callback) {}

		virtual void initialize(core::Engine & engine) override final {}
		virtual void process() override final { if (m_cycles-- <= 0) Assert::Fail(); m_callback(); }

	private:
		int m_cycles;

		std::function<void()> m_callback;
	};
}

namespace core::setup
{
	TEST_CLASS(ScriptModuleTest)
	{
	public:
		TEST_METHOD(ScriptModule_initializeEngine)
		{
			Engine engine;
			engine.initialize();
			engine.getStates().set<MockState>(1, [this]() { m_script.execute("ENGINE.stop()"); });

			script::initializeEngine(m_script, engine);

			engine.start();
		}

		TEST_METHOD(ScriptModule_initializeGui)
		{
			Gui gui{ m_assets, m_modules, "" };

			script::initializeGui(m_script);
			script::detail::bindGui(m_script, gui);

			Assert::IsTrue(m_script.execute(R"( GUI.has("widget") )"));

			Assert::IsTrue(m_script.execute(R"( GUI.isVisible("widget") )"));
			Assert::IsTrue(m_script.execute(R"( GUI.isLocked("widget") )"));
			Assert::IsTrue(m_script.execute(R"( GUI.setVisible("widget", true) )"));
			Assert::IsTrue(m_script.execute(R"( GUI.setLocked("widget", true) )"));

			Assert::IsTrue(m_script.execute(R"( GUI.getBool("widget") )"));
			Assert::IsTrue(m_script.execute(R"( GUI.getFloat("widget") )"));
			Assert::IsTrue(m_script.execute(R"( GUI.getString("widget") )"));
			Assert::IsTrue(m_script.execute(R"( GUI.setBool("widget", true) )"));
			Assert::IsTrue(m_script.execute(R"( GUI.setFloat("widget", 0.0f) )"));
			Assert::IsTrue(m_script.execute(R"( GUI.setString("widget", "") )"));
		}
		TEST_METHOD(ScriptModule_initializeGuiRegistry)
		{
			GuiRegistry guis{ m_assets, m_modules, display(), m_bus, m_scene };

			script::initializeGuiRegistry(m_script, guis);
			script::initializeFileSystem(m_script);

			Assert::IsTrue(m_script.execute(R"( GUI_REGISTRY.open(File("foobar")) )"));
			Assert::IsTrue(m_script.execute(R"( GUI_REGISTRY.close(File("foobar")) )"));
		}

		TEST_METHOD(ScriptModule_initializeFileSystem)
		{
			script::initializeFileSystem(m_script);

			Assert::IsTrue(m_script.execute(R"( var fileA = File() )"));
			Assert::IsTrue(m_script.execute(R"( var fileB = File("foobar") )"));
			Assert::IsTrue(m_script.execute(R"( var folder = Folder("foobar") )"));
		}

	private:
		AssetRegistry m_assets = mockAssetRegistry();
		ModuleRegistry m_modules;
		UBORegistry m_ubos = mockUBORegistry();
		EventBus m_bus;
		Scene m_scene{ m_assets, display(), m_ubos };

		Script m_script{ "script" };
	};
}