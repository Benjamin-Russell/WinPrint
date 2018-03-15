#pragma once // Windows-only program

// Original Author: Benjamin Russell 2018
// MIT License
// Github: https://github.com/Benjamin-Russell/WinPrint

#include <Windows.h>
#include <string>
#include <fstream>

const std::string PRINT_FILE_NAME = "To Print.txt";
const std::string PATH_TO_PRINT_FILE = "../Resources/";

// Wrapper for Windows print functionality.
// Prints given or default file
namespace WinPrint
{
	// Attempt to print a file, returns any errors.
	// This is the only function one needs to call.
	// Could easily be made to take full file path
	std::string PrintFile(std::string fileName = PRINT_FILE_NAME);

	// Process the print-file and send each line to the printer's drivers.
	void DrawPrintFile(HDC printer, DOCINFO* docInfo, std::string filePath, std::string* errorMessage);

	// Print text to device context
	void PrintText(HDC printer, int x, int y, std::string text);

	// Get name of system's default printer. 
	// If you attempt to print something manually, the name
	// of the first printer presented should be exactly what 
	// this function returns. 

	// Provide a string ptr for error messages. If populated,
	// the function failed.
	LPWSTR GetDefaultPrinterName(std::string* errorMessage);

	// Returns -1 on error
	int GetFileSize(std::string filePath);

	// string to wstring
	std::wstring convertStringToWstring(std::string str);
};