/* SIE CONFIDENTIAL
 * PlayStation(R)5 Programmer Tool Runtime Library Release 2.00.00.09-00.00.00.0.1
 * Copyright (C) 2019 Sony Interactive Entertainment Inc.
 */

#include "webbrowser_dialog_menu.h"
#include <kernel.h>
#include <sampleutil.h>
#include <user_service.h>
#include <web_browser_dialog.h>
#include <libsysmodule.h>

#include "application.h"

namespace
{
const uint32_t kDebugMenuPositionX = 50;
const uint32_t kDebugMenuPositionY = 50;

#define SAMPLE_URI							"https://www.playstation.com/country-selector/"

#define SAMPLE_CUSTOM_HEADER_WIDTH			(1920)
#define SAMPLE_CUSTOM_BODY_WIDTH			(1920)
#define SAMPLE_CUSTOM_BODY_HEIGHT			(963)
#define SAMPLE_CUSTOM_BODY_POS_Y			(117)

#define SAMPLE_CALLBACK_URI					"https://sample.siedev.net/webbrowser_callback_SDK.htm"
#define SAMPLE_CALLBACK_PREFIX				"testscheme:"
#define SAMPLE_CALLBACK_PREFIX_REGEXP		"tes.p[a-z]ram$"

#define SAMPLE_NPAUTH_URI					"psno://localhost/?response_type=code&client_id=10cd96b3-8c2e-46a4-93d3-10b509cc8f11&redirect_uri=https%3A%2F%2Fwww%2Eplaystation%2Ecom%2Fcountry%2Dselector%2F&scope=psn%3As2s&state=test.playstation"

#define SAMPLE_URI_PREDETERMINED			"https://sample.siedev.net/webbrowser_domain_check.htm"
#define SAMPLE_URI_PREDETERMINED_DOMAIN1	"sample.siedev.net"
#define SAMPLE_URI_PREDETERMINED_DOMAIN2	"www.playstation.com"

#define SAMPLE_COOKIE_URI					"https://sample.siedev.net/show_cookie_SDK.html"

	// String of Browser status
	static const char *kBrowserStateString[] =
	{
		"NONE",
		"INITIALIZED",
		"RUNNING",
		"FINISHED",
	};

	static WebBrowserDialog			*s_instance;
	static int						s_activeBrowser = -1;

	// Is Browser active ?
#define IS_BROWSER_ACTIVE	(s_activeBrowser >= 0)

	void openBrowser(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceWebBrowserDialogParam	param;
		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();

		// initalize webbrowser dialog
		switch (arg)
		{
		case 0:
			{
				strncpy(url, SAMPLE_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
				break;
			}
		case 4:
			{
				strncpy(url, SAMPLE_NPAUTH_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
				break;
			}
		default:
			break;
		}
		
		param.url = url;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	void openBrowserCustom(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceWebBrowserDialogParam	param;
		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_CUSTOM;
		param.userId = getPadCtrlUserId();

		// initalize webbrowser dialog
		strncpy(url, SAMPLE_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
		param.url = url;

		// setup custom param
		param.headerWidth	= SAMPLE_CUSTOM_HEADER_WIDTH;
		param.width			= SAMPLE_CUSTOM_BODY_WIDTH;
		param.height		= SAMPLE_CUSTOM_BODY_HEIGHT;
		param.positionY		= SAMPLE_CUSTOM_BODY_POS_Y;

		param.parts =	SCE_WEB_BROWSER_DIALOG_CUSTOM_PARTS_TITLE |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_PARTS_ADDRESS;

		param.control = SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_EXIT |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_RELOAD |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_BACK |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_FORWARD |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_ZOOM |
						SCE_WEB_BROWSER_DIALOG_CUSTOM_CONTROL_OPTION_MENU;;

		SceWebBrowserDialogWebViewParam	webview_param;
		memset(&webview_param, 0x0, sizeof(webview_param));
		webview_param.size = sizeof(SceWebBrowserDialogWebViewParam);
		webview_param.option = SCE_WEB_BROWSER_DIALOG_WEBVIEW_OPTION_BACKGROUND_TRANSPARENCY;

		param.webviewParam = &webview_param;

		param.animation = SCE_WEB_BROWSER_DIALOG_ANIMATION_DISABLE;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	void openBrowserWithCallbackParam(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceChar8					callback[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(callback, 0x0, sizeof(callback));

		SceWebBrowserDialogParam	param;
		SceWebBrowserDialogCallbackInitParam callback_param;
		memset(&callback_param, 0x00, sizeof(callback_param));

		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();
		
		// initalize webbrowser dialog
		strncpy(url, SAMPLE_CALLBACK_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);

		param.url = url;

		callback_param.size = sizeof(SceWebBrowserDialogCallbackInitParam);

		// initalize webbrowser dialog
		switch (arg)
		{
		case 2:
			{
				callback_param.type = SCE_WEB_BROWSER_DIALOG_CALLBACK_PARAM_TYPE_URL;
				strncpy(callback, SAMPLE_CALLBACK_PREFIX, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
				break;
			}
		case 3:
			{
				callback_param.type = SCE_WEB_BROWSER_DIALOG_CALLBACK_PARAM_TYPE_REGEXP;
				strncpy(callback, SAMPLE_CALLBACK_PREFIX_REGEXP, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
				break;
			}
		default:
			{
				break;
			}
		}

		callback_param.data = callback;
		param.callbackInitParam = &callback_param;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	void openBrowserImeParam(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceWebBrowserDialogParam	param;
		SceWebBrowserDialogImeParam	ime_param;
		memset(&ime_param, 0x00, sizeof(ime_param));

		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();

		// initialize ime parameter
		ime_param.size = sizeof(ime_param);
		ime_param.option = 
			SCE_WEB_BROWSER_DIALOG_IME_OPTION_NO_AUTO_CAPITALIZATION | 
			SCE_WEB_BROWSER_DIALOG_IME_OPTION_NO_LEARNING |
			SCE_WEB_BROWSER_DIALOG_IME_OPTION_DISABLE_COPY_PASTE |
			SCE_WEB_BROWSER_DIALOG_IME_OPTION_DISABLE_AUTO_SPACE;

		// initalize webbrowser dialog
		strncpy(url, SAMPLE_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);

		param.url = url;

		param.imeParam = &ime_param;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	void openBrowserForPredeterminedContent(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceWebBrowserDialogParam	param;
		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();

		// initalize webbrowser dialog
		strncpy(url, SAMPLE_URI_PREDETERMINED, SCE_WEB_BROWSER_DIALOG_URL_SIZE);
		param.url = url;

		SceWebBrowserDialogPredeterminedContentParam param2;
		memset(&param2, 0x0, sizeof(param2));
		param2.domain[0] = SAMPLE_URI_PREDETERMINED_DOMAIN1;
		param2.domain[1] = SAMPLE_URI_PREDETERMINED_DOMAIN2;
		param2.size = sizeof(SceWebBrowserDialogPredeterminedContentParam);

		ret = sceWebBrowserDialogOpenForPredeterminedContent(&param, &param2);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}


	void openBrowserUrlExtended(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE_EXTENDED];
		memset(url, 0x0, sizeof(url));

		SceChar8					callback[SCE_WEB_BROWSER_DIALOG_URL_SIZE_EXTENDED];
		memset(callback, 0x0, sizeof(callback));

		SceWebBrowserDialogParam	param;
		SceWebBrowserDialogCallbackInitParam callback_param;
		memset(&callback_param, 0x00, sizeof(callback_param));

		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();

		// initalize webbrowser dialog
		strncpy(url, SAMPLE_CALLBACK_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE_EXTENDED);

		param.url = url;

		strncpy(callback, SAMPLE_CALLBACK_PREFIX, SCE_WEB_BROWSER_DIALOG_URL_SIZE_EXTENDED);
		callback_param.size = sizeof(SceWebBrowserDialogCallbackInitParam);
		callback_param.type = SCE_WEB_BROWSER_DIALOG_CALLBACK_PARAM_TYPE_URL;
		callback_param.data = callback;
		param.callbackInitParam = &callback_param;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	void openBrowserSetCookie(uint64_t arg)
	{
		if (IS_BROWSER_ACTIVE)
		{
			return;
		}

		int ret = SCE_OK;

		ret = sceUserServiceInitialize(NULL);
		if (ret != SCE_OK && ret != SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
		{
			return;
		}

		SceChar8					url[SCE_WEB_BROWSER_DIALOG_URL_SIZE];
		memset(url, 0x0, sizeof(url));

		SceWebBrowserDialogParam	param;
		// initalize parameter of webbrowser dialog
		sceWebBrowserDialogParamInitialize(&param);
		param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
		param.userId = getPadCtrlUserId();

		// initialize cookie

		// clear cookie in temporary buffer
		SceWebBrowserDialogResetCookieParam reset_cookie;
		memset(&reset_cookie, 0x0, sizeof(reset_cookie));
		reset_cookie.size = sizeof(reset_cookie);

		ret = sceWebBrowserDialogResetCookie(&reset_cookie);

		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		// set cookie to temporary buffer
		for (int i = 0; i < SCE_WEB_BROWSER_DIALOG_COOKIE_COUNT; i++)
		{
			SceChar8	cookie_url[SCE_WEB_BROWSER_DIALOG_COOKIE_URL_SIZE];
			SceChar8	cookie_data[SCE_WEB_BROWSER_DIALOG_COOKIE_DATA_SIZE];
			memset(cookie_url, 0x0, sizeof(cookie_url));
			memset(cookie_data, 0x0, sizeof(cookie_data));

			SceWebBrowserDialogSetCookieParam set_cookie;
			memset(&set_cookie, 0x0, sizeof(set_cookie));

			strncpy(cookie_url, SAMPLE_COOKIE_URI, sizeof(SAMPLE_COOKIE_URI) - 1);

			snprintf(cookie_data, sizeof(cookie_data), "key%d=value%d", i, i);

			set_cookie.size = sizeof(set_cookie);
			set_cookie.url = cookie_url;
			set_cookie.cookie = cookie_data;

			ret = sceWebBrowserDialogSetCookie(&set_cookie);

			SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);
		}

		// initalize webbrowser dialog
		strncpy(url, SAMPLE_COOKIE_URI, SCE_WEB_BROWSER_DIALOG_URL_SIZE);

		param.url = url;

		ret = sceWebBrowserDialogOpen(&param);
		SCE_SAMPLE_UTIL_ASSERT(SCE_OK == ret);

		if (SCE_OK == ret)
		{
			s_activeBrowser = arg;
		}

		if (s_instance)
		{
			s_instance->setResult(ret);
		}
	}

	// Button action table
	static const ButtonData s_buttonTable[] =
	{
		{ "Default",				openBrowser },
		{ "Custom",					openBrowserCustom },
		{ "Callback",				openBrowserWithCallbackParam },
		{ "Callback RegExp",		openBrowserWithCallbackParam },
		{ "NpAuthCode",				openBrowser },
		{ "ImeParam",				openBrowserImeParam },
		{ "ForPredeterminedContent",openBrowserForPredeterminedContent },
		{ "UrlExtended",			openBrowserUrlExtended },
		{ "SetCookie",				openBrowserSetCookie }
	};

}


WebBrowserDialog::WebBrowserDialog()
	: m_result(0)
	, m_status(SCE_COMMON_DIALOG_STATUS_NONE)
{
	s_instance	= this;
	
	int	ret = 0;

	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_WEB_BROWSER_DIALOG);
	SCE_SAMPLE_UTIL_ASSERT( SCE_OK == ret );

	ret = sceWebBrowserDialogInitialize();
	SCE_SAMPLE_UTIL_ASSERT( SCE_OK == ret );

}

WebBrowserDialog::~WebBrowserDialog()
{
	int	ret = 0;

	ret	= sceWebBrowserDialogTerminate();
	SCE_SAMPLE_UTIL_ASSERT( SCE_OK == ret );

	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_WEB_BROWSER_DIALOG);
	SCE_SAMPLE_UTIL_ASSERT( SCE_OK == ret );
	
	s_instance	= nullptr;
}

void WebBrowserDialog::update()
{
	// Check WebBrowser state
	m_status = sceWebBrowserDialogGetStatus();

	if(IS_BROWSER_ACTIVE)
	{
		SceCommonDialogStatus status	= sceWebBrowserDialogUpdateStatus();
		
		if( SCE_COMMON_DIALOG_STATUS_FINISHED == status )
		{
			SceWebBrowserDialogResult	resultParam;
			memset( &resultParam, 0x00, sizeof(resultParam) );

			SceWebBrowserDialogCallbackResultParam	callbackParam;
			memset(&callbackParam, 0, sizeof(callbackParam));
			callbackParam.size = sizeof(SceWebBrowserDialogCallbackResultParam);
			resultParam.callbackResultParam = &callbackParam;

			int32_t	ret = sceWebBrowserDialogGetResult( &resultParam );
			if (SCE_OK == ret && SCE_WEB_BROWSER_DIALOG_RESULT_CALLBACK == resultParam.result)
			{
				// getting callback URL string over 2048 bytes
				if (SCE_COMMON_DIALOG_ERROR_PARAM_INVALID == ret && resultParam.callbackResultParam->bufferSize > 0)
				{
					resultParam.callbackResultParam->buffer = new char[resultParam.callbackResultParam->bufferSize]();
					ret = sceWebBrowserDialogGetResult(&resultParam);
					if (SCE_OK == ret)
					{
						PRINTF("callback buffer = %s\n", resultParam.callbackResultParam->buffer);
					}
					delete [] resultParam.callbackResultParam->buffer;
					resultParam.callbackResultParam->buffer = NULL;
				}
				PRINTF("callback type = %d\n", resultParam.callbackResultParam->type);
				PRINTF("callback data = %s\n", resultParam.callbackResultParam->data);
			}

			SCE_SAMPLE_UTIL_ASSERT( SCE_OK == ret );
		
			setResult( resultParam.result );
			
			// clear
			s_activeBrowser = -1;
		}
	}
}

void WebBrowserDialog::render()
{
	// window position
	const float kPosY = kDebugMenuPositionY * 2.f;
	float posX = kDebugMenuPositionX * 2.f;

	// Command window
	ImGui::SetNextWindowPos(ImVec2(posX, kPosY));
	ImGui::SetNextWindowSize(ImVec2(630.f, 800.0f));
	ImGui::SetNextWindowFocus();
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		ImGui::Text("Commands");
		ImGui::Separator();

		// Call WebBrowser
		for (int i = 0; i < IM_ARRAYSIZE(s_buttonTable); i++)
		{
			if (ImGui::Button(s_buttonTable[i].pLabel, ImVec2(620.0f, 64.0f)))
			{
				s_buttonTable[i].func(i);
			}
		}
		ImGui::Separator();
	}
	ImGui::End();

	// Result window
	posX += (630.f + 128.f);
	ImGui::SetNextWindowPos(ImVec2(posX, kPosY));
	ImGui::SetNextWindowSize(ImVec2(600.f, 260.f));
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 40.f);
	ImGui::Begin("Result", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Result");
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Indent();
		{
			ImGui::Text("Status   :  %s", kBrowserStateString[m_status]);
			if (m_result == SCE_OK)
			{	// Success
				ImGui::Text("Result   :  OK");
			}
			else
			{	// Error
				ImGui::Text("Result   :  0x%x", m_result);
			}
			ImGui::Text("Button Id:  %d", s_activeBrowser);
		}
		ImGui::Unindent();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void WebBrowserDialog::setResult( int32_t result )
{
	m_result = result;
}
