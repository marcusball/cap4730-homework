#pragma once

class ControlInterceptor{
public:
	virtual bool KeyCallback(int key, int scancode, int action, int mods) = 0;
	virtual bool MouseCallback(int button, int action, int mods) = 0;
};