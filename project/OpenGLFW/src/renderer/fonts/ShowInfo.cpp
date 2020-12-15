/*************************************************
Author: Stan
Created: 2019-6-4
Note: for debug info display in Screen.
************************************************/
#include "ShowInfo.h"
#include "ShowInfoRender.h"

namespace show {

const char *SHOW_FONT_IMAGE_PATH = "res/imgs/fonts/ascii_con.tga";


ShowInfo *ShowInfo::s_instance = nullptr;

ShowInfo::ShowInfo()
	:_render(new ShowInfoRender)
{
	
}

ShowInfo::~ShowInfo()
{
	delete _render;
	_render = nullptr;
}

bool ShowInfo::init(float posXInScreen, float posYInScreen, float scale /*= 1.0f*/)
{
	if (_isInited)
	{
		return true;
	}

	_render->init(SHOW_FONT_IMAGE_PATH, '!', 8, 12, 1, 12, 12, 50);
	_render->setScale(scale);
	_render->setPosition(posXInScreen, posYInScreen);

	//test 
	//_render->setContent("fps: 59\nvertices: 101\ndraw call:3");


	_isInited = true;
	return true;
}

void ShowInfo::render()
{
	_render->render();
}

void ShowInfo::onScreenChangeSize(float w, float h)
{
	_render->onScreenChangeSize(w, h);
}

void ShowInfo::setContent(const std::string & content)
{
	_render->setContent(content);
}

void ShowInfo::setScale(float s)
{
	_render->setScale(s);
}

void ShowInfo::setPosition(float x, float y)
{
	_render->setPosition(x, y);
}

void ShowInfo::setAnchor(float x, float y)
{
	_render->setAnchor(x, y);
}

void ShowInfo::destroy()
{
	if (_isInited == false)
	{
		return;
	}

	_render->destroy();
	_isInited = false;
}


}
