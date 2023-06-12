#include <Windows.h>
#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include <chrono>
#include <fstream>
#include "Main.h"
#include "kernelinterface.hpp"
#include <wingdi.h>

#pragma warning (disable : 6385)

/* TO DO
	cpy hbitmap to a bitmap so that we can scan each pixel in the bitmap YEAH BABE DONE
	
	do PixelSearch WOOOOOOOOOO

	MAKE SURE THAT OUTPUT JPG IS IDENTICAL TO C#... OR ELSE nah looks good enough
	
	OK OK WE DID IOCTL COMMUNICATION ON THE DRIVER'S SIDE NOW USE IT AND SEE IF IT WORKS GUESS WHAT VISUAL STUDIO IS BEING A PEICE OF *#@$ I AM GOING TO *#&$(@# DO IT A DIFFERENT WAY ALL I NEED TO DO IS PASS 3 *$&#)@* VARIABLES AND THAT'S IT! NOTHING ELSE, NOTHING MORE THATS IT AND I CANNOT DO IT BEECAUSE VIUSAL STUdio doesn't *&@#&^@ feel like it today i have given up with holding caps lock and i have almost given up all hope if the stupid ioctl is the only way to pass a few *$(#&@# VARIABLES THROUGH THATS IT. Guess what? this is my 4th problem on my computer that NOBODY ELSE HAS NOBODY NOT A SINGLE SOUL has this error, it's just me and it's the 4th error that has not been seen anywhere else and i DON'T KNOW WHY IT'S LIKE MY COPY OF WINDOWS IS A FAT LAZY DUDE WHO JUST DOESN'T FEEL LIKE DOING SOME SPECIFIC THINGS THAT INCLUDE GETTING OFF THE COUCH AND COMPILE A DRIVER THAT HAS MORE THAN 2 FILES IN IT, ON TOP OF THAT MY KEYBOARD ISS SCREWING UP AND I HAVEN'T GOTTEN A NEW ONE YET BECAUSE A WOOTING COSTS WAY TOO MUCH AND I DON'T FEEL LIKE IT, JUST LIKE MY STUPID OPERATING SYSTEM AND VISUAL STUDIO, MY BLOODY SETTINGS DOESN'T OPEN, OBS AND STREAMLABS DON'T OPEN, MY MAIL APP DOESN'T UPDATE, AND MY SANITY IS GONE
	
	ok no moree ioctl, time for shared memory

	hook up the move and click methods to the driver (good luck...)


	and the final step, DESTROY EVERYONE YOU SEE WITH SILENT AIM YEAH LEAVE THEM NO MERCY HOLD T HARDER THAN EVER WHILE QUEUING WITH FRIENDS UNTIL THEY NOTICE THEN SET UP THE CHEAT ON MARCUS' PC OH YEAH BABY	

	yo i made it
	*/

using namespace std;
using namespace chrono;

const string communicationPath = "C:\\_Rage\\Communication.txt";
const string communicationPathX = "C:\\_Rage\\CommunicationX.txt";
const string communicationPathY = "C:\\_Rage\\CommunicationY.txt";
const string communicationPathL = "C:\\_Rage\\CommunicationL.txt";

const string driverCommunicationPath = "\\DosDevices\\C:\\_Rage\\Communication.txt";

//cfg
LONG FOVX = 300;
LONG FOVY = 300;
int res = 2;
byte silentAim = 0;

//globals
double xMove;
double yMove;
double sens;

double sensScale;

int startCoords; // DO NOT USE THESE, USE A RECTANGLE
int finishCoords; // DO NOT USE THESE, USE A RECTANGLE

int pixelsFound;

int sHeight = GetSystemMetrics(SM_CYSCREEN);
int sWidth = GetSystemMetrics(SM_CXSCREEN);
int SWC = sWidth / 2; // Screen Width Center
int SHC = sHeight / 2; // screen Height Center

double xAvg = 0;
double yAvg = 0;

// MOST OPTIMAL THRESHOLD

int MIN_R = 180;
int MAX_R = 255;
int MIN_G = 80;
int MAX_G = 160;
int MIN_B = 180;
int MAX_B = 255;

int TARGET_R = 248; //253 is actual yellow outlines
int TARGET_G = 248;
int TARGET_B = 1; // LOL THE ONLY COLOR THAT CHANGES IS BLUE AHAHAHAAAAAAA GET REKT NERD we might change this later if its too inconsistant

const int KEY = 0x54; // SEE ASCII TABLE

struct // this is all i need to pass to the driver
{
	LONG XMOVE;
	LONG YMOVE;
	LONG LDOWN;
} Ragefly ;



void Move(LONG x, LONG y)
{
	KernelInterface Driver = KernelInterface("\\\\.\\ragedrv1");

	Driver.SetMouse(x, y, 0);

	/*
	for (LONG i = 5; i > 0; i--)
	{
		Driver.SetMouse((x / (i + 1)) * 0.27, (y / (i + 1)) * 0.27, 0);
		Sleep(5);
	}

	for (LONG i = 0; i < 5; i++)
	{
		Driver.SetMouse((x / (i + 1)) * 0.27, (y / (i + 1)) * 0.27, 0);
		Sleep(5);
	}
	*/

}

void Click()
{
	KernelInterface Driver = KernelInterface("\\\\.\\ragedrv1");

	Driver.SetMouse(0, 0, 1);
	Sleep(10);
	Driver.SetMouse(0, 0, 2);
}

void HLoop() // the hack loop
{
	cout << "[+] Executed!\n\n";

	LONG arrSize = (FOVX * 4 * FOVY);

	sensScale = 1.074 * pow(sens, -0.9936827126);

	//creating a bitmapheader for getting the dibits
	BITMAPINFOHEADER bminfoheader;
	::ZeroMemory(&bminfoheader, sizeof(BITMAPINFOHEADER));
	bminfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bminfoheader.biWidth = FOVX;
	bminfoheader.biHeight = -FOVY;
	bminfoheader.biPlanes = 1;
	bminfoheader.biBitCount = 32;
	bminfoheader.biCompression = BI_RGB;
	bminfoheader.biSizeImage = FOVX * 4 * FOVY;
	bminfoheader.biClrUsed = 0;
	bminfoheader.biClrImportant = 0;

	for (;;)
	{
		//auto duration = high_resolution_clock::now();

		xAvg = 0;
		yAvg = 0;

		pixelsFound = 0;

		// SCREENGRAB HERE
		
		HBITMAP hBitmap;
		HDC hdc = GetDC(NULL);

		unsigned char* pPixels = new unsigned char[arrSize];

		//ScreenCapture(SWC - (FOVX / 2), SHC - (FOVY / 2), FOVX, FOVY, "C:\\Users\\USER\\Desktop\\c++\\not exe\\fortnite.jpg", hBitmap);

		ScreenCaptureNBM(SWC - (FOVX / 2), SHC - (FOVY / 2), FOVX, FOVY, hBitmap);

		GetDIBits(hdc, hBitmap, 0, FOVY, pPixels, (BITMAPINFO*)&bminfoheader, DIB_RGB_COLORS);

		// PXSEARCH HERE

		for (int y = 0; y < FOVY; y+=res)
		{
			for (int x = 0; x < FOVX; x+=res)
			{
				if (MIN_R < pPixels[(FOVX * y + x) * 4 + 2] && MAX_R > pPixels[(FOVX * y + x) * 4 + 2]
					&& MIN_G < pPixels[(FOVX * y + x) * 4 + 1] && MAX_G > pPixels[(FOVX * y + x) * 4 + 1]
					&& MIN_B < pPixels[(FOVX * y + x) * 4 + 0] && MAX_B > pPixels[(FOVX * y + x) * 4 + 0])
				{
					if (pixelsFound == 0) // change this later
					{
						xAvg = x + (SWC - (FOVX / 2));
						yAvg = y + (SHC - (FOVY / 2));

						//cout << "b = " << (int)pPixels[(FOVX * y + x) * 4 + 0] << endl;
						pixelsFound++;
					}
				}
			}
		}


		DeleteDC(hdc);
		DeleteObject(hBitmap); // delete the bitmap and all that
		delete[] pPixels; // delete the array of rgb

		

		//cout << pixelsFound << "\n";

		if (pixelsFound != 0) // if a pixel has been found
		{
			// avg out the results

			//xAvg /= pixelsFound;
			//yAvg /= pixelsFound;

			// AIM TIME! :3

			//cout << "pixels found = " << pixelsFound << endl;

			//if (GetAsyncKeyState(KEY))
			if (true)
			{
				// std instaflick

				xMove = xAvg - SWC;
				yMove = yAvg - SHC;

				//xMove /= sens;
				//yMove /= sens;

				xMove *= sensScale;
				yMove *= sensScale;

			    yMove += 600 * sens; // offset because it only finds the top pixels

				Move((int)xMove, (int)yMove);
				//Click();
				//Sleep(1);

				if (silentAim == 1)
				{
					// the silent aim bois (im not gonna use it, too sus) + it kinda doesn't work

					Sleep(1);
					Move((int)-xMove, (int)-yMove);
					Sleep(1);
				}
			}
		}


		//auto finish = chrono::high_resolution_clock::now();
		//chrono::duration<double> elapsed = finish - duration;
	    //cout << "Time: " << (elapsed.count() * 1000) << "ms" << endl;

		//Sleep(5); // for safety
	}
}


int main() // set cfg
{

	//Click();
	Move(100, 100); // dbg


	cout << "[+] Monitor Width = " << sWidth << "\n[+] Monitor Height = " << sHeight << "\n\n";

	cout << "what is the speed of the colorbot (the higher the less accuracy, minimum 1)\n> ";
	cin >> res;
	cout << "what is the fovX (in pixels, I use 1000)\n> ";
	cin >> FOVX;
	cout << "what is the fovY (in pixels, I use 600)\n> ";
	cin >> FOVY;
	cout << "do you want silent aim? (1 for on 0 for off, IF ON: less sus to spectators, but more sus to anticheat)\n> ";
	cin >> silentAim;
	cout << "what is your sens\n> ";
	cin >> sens;

	HLoop();
}




/*
	HBITMAP hBitmap;
	HDC hdc = GetDC(NULL);

	unsigned char* pPixels = new unsigned char[(FOVX * 4 * FOVY)];

	//creating a bitmapheader for getting the dibits
	BITMAPINFOHEADER bminfoheader;
	::ZeroMemory(&bminfoheader, sizeof(BITMAPINFOHEADER));
	bminfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bminfoheader.biWidth = FOVX;
	bminfoheader.biHeight = -FOVY;
	bminfoheader.biPlanes = 1;
	bminfoheader.biBitCount = 32;
	bminfoheader.biCompression = BI_RGB;

	bminfoheader.biSizeImage = FOVX * 4 * FOVY;
	bminfoheader.biClrUsed = 0;
	bminfoheader.biClrImportant = 0;

	Sleep(2000);

	ScreenCapture((SWC) - (FOVX / 2), (SHC) - (FOVY / 2), FOVX, FOVY, "C:\\Users\\USER\\Desktop\\c++\\not exe\\fortnite.jpg", hBitmap); // dbg SCREENSHOT WORKS CPY HBITMAP TO BITMAP

	GetDIBits(hdc, hBitmap, 0, FOVY, pPixels, (BITMAPINFO*)&bminfoheader, DIB_RGB_COLORS);

	int x, y; // fill the x and y coordinate

	x = 100;
	y = 100;

	int r = pPixels[(FOVX * y + x) * 4 + 2];
	int g = pPixels[(FOVX * y + x) * 4 + 1];
	int b = pPixels[(FOVX * y + x) * 4 + 0];

	DeleteObject(hBitmap);
	delete[] pPixels; // delete the array of objects


	cout << "r = " << r << "\ng = " << g << "\nb = " << b << "\n";
*/