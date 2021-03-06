#pragma once

#include <limits>
#include <typeindex>
#include <typeinfo>

namespace core
{
	class EventBus;

	class Listener
	{
	public:
		Listener() = default;
		Listener(
			EventBus & bus, const std::type_index & type,
			unsigned int id, int priority, bool monitor
		) noexcept;
		Listener(const Listener &) = delete;
		Listener(Listener && other) noexcept;
		~Listener() noexcept;

		Listener & operator=(const Listener &) = delete;
		Listener & operator=(Listener && other) noexcept;

		inline auto type() const { return m_type; }
		inline auto id() const { return m_id; }
		inline auto priority() const { return m_priority; }
		inline auto monitor() const { return m_monitor; }

	private:
		EventBus * m_bus = nullptr;

		std::type_index m_type = typeid(void);
		unsigned int m_id = 0;
		int m_priority = 0;
		bool m_monitor = false;
	};
}