
#include "gui/Gui.h"

#include "io/File.h"
#include "io/Folder.h"
#include "mock/MockAssetRegistry.h"
#include "script/ModuleRegistry.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const util::Folder folder = "test_file";
	const util::File file = folder.file("gui.xml");
}

namespace core::gui
{
	TEST_CLASS(GuiTest)
	{
	public:
		GuiTest() { initialize(); }
		~GuiTest() { deinitialize(); }

		TEST_METHOD(Gui_ctor)
		{
			Gui gui{ m_assets, m_modules, file };

			Assert::IsTrue(gui.has("foo"));
			Assert::IsFalse(gui.has("whatever"));
		}

		TEST_METHOD(Gui_setVisible)
		{
			Gui gui{ m_assets, m_modules, file };

			Assert::IsTrue(gui.isVisible("foo"));
			gui.setVisible("foo", false);
			Assert::IsFalse(gui.isVisible("foo"));

			Assert::IsFalse(gui.isVisible("whatever"));
		}
		TEST_METHOD(Gui_setLocked)
		{
			Gui gui{ m_assets, m_modules, file };

			Assert::IsFalse(gui.isLocked("foo"));
			gui.setLocked("foo", true);
			Assert::IsTrue(gui.isLocked("foo"));

			Assert::IsTrue(gui.isLocked("whatever"));
		}

	private:
		void initialize()
		{
			folder.create();
			file.write(R"(
				<widgets>
					<widget name="foo" />
					<widget name="bar" />
				</widgets>
			)");
		}
		void deinitialize()
		{
			folder.erase(true);
		}

		AssetRegistry m_assets = mockAssetRegistry();
		ModuleRegistry m_modules;
	};
}