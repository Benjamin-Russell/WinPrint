#include "WinPrint.h"

// Attempt to print a file, return any errors
std::string WinPrint::PrintFile(std::string fileName)
{
	// Complete file path
	std::string filePath = PATH_TO_PRINT_FILE + fileName;
	std::wstring wFilePath = WinPrint::convertStringToWstring(filePath);

	// Get default printer name
	std::string errorMessage;
	LPWSTR printerName = WinPrint::GetDefaultPrinterName(&errorMessage);

	// return error if applicable
	if (errorMessage.size() > 0)
	{
		return errorMessage;
	}

	// Define DOCINFO structure
	DOCINFO documentInfo = DOCINFO();
	documentInfo.cbSize = WinPrint::GetFileSize(filePath);
	documentInfo.lpszDocName = wFilePath.c_str();

	if (documentInfo.cbSize == -1)
	{
		// Failure: Unable to find size of the file
		return "Error: Unable to access print file.";
	}

	// Create device context for printer
	HDC printerDeviceContext = CreateDC(NULL, printerName, NULL, NULL);

	if (printerDeviceContext == NULL)
	{
		// Failure: Unable to create device context
		return "Error: Unable to find printer.";
	}

	// Here is where you might set up an AbortProc. 
	// Doesn't seem necessary for my own use.

	// Call printer and process document
	DrawPrintFile(printerDeviceContext, &documentInfo, filePath, &errorMessage);

	if (errorMessage.size() > 0)
	{
		// Something went wrong
		return errorMessage;
	}

	// Delete device context
	DeleteDC(printerDeviceContext);

	// No errors
	return "";
}

void WinPrint::DrawPrintFile(HDC printer, DOCINFO* docInfo, std::string filePath, std::string* errorMessage)
{
	*errorMessage = "";

	// This function and others wait for the printer to respond, and the program will be unresponsive while waiting.
	int printResult = StartDoc(printer, docInfo);
	if (printResult <= 0)
	{
		// Failure: Unable to print for 1 of many possible reasons
		*errorMessage = "Error: Could not create print job.";
		return;
	}

	// Begin a page
	printResult = StartPage(printer);
	if (printResult <= 0)
	{
		// Failure: Unable to print for 1 of many possible reasons
		*errorMessage = "Error: Could not complete print job.";
		return;
	}

	// Open print file
	std::ifstream printFile(filePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!printFile.is_open())
	{
		*errorMessage = "Error: Unable to access print file.";
		return;
	}

	// Create font
	// Font options: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183499(v=vs.85).aspx
	unsigned int fontSize = 96;
	HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));

	// Select the font for writing text
	SelectObject(printer, hFont);

	// Draw contents of the file line by line
	std::string line;
	int y = 0;
	while (std::getline(printFile, line))
	{
		WinPrint::PrintText(printer, 0, y, line);

		// Increment y by font size (next line)
		y += fontSize;
	}

	// Clean up
	DeleteObject(hFont);
	printFile.close();

	// End Page
	printResult = EndPage(printer);
	if (printResult <= 0)
	{
		// Failure: Unable to print for 1 of many possible reasons
		*errorMessage = "Error: Could not complete print job.";
		return;
	}

	// End Print Call
	printResult = EndDoc(printer);
	if (printResult <= 0)
	{
		// Failure: Unable to print for 1 of many possible reasons
		*errorMessage = "Error: Could not complete print job.";
		return;
	}
}

void WinPrint::PrintText(HDC printer, int x, int y, std::string text)
{
	// Convert string to wstring
	std::wstring wsText = WinPrint::convertStringToWstring(text);

	// If text ends with a \r or \n, take it out.
	unsigned int textLength = lstrlen(wsText.c_str());
	if (textLength >= 1)
	{
		if (text[textLength - 1] == '\r'
			|| text[textLength - 1] == '\n')
		{
			textLength--;
			text.resize(textLength);
		}
	}

	// If there's any text to print
	if (textLength > 0)
	{
		TextOut(printer, x, y, wsText.c_str(), textLength);
	}
}

LPWSTR WinPrint::GetDefaultPrinterName(std::string* errorMessage)
{
	*errorMessage = "";
	unsigned long printerNameLength;
	GetDefaultPrinter(NULL, &printerNameLength); // Get length of name

	// Allocate proper size for printerName
	LPWSTR printerName = LPWSTR(malloc(sizeof(wchar_t)*printerNameLength));

	// Get the name for real
	int printerRetrieveValue = GetDefaultPrinter(printerName, &printerNameLength);

	if (printerNameLength == 0)
	{
		// Failure: There is no default printer
		*errorMessage = "Error: There is no default printer.";
	}
	else if (printerRetrieveValue == 0)
	{
		// Failure: Unable to retrieve default printer
		*errorMessage = "Error: Unable to find default printer name.";
	}

	// If errorMessage is populated, this printer name won't be valid
	return printerName;
}

int WinPrint::GetFileSize(std::string filePath)
{
	std::ifstream file(filePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!file.is_open())
	{
		return -1;
	}

	file.seekg(0, std::ios::end);
	int fileSize = (int)file.tellg();
	file.close();

	return fileSize;
}

std::wstring WinPrint::convertStringToWstring(std::string str)
{
	size_t lengthReturnValue;
	std::wstring toReturn = std::wstring(str.size(), L' ');
	mbstowcs_s(&lengthReturnValue, &toReturn[0], (size_t)(str.size() + 1), str.c_str(), _TRUNCATE);
	toReturn.resize(lengthReturnValue); // Shrink to fit

	return toReturn;
}