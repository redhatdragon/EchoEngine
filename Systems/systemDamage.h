#pragma once

class SystemDamage : public System {
public:
	virtual void run();
	virtual const char* getName() {
		return "SystemDamage";
	}
} static systemDamage;