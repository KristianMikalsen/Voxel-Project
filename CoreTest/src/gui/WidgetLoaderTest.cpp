
#include "gui/WidgetLoader.h"

#include "event/EventBus.h"
#include "gui/Widget.h"
#include "mock/MockAssetRegistry.h"
#include "script/Script.h"

#include <pugixml/pugixml.hpp>

#include "Common.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace core::gui
{
	TEST_CLASS(WidgetLoaderTest)
	{
	public:
		TEST_METHOD(WidgetLoader_loadName)
		{
			auto parent = addWidget(m_root, "parent", "panel");
			auto childA = addWidget(parent, "childA", "panel");
			auto childB = addWidget(parent, "childB", "button");
			auto childC = addWidget(childA, "childC", "button");
			addWidget(m_root, "", "button"); // Uses default name when no name is specified
			addWidget(m_root, "parent", "button"); // Cannot load duplicated names

			Widget root;
			m_loader.load(m_root, root);
			Assert::AreEqual(2u, root.m_family.m_children.size());
			
			auto & p = root.m_family.m_children[0];
			auto & a = p->m_family.m_children[0];
			auto & b = p->m_family.m_children[1];
			auto & c = a->m_family.m_children[0];
			auto & nameless = root.m_family.m_children[1];

			Assert::AreEqual({ "root.parent" }, p->m_name);
			Assert::AreEqual({ "root.parent.childA" }, a->m_name);
			Assert::AreEqual({ "root.parent.childB" }, b->m_name);
			Assert::AreEqual({ "root.parent.childA.childC" }, c->m_name);
			Assert::AreEqual({ "__widget_5" }, nameless->m_name);
		}

		TEST_METHOD(WidgetLoader_loadBorder)
		{
			Widget::Border border;
			border.m_inner = 2.0f;
			border.m_outer = 3.0f;

			Widget widget;
			m_loader.loadBorder(addBorder(m_doc, border), widget);

			Assert::AreEqual(border.m_inner, widget.m_border.m_inner);
			Assert::AreEqual(border.m_outer, widget.m_border.m_outer);
		}
		TEST_METHOD(WidgetLoader_loadBoundingBox)
		{
			Widget::BoundingBox bbox;
			bbox.m_minSize = { 40.0f, 25.2f };

			Widget widget;
			m_loader.loadBoundingBox(addBoundingBox(m_doc, bbox), widget);

			Assert::AreEqual(bbox.m_pos, widget.m_bbox.m_pos);
			Assert::AreEqual(bbox.m_size, widget.m_bbox.m_size);
			Assert::AreEqual(bbox.m_minSize, widget.m_bbox.m_minSize);
		}
		TEST_METHOD(WidgetLoader_loadGroup)
		{
			Widget root;
			addWidget(m_root, "leader", "panel");
			addWidget(m_root, "widget", "panel");
			m_loader.load(m_root, root);

			auto & leader = *root.m_family.m_children[0];
			auto & widget = *root.m_family.m_children[1];
			m_loader.loadGroup(addGroup(m_doc, "leader"), widget);

			Assert::IsTrue(&leader == widget.m_group.m_leader);
			Assert::IsTrue(&leader == leader.m_group.m_leader);
			Assert::AreEqual(0u, widget.m_group.m_members.size());
			Assert::AreEqual(2u, leader.m_group.m_members.size());
		}
		TEST_METHOD(WidgetLoader_loadLink)
		{
			Widget root;
			addWidget(m_root, "target", "panel");
			addWidget(m_root, "widget", "panel");
			m_loader.load(m_root, root);

			Widget::Link link;
			link.m_target = root.m_family.m_children[0].get();
			link.m_ratio = { 0.75f, 1.0f };

			auto & widget = *root.m_family.m_children[1];
			m_loader.loadLink(addLink(m_doc, "target", link.m_ratio), widget);

			Assert::IsTrue(link.m_target == widget.m_link.m_target);
			Assert::AreEqual(link.m_ratio, widget.m_link.m_ratio);
		}
		TEST_METHOD(WidgetLoader_loadState)
		{
			Widget::State state;
			state.m_visible = false;
			state.m_locked = true;

			Widget widget;
			m_loader.loadState(addState(m_doc, state), widget);

			Assert::AreEqual(state.m_visible, widget.m_state.m_visible);
			Assert::AreEqual(state.m_locked, widget.m_state.m_locked);
		}

		TEST_METHOD(WidgetLoader_loadDefaultValues)
		{
			auto node = m_doc.append_child("widget");
			
			Widget widget;
			m_loader.load(node, widget);

			Assert::AreEqual(0.0f, widget.m_border.m_inner);
			Assert::AreEqual(0.0f, widget.m_border.m_outer);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_bbox.m_pos);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_bbox.m_size);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_bbox.m_minSize);
			Assert::IsNull(widget.m_link.m_target);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_link.m_ratio);
			Assert::IsTrue(&widget == widget.m_group.m_leader);
			Assert::IsTrue(widget.m_state.m_visible);
			Assert::IsFalse(widget.m_state.m_locked);
		}
		TEST_METHOD(WidgetLoader_loadSpecialValues)
		{
			auto node = m_doc.append_child("widget");
			node.append_child("border").append_attribute("outer").set_value("2");
			node.append_child("bbox").append_attribute("size").set_value("40, 80");
			node.append_child("group").append_attribute("leader").set_value("other");
			node.append_child("link").append_attribute("target").set_value("other");

			Widget widget, other;
			m_loader.load(addWidget(m_doc, "other", "panel"), other);
			m_loader.load(node, widget);

			Assert::AreEqual(0.0f, widget.m_border.m_inner);
			Assert::AreEqual(2.0f, widget.m_border.m_outer);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_bbox.m_pos);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_bbox.m_size);
			Assert::AreEqual({ 40.0f, 80.0f }, widget.m_bbox.m_minSize);
			Assert::IsTrue(&other == widget.m_link.m_target);
			Assert::AreEqual({ 0.0f, 0.0f }, widget.m_link.m_ratio);
			Assert::IsTrue(&other == widget.m_group.m_leader);
			Assert::IsTrue(widget.m_state.m_visible);
			Assert::IsFalse(widget.m_state.m_locked);
		}

		TEST_METHOD(WidgetLoader_loadChild)
		{
			addWidget(m_root, "childA", "panel");
			addWidget(m_root, "childB", "panel");

			Widget widget;
			m_loader.loadChildren(m_root, widget);

			Assert::AreEqual(2u, widget.m_family.m_children.size());
		}
		TEST_METHOD(WidgetLoader_loadWidgetHierarchy)
		{
			auto parent = addWidget(m_doc, "parent", "panel");
			auto childA = addWidget(parent, "childA", "panel");
			auto childB = addWidget(parent, "childB", "button");
			auto childC = addWidget(childA, "childC", "button");

			Widget p;
			m_loader.load(parent, p);

			auto & a = p.m_family.m_children[0];
			auto & b = p.m_family.m_children[1];
			auto & c = a->m_family.m_children[0];

			Assert::AreEqual(2u, p.m_family.m_children.size());
			Assert::AreEqual(1u, a->m_family.m_children.size());
			Assert::AreEqual(0u, b->m_family.m_children.size());
			Assert::AreEqual(0u, c->m_family.m_children.size());

			Assert::IsNull(p.m_family.m_parent);
			Assert::IsTrue(&p == a->m_family.m_parent);
			Assert::IsTrue(&p == b->m_family.m_parent);
			Assert::IsTrue(a.get() == c->m_family.m_parent);
		}

		// ...

		TEST_METHOD(WidgetLoader_loadButton)
		{
			Widget widget;
			m_loader.loadButton(addButton(m_doc), widget);

			Assert::IsFalse(widget.m_handler == nullptr);
			Assert::IsFalse(widget.m_renderer == nullptr);
			Assert::IsFalse(widget.m_processor == nullptr);
		}
		TEST_METHOD(WidgetLoader_loadSlider)
		{
			Widget widget;
			m_loader.loadSlider(addSlider(m_doc), widget);

			Assert::IsFalse(widget.m_handler == nullptr);
			Assert::IsTrue(widget.m_renderer == nullptr);
			Assert::IsTrue(widget.m_processor == nullptr);
			Assert::AreEqual(4u, widget.m_family.m_children.size());
		}
		TEST_METHOD(WidgetLoader_loadLabel)
		{
			Widget widgetA, widgetB;
			m_loader.loadLabel(addLabel(m_doc, false), widgetA);
			m_loader.loadLabel(addLabel(m_doc, true), widgetB);

			Assert::IsTrue(widgetA.m_handler == nullptr);
			Assert::IsFalse(widgetA.m_renderer == nullptr);
			Assert::IsTrue(widgetA.m_processor == nullptr);

			Assert::IsFalse(widgetB.m_handler == nullptr);
			Assert::IsFalse(widgetB.m_renderer == nullptr);
			Assert::IsFalse(widgetB.m_processor == nullptr);
		}
		TEST_METHOD(WidgetLoader_loadPanel)
		{
			Widget widget;
			m_loader.loadPanel(addPanel(m_doc), widget);

			Assert::IsTrue(widget.m_handler == nullptr);
			Assert::IsFalse(widget.m_renderer == nullptr);
			Assert::IsTrue(widget.m_processor == nullptr);
		}

	private:
		pugi::xml_node addBorder(pugi::xml_node & widget, const Widget::Border & border)
		{
			auto node = widget.append_child("border");
			node.append_attribute("inner").set_value(border.m_inner);
			node.append_attribute("outer").set_value(border.m_outer);
			return node;
		}
		pugi::xml_node addBoundingBox(pugi::xml_node & widget, const Widget::BoundingBox & bbox)
		{
			const auto str = std::to_string(bbox.m_minSize.x) + ", " + std::to_string(bbox.m_minSize.y);
			auto node = widget.append_child("bbox");
			node.append_attribute("size").set_value(str.c_str());
			return node;
		}
		pugi::xml_node addGroup(pugi::xml_node & widget, const std::string & name)
		{
			auto node = widget.append_child("group");
			node.append_attribute("leader").set_value(name.c_str());
			return node;
		}
		pugi::xml_node addLink(pugi::xml_node & widget, const std::string & name, const glm::vec2 & ratio)
		{
			const auto str = std::to_string(ratio.x) + ", " + std::to_string(ratio.y);
			auto node = widget.append_child("link");
			node.append_attribute("target").set_value(name.c_str());
			node.append_attribute("ratio").set_value(str.c_str());
			return node;
		}
		pugi::xml_node addState(pugi::xml_node & widget, const Widget::State & state)
		{
			auto node = widget.append_child("state");
			node.append_attribute("visible").set_value(state.m_visible);
			node.append_attribute("locked").set_value(state.m_locked);
			return node;
		}
		pugi::xml_node addWidget(pugi::xml_node & widget, const std::string & name, const std::string & type)
		{
			auto node = widget.append_child("widget");
			if (!name.empty())
				node.append_attribute("name").set_value(name.c_str());
			if (!type.empty())
				node.append_attribute("type").set_value(type.c_str());
			return node;
		}
		
		pugi::xml_node addButton(pugi::xml_node & widget)
		{
			return widget.append_child("button");
		}
		pugi::xml_node addSlider(pugi::xml_node & widget)
		{
			return widget.append_child("slider");
		}
		pugi::xml_node addLabel(pugi::xml_node & widget, bool dynamic)
		{
			auto node = widget.append_child("label");
			node.append_attribute("mutable").set_value(dynamic);
			return node;
		}
		pugi::xml_node addPanel(pugi::xml_node & widget)
		{
			return widget.append_child("panel");
		}

		// ...

		pugi::xml_document m_doc;
		pugi::xml_node m_root = addWidget(m_doc, "root", "panel");

		AssetRegistry m_assets = mockAssetRegistry();
		EventBus m_bus;
		Script m_script{ "script" };
		WidgetLoader m_loader{ m_assets, m_script, m_bus };
	};
}