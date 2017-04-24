// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "myclass.h"
#include "myclass.cpp"
#include "MConfig.h"
#include "Helpers.h"
#include "Helpers.cpp"
#include "MConfig.cpp"
#include "KNearestOcr.h"
#include "KNearestOcr.cpp"
#include <iostream>
#include <conio.h>

int main()
{
	//MMeter::myclass MC0("c:\\!vd\\349039289.jpg", "c:\\!vd\\work\\");
	//MC0.learn();
	MMeter::myclass MC1("c:\\!vd\\16315952_levels.jpg", "c:\\!vd\\work\\");
	MC1.getMeterValue();// learn();
	/*MMeter::myclass MC2("c:\\!vd\\003.jpg", "c:\\!vd\\work\\");
	MC2.getMeterValue();
	MMeter::myclass MC3("c:\\!vd\\004.jpg", "c:\\!vd\\work\\");
	MC3.getMeterValue();
	MMeter::myclass MC4("c:\\!vd\\005.jpg", "c:\\!vd\\work\\");
	MC4.getMeterValue();
	MMeter::myclass MC5("c:\\!vd\\006.jpg", "c:\\!vd\\work\\");
	MC5.getMeterValue();
	MMeter::myclass MC6("c:\\!vd\\007.jpg", "c:\\!vd\\work\\");
	MC6.getMeterValue();
	MMeter::myclass MC7("c:\\!vd\\008.jpg", "c:\\!vd\\work\\");
	MC7.getMeterValue();
	MMeter::myclass MC8("c:\\!vd\\009.jpg", "c:\\!vd\\work\\");
	MC8.getMeterValue();*/	
    _getch();	
    return 0;
}

