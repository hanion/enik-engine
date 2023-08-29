#pragma once
#include <base.h>

#include <functional>

namespace Enik {

class UUID {
public:
	UUID();
	UUID(uint64_t uuid);
	UUID(const UUID&) = default;

	operator uint64_t() const { return m_UUID; }

private:
	uint64_t m_UUID;
};

}

namespace std {
template <>
struct hash<Enik::UUID> {
	size_t operator()(const Enik::UUID& uuid) const {
		return (uint64_t)uuid;
	}
};
}