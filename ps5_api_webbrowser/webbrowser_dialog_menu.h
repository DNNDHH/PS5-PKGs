#pragma once

#include <common_dialog/types.h>

class WebBrowserDialog
{
public:
	WebBrowserDialog();
	~WebBrowserDialog();
	void update();
	void render();
	
	void setResult( int32_t result );
private:
	int32_t					m_result;
	SceCommonDialogStatus	m_status;
};
