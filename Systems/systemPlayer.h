#pragma once

class SystemPlayer : public System {
public:
	virtual void run();
	virtual const char* getName() {
		return "SystemPlayer";
	}
} static systemPlayer;