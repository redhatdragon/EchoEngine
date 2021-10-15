#pragma once

class SystemAI : public System {
public:
	virtual void init();
	virtual void run();
	virtual const char* getName() {
		return "SystemAI";
	}
} static systemAI;