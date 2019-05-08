#pragma once

#include "gui/Widget.h"

#include <pugixml/pugixml.hpp>

namespace core
{
	/**
		The widget loader loads a widget and its entire family from a specific node. All widgets
		will be given their processors, renders, and listeners as well.
	*/
	class WidgetLoader
	{
	public:
		WidgetLoader() = delete;
		WidgetLoader(Widgets & widgets) : m_widgets(widgets) {}

		/**
			Loads the data stored in the node as a widget node. Will load child widgets as well, if
			any are specified in the node.

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void load(const pugi::xml_node & node, Widget & widget);

		/**
			Loads up core widget data such as name and type.
			<widget name="widget" type="panel" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadHeader(const pugi::xml_node & node, Widget & widget);
		/**
			Loads up the inner and outer border of a widget.
			<border inner="2" outer="3" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadBorder(const pugi::xml_node & node, Widget & widget);
		/**
			Loads up the minimum size the widget can have.
			<bbox size="40, 25" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadBoundingBox(const pugi::xml_node & node, Widget & widget);
		/**
			Loads up all children of the widget, and loads the childrens' children, recursively.
			<widget name="childA" type="type" />
			<widget name="childB" type="type" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
			...
		*/
		void loadChildren(const pugi::xml_node & node, Widget & widget);
		/**
			Loads a link between this widget and a different widget. The position of this widget
			will be specified in terms of the position of the link target position and the anchor
			point.
			<link target="widget" ratio="0.5, 1" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadLink(const pugi::xml_node & node, Widget & widget);
		/**
			Assigns a widget as a leader of the group. If the widget has a leader as well, this
			widget will be added to that group instead.
			<group leader="widget" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadGroup(const pugi::xml_node & node, Widget & widget);
		/**
			Loads up the state of the widget, whether it is visible, locked, and/or active.
			<state visible="true" locked="false" active="false" />

			@param node The xml node containing widget data.
			@param widget The widget which should be loaded.
		*/
		void loadState(const pugi::xml_node & node, Widget & widget);

	private:
		Widgets & m_widgets;
	};
}
