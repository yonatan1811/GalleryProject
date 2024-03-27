#include <iostream>
#include <string>
#include "MemoryAccess.h"
#include "AlbumManager.h"
#include "DataBaseAccess.h"


int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}

void InitPrint()
{
	time_t now = time(nullptr);
	std::cout << "Developer : Yonatan Cohen \nTime : " << std::put_time(localtime(&now) , "%d/%m/%Y %H:%M:%S") << std::endl;
}

int main(void)
{
	// initialization data access
	//MemoryAccess dataAccess;
	DataBaseAccess dataAccess;

	//setting the pictures id each time so it wont start on 100 every time
	

	// initialize album manager
	AlbumManager albumManager(dataAccess);


	std::string albumName;
	

	std::cout << "Welcome to Gallery!" << std::endl;
	std::cout << "===================" << std::endl;
	InitPrint();
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;
	
	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}


