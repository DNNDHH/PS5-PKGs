/* SIE CONFIDENTIAL
 * PlayStation(R)5 Programmer Tool Runtime Library Release 2.00.00.09-00.00.00.0.1
 * Copyright (C) 2019 Sony Interactive Entertainment Inc.
 */

#include <stdio.h>
#include <vectormath.h>
#include <vector>
#include <string>
#include <sampleutil.h>
#include <common_dialog.h>
#include <np.h>
#include "application.h"
#include "webbrowser_dialog_menu.h"

size_t sceLibcHeapSize = 1024*1024*16;

namespace
{
struct UserInfo
{
	sce::SampleUtil::System::UserId		m_userId;
	std::unique_ptr<sce::SampleUtil::Input::PadContext>	m_padContext;
	std::unique_ptr<sce::SampleUtil::Input::OskContext>	m_oskContext;
	
	UserInfo()
		: m_userId( sce::SampleUtil::System::kInvalidUserId )
		, m_padContext( nullptr ), m_oskContext(nullptr)
	{
	}
};

std::vector<UserInfo>										s_userInfoList;
sce::SampleUtil::System::UserId								s_padCtrlUserId	= sce::SampleUtil::System::kInvalidUserId;
sce::SampleUtil::System::UserIdManager*						s_userIdManager	= nullptr;

std::unique_ptr<WebBrowserDialog>							s_webBrowserDialog;
std::unique_ptr<sce::SampleUtil::Input::KeyboardContext>	s_keyboardContext;
}

class Application : public sce::SampleUtil::SampleSkeleton
{

public:

	virtual int initialize(void)
	{
		int ret=0;
		(void)ret;

		//E Initialize SampleUtil. All SampleUtil features are enabled by FUNCTION_FLAG_ALL.
		//J SampleUtilの初期化。FUNCTION_FLAG_ALLを指定しSampleUtilの機能を有効化。
		sce::SampleUtil::Thread::JobQueueOption jobQueueOpt;
		jobQueueOpt.m_stackSize = 128 * 1024;
		SampleSkeletonOption skeletonOpt;
		skeletonOpt.jobQueueOption = &jobQueueOpt;
		ret = initializeUtil(kFunctionFlagGraphics | kFunctionFlagImGui | kFunctionFlagJobQueue, -1, -1, &skeletonOpt);
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	
		ret = sceUserServiceInitialize(nullptr);
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		ret = sceCommonDialogInitialize();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		
		s_webBrowserDialog = std::make_unique<WebBrowserDialog>();
		s_keyboardContext	= std::make_unique<sce::SampleUtil::Input::KeyboardContext>(SCE_USER_SERVICE_USER_ID_EVERYONE);
		
		return SCE_OK;
	}

	//E Update the application logic. Call updateUtil() at the beginning.
	//J アプリケーションのロジックの更新を行います。updateUtil()は冒頭で必ず呼び出します。
	virtual int update(void)
	{
		int ret=0;
		(void)ret;
		
		SceUserServiceLoginUserIdList		userList;

		if( SCE_OK == sceUserServiceGetLoginUserIdList(&userList) )
		{
			for( auto infoIt = s_userInfoList.begin(); infoIt != s_userInfoList.end(); )
			{
				bool	bHit	= false;
				
				for( size_t idCnt = 0; idCnt < SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++idCnt )
				{
					if( infoIt->m_userId == userList.userId[ idCnt ] )
					{
						bHit	= true;
						break;
					}
				}
				
				if( !bHit )
				{
					const auto deletedUserId = infoIt->m_userId;
					 
					infoIt->m_padContext.reset();
					infoIt->m_oskContext.reset();
					infoIt	= s_userInfoList.erase( infoIt );

					if (deletedUserId == getPadCtrlUserId())
					{
						if (infoIt != s_userInfoList.cend())
						{
							setPadCtrlUserId(infoIt->m_userId);
						}
						else
						{
							setPadCtrlUserId(sce::SampleUtil::System::kInvalidUserId);
						}
					}
				}else
				{
					 ++infoIt;
				}
			}
			
			for( size_t idCnt = 0; idCnt < SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++idCnt )
			{
				if( SCE_SAMPLE_UTIL_USER_ID_INVALID != userList.userId[ idCnt ] )
				{
					bool	bHit	= false;
					
					for( auto infoIt = s_userInfoList.begin(); infoIt != s_userInfoList.end();++infoIt )
					{
						if( userList.userId[ idCnt ] == infoIt->m_userId )
						{
							bHit	= true;
							break;
						}
					}
					
					if( !bHit )
					{
						UserInfo							userInfo;
						
						userInfo.m_userId	= userList.userId[ idCnt ];
						
						userInfo.m_padContext = std::make_unique<sce::SampleUtil::Input::PadContext>( userList.userId[ idCnt ],
																	  0,
																	  0,
																	  nullptr );
						userInfo.m_oskContext = std::make_unique<sce::SampleUtil::Input::OskContext>( userList.userId[ idCnt ] );
						
						s_userInfoList.push_back( std::move(userInfo) );

						if ( sce::SampleUtil::System::kInvalidUserId == getPadCtrlUserId() )
						{
							setPadCtrlUserId( userList.userId[ idCnt ] );
						}
					}
				}
			}
			
			for( auto infoIt = s_userInfoList.begin(); infoIt != s_userInfoList.end(); ++infoIt)
			{
				infoIt->m_padContext->update();
			}
		}
		
		sce::SampleUtil::Input::PadContext *padContext = nullptr;
		sce::SampleUtil::Input::OskContext *oskContext = nullptr;
		sce::SampleUtil::Input::KeyboardContext *keyboardContext = s_keyboardContext.get();
		
		{
			for( auto infoIt = s_userInfoList.begin(); infoIt != s_userInfoList.end(); ++infoIt)
			{
				if( s_padCtrlUserId == infoIt->m_userId )
				{
					padContext	= infoIt->m_padContext.get();
					oskContext	= infoIt->m_oskContext.get();
					break;
				}
			}

		}
		
		sce::SampleUtil::UIFramework::ImGuiUpdateParam imguiParam = { padContext, keyboardContext, oskContext };
		SampleSkeletonUpdateParam param = { &imguiParam };

		//E Update SampleUtil
		//J SampleUtilの更新
		ret = updateUtil(&param);
		if(ret != SCE_OK){
			return ret;
		}
		
		s_webBrowserDialog->update();
		
		return ret;
	}	

	virtual void render(void)
	{
		getGraphicsContext()->beginScene(getGraphicsContext()->getNextRenderTarget(),
			                             getGraphicsContext()->getDepthStencilSurface());
		{
			getGraphicsContext()->clearRenderTarget(0x00000000);

			s_webBrowserDialog->render();
		}

		int ret = SCE_OK;
		ret = renderUtil();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

		getGraphicsContext()->endScene();
		getGraphicsContext()->flip(1);
	}

	virtual int finalize(void)
	{
		for ( auto iterator = s_userInfoList.begin() ; iterator != s_userInfoList.cend() ; )
		{
			iterator->m_padContext.reset();
			iterator->m_oskContext.reset();
			iterator = s_userInfoList.erase( iterator );
		}

		s_webBrowserDialog.reset();
		s_keyboardContext.reset();
		sceUserServiceTerminate();
		
		int ret = finalizeUtil();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		
		return ret;
	}

private:
};

//E Instance definition of the application class
//J アプリケーションクラスのインスタンス定義
Application g_application;


int main(void)
{
	int ret = 0;
	(void)ret;

	ret = g_application.initialize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

	while(1){
		ret = g_application.update();
		if (ret != SCE_OK){
			break;
		}
		g_application.render();
	}

	ret = g_application.finalize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

	return 0;
}

void setPadCtrlUserId( sce::SampleUtil::System::UserId userId )
{
	s_padCtrlUserId	= userId;
}

sce::SampleUtil::System::UserId getPadCtrlUserId()
{
	return	s_padCtrlUserId;
}

sce::SampleUtil::System::UserIdManager* getUserIdManager()
{
	return	s_userIdManager;
}

int enqueueJobItem(sce::SampleUtil::Thread::JobItem *job)
{
	int ret = g_application.getJobQueue()->enqueue(job);
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	return ret;
}