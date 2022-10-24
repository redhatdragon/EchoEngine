#pragma once

class SystemDisplay : public System {
public:
	virtual void run();
	virtual const char* getName() {
		return "SystemDisplay";
	}
} static systemDisplay;