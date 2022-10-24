#pragma once

class SystemDead : public System {
public:
	virtual void run();
	virtual const char* getName() {
		return "SystemDead";
	}
} static systemDead;