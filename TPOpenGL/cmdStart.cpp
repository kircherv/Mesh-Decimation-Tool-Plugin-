#include "cmdStart.h"
//#include <Commdlg.h>
//#include <windows.h>
//#include "resource.h"

void CmdStart::selectObj()
{
	std::cout << "Please select which file you would like to import and press Enter" << std::endl << "write 1 for Planet" << std::endl << "write 2 for nanosuit" << std::endl << "write 3 for rock " << std::endl;
	
	std::cin >> cmdFileSelect;
	switch (cmdFileSelect)
	{
		case 1:
		{
			cmdFile = "resources/objects/noTexture/planet.obj";
			std::cout << "Selected 1: planet" << std::endl;
			break;
		}
		case 2:
		{
			cmdFile = "resources/objects/noTexture/nanosuit.obj";
			std::cout << "Selected 2: Nanosuit" << std::endl;
			break;
		}
		case 3:
		{
			cmdFile = "resources/objects/noTexture/rock.obj";
			std::cout << "Selected: 3: rock" << std::endl;
			break;
		}
	}
	Model currentModel(FileSystem::getPath(cmdFile));
	currentModel.setPath(cmdFile);

}
