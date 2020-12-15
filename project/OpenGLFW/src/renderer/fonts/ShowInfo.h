/*************************************************
Author: Stan
Created: 2019-6-4
Note: for debug info display in Screen.
************************************************/
#pragma once
#include <string>

namespace show {

class ShowInfoRender;

class ShowInfo {
private:
	static ShowInfo *s_instance;
public:
	static ShowInfo *instance() {
		if (s_instance == nullptr)
		{
			s_instance = new ShowInfo();
		}

		return s_instance;
	}

	static void destroyInstance() {
		if (s_instance != nullptr)
		{
			s_instance->destroy();
			delete s_instance;
			s_instance = nullptr;
		}
	}

private:
	ShowInfo();
	~ShowInfo();

	void destroy();

public:
	bool init(float posXInScreen, float posYInScreen, float scale = 1.0f);
	void render();
	void onScreenChangeSize(float w, float h);

	void setContent(const std::string &content);
	void setScale(float s);
	void setPosition(float x, float y);
	void setAnchor(float x, float y);

private:
	ShowInfoRender *_render = nullptr;
	bool _isInited = false;
};


}

