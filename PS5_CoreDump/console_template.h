/* SIE CONFIDENTIAL
 * PlayStation(R)5 Programmer Tool Runtime Library Release 2.00.00.09-00.00.00.0.1
 * Copyright (C) 2019 Sony Interactive Entertainment Inc.
 */

#pragma once

class Application
{
private:

public:

	Application(){}
	~Application() = default;

	int initialize();
	int run();
	int finalize();
	
};
