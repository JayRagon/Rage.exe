#pragma once

#include "Communication.hpp"

class KernelInterface
{
public:
	HANDLE hDriver;

	KernelInterface(LPCSTR RegistryPath)
	{
		hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}

	DWORD GetClientAddress()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
		{
			return 2;
		}

		ULONG dOutput = 0;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_CLIENTADDRESS, &dOutput, sizeof(dOutput), &dOutput, sizeof(dOutput), &Bytes, NULL))
		{
			return dOutput;
		}

		return 1;
	}

	DWORD SetMouse(LONG XMOVE, LONG YMOVE, LONG LDOWN)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
		{
			return 2;
		}

		DWORD Bytes;

		MOUSEDATA mouseData;

		mouseData.XMOVE = XMOVE;
		mouseData.YMOVE = YMOVE;
		mouseData.LDOWN = LDOWN;

		if (DeviceIoControl(hDriver, IOCTLCUSTOM, &mouseData, sizeof(mouseData), 0, 0, &Bytes, NULL))
		{
			return 0;
		}

		return 1;
	}
};