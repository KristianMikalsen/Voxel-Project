#pragma once

#include "gui/GuiData.h"

#include <string>

namespace core
{
	class Script;
	struct Widget;

	/**
		Base class for all types of widget actions. Typically all actions will execute some script
		when activated.
	*/
	class WidgetAction
	{
	protected:
		WidgetAction(const Script & script, const std::string & code) : m_script(script), m_code(code) {}

		const Script & m_script;
		const std::string m_code;
	};

	// ...

	/**
		Sliders are activated whenever the value they are storing is changed. This typically happens
		when the user is moving the glider or activating the 'increment' or 'decrement' buttons.
	*/
	class WidgetActionSliderBar : private WidgetAction
	{
	public:
		WidgetActionSliderBar(const Script & script, const std::string & code) : WidgetAction(script, code) {}

		void operator()(Widget & widget) const;
	};
	/**
		Slider come with an incrementer and decrementer button. Each activation of these buttons
		forces the slider value to update, while also respecting slider limits and step size.
	*/
	class WidgetActionSliderButton : private WidgetAction
	{
	public:
		WidgetActionSliderButton(Widget & widget, const Script & script, const std::string & code, const gui::SliderData & data, bool increment)
			: WidgetAction(script, code), m_widget(widget), m_data(data), m_increment(increment)
		{}

		void operator()(Widget & widget) const;

	private:
		Widget & m_widget;

		gui::SliderData m_data;
		bool m_increment;
	};
}