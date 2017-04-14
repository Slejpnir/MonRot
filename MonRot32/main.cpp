
#include "stdafx.h"
using namespace std;
HANDLE hSerial;

void DisplayRotate(int rotation)
{
	DEVMODE dm;
	// initialize the DEVMODE structure
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	DISPLAY_DEVICE monitor;
	ZeroMemory(&monitor, sizeof(monitor));
	monitor.cb = sizeof(monitor);

	EnumDisplayDevices(nullptr, 1, &monitor, EDD_GET_DEVICE_INTERFACE_NAME);

	if (0 != EnumDisplaySettings(monitor.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
	{
		
		if (rotation == DMDO_DEFAULT)
		{
			//dm.dmPosition.x = -1950;
			//dm.dmPosition.y = -150;
		}
		if (rotation == DMDO_90)
		{
			//dm.dmPosition.x = -1200;
			//dm.dmPosition.y = -714;
		}
		// swap height and width
		auto dwTemp = dm.dmPelsHeight;
		dm.dmPelsHeight = dm.dmPelsWidth;
		dm.dmPelsWidth = dwTemp;

		// determine new orientaion
		dm.dmDisplayOrientation = rotation;


		auto lRet = ChangeDisplaySettingsEx(monitor.DeviceName, &dm, nullptr, 0, nullptr);
		if (DISP_CHANGE_SUCCESSFUL != lRet)
		{
			cout << "Failed";// add exception handling here
		}
	}
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgs, int nWinMode)
{
	auto sPortName = L"COM4";
	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			cout << "serial port does not exist.\n";
		}
		cout << GetLastError() << "" << "some other error occurred.\n";
	}
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		cout << "getting state error\n";
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	//dcbSerialParams.fAbortOnError=
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		cout << "error setting serial port state\n";
	}

	auto data = 'A';
	DWORD dwSize = sizeof(data);
	DWORD dwBytesWritten;
	//BOOL iRet;
	char sReceivedChar;
	DWORD iSize = 0;

	auto is_first_time = true;

	auto rotation = 0;
	//BOOL IsRead ;

	_COMMTIMEOUTS tt;
	tt.ReadIntervalTimeout = 100;
	tt.ReadTotalTimeoutConstant = 100;
	tt.ReadTotalTimeoutMultiplier = 100;


	SetCommTimeouts(hSerial, &tt);

	while (true)
	{
		WriteFile(hSerial, &data, dwSize, &dwBytesWritten, nullptr);

		//if (iRet)
		//cout << dwSize << " Bytes in string. " << dwBytesWritten << " Bytes sended. " << endl;
		iSize = 0;
		sReceivedChar = 'N';
		ReadFile(hSerial, &sReceivedChar, 1, &iSize, nullptr);  // получаем 1 байт
		if (is_first_time) {
			if (sReceivedChar == data)
				rotation = DMDO_DEFAULT;
			else
				rotation = DMDO_90;
			is_first_time = false;
			continue;
		}
		if (sReceivedChar == data) {
			if (rotation == DMDO_90) {
				rotation = DMDO_DEFAULT;
				DisplayRotate(rotation);

			}
		}
		else {
			if (rotation == DMDO_DEFAULT) {
				rotation = DMDO_90;
				DisplayRotate(rotation);

			}
		}
		Sleep(1);
	}

	//CloseHandle(hSerial);


	//system("PAUSE");

	//return 0;
}