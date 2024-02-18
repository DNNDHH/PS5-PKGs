#include "console_template.h"

#include <stdio.h>
#include <scebase.h>
#include <libdbg.h>
#pragma comment( lib, "SceDbg_nosubmission_stub_weak")

#include <string.h>
#include <coredump.h>

const size_t sCoredumpHandlerStackSize = 16 * 1024;
const size_t sTestBufferLen = 1024;
size_t sTestBufferWriteCounter = 0;
int sTestBuffer[sTestBufferLen];

static int32_t coredumpHandler(void *pCommon);
void basicTest(void);
void writeUserDataAsText(const char* format, ...);
void onCrash(void);

int Application::initialize() 
{
	printf("initialize\n");
	// Load modules

	// Create objects and initialize parameters
	int ret = 0;
	(void)ret;
	//E Register the coredump handler
	//J コアダンプハンドラの登録
	ret = sceCoredumpRegisterCoredumpHandler(coredumpHandler,
		sCoredumpHandlerStackSize,
		(void*)this);
	SCE_DBG_ASSERT(ret == SCE_OK);
	
	return 0;
}

int Application::run()
{
	// Run
	printf("run\n");
	basicTest();
	
	return 0;
}

int Application::finalize() 
{
	printf("finalize\n");
	// finalize and destroy objects
	int ret = 0;
	(void)ret;

	//E Unregister the coredump handler
	//J コアダンプハンドラの登録解除
	ret = sceCoredumpUnregisterCoredumpHandler();
	SCE_DBG_ASSERT(ret == SCE_OK);

	// Unload modules

	return 0;
}

//E Main test code, intentional fault
	//J テストコード本体、意図的にクラッシュさせる
void basicTest(void)
{
	memset(sTestBuffer, 0, sizeof(sTestBuffer));

	for (sTestBufferWriteCounter = 0; sTestBufferWriteCounter < sTestBufferLen; sTestBufferWriteCounter++) {
		sTestBuffer[sTestBufferWriteCounter] = sTestBufferWriteCounter;
	}

	__builtin_trap();
}

//E Utility function for debug messages
//J デバッグメッセージのためのユーティリティ関数
void writeUserDataAsText(const char* format, ...)
{
	static char formatted_text[1024]; // temporary log buffer
	va_list ap;
	va_start(ap, format);
	int size = vsnprintf(formatted_text, sizeof(formatted_text), format, ap);
	va_end(ap);
	sceCoredumpWriteUserData(formatted_text, size);
}

//E Event handler called by Coredump handler
//J コアダンプハンドラーから呼び出されるイベントハンドラ
void onCrash(void)
{
	writeUserDataAsText("# Test Buffer Info\n");
	writeUserDataAsText("Size:%d\n", sizeof(sTestBuffer));
	writeUserDataAsText("Write Counter:%d\n", sTestBufferWriteCounter);
	writeUserDataAsText("The following is the test buffer dump\n");
	sceCoredumpWriteUserData(sTestBuffer, sizeof(sTestBuffer));
}

int main(int argc, char **argv)
{
	int err = SCE_OK;

	Application application;
	printf("## Sample Application: start initializing ##\n");
	application.initialize();

	application.run();

	application.finalize();
	printf("## Sample Application: finalized ##\n");

	SCE_DBG_ASSERT(err == SCE_OK);
	return 0;
}

//E Coredump Handler
//J コアダンプハンドラー
static int32_t coredumpHandler(void *pCommon)
{
	onCrash();

	return 0;
}