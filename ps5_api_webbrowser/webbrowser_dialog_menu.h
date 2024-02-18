/* SIE CONFIDENTIAL
 * PlayStation(R)5 Programmer Tool Runtime Library Release 2.00.00.09-00.00.00.0.1
 * Copyright (C) 2019 Sony Interactive Entertainment Inc.
 */
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
