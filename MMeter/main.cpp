#include "myclass.h"

extern "C" __declspec(dllexport) const char* getMeterValue(char* imagePath, char* workPath)
{
	try {
		MMeter::myclass MC(imagePath, workPath);
		MC.process2();
		return MC.getMeterValue();
	}
	catch (int) {		
		return "-1";
	}
}

//
//int main()
//{
//	//MMeter::myclass MC("C:\\Users\\gudim\\Documents\\Visual Studio 2015\\Projects\\TestOpenCV\\TestOpenCV\\bin\\Debug\\Sources\\img_0529.jpg");
//	//MC.process();
//	//_getch();
//	return 0;
//}
//
