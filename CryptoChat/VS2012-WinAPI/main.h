/****************************************************************************

    Файл main.h

    Заголовочный файл модуля main.cpp.
 

****************************************************************************/

#define _CRT_RAND_S
#include "EllipticCurve.h"
#include <time.h>
#include <process.h>
#include <Commctrl.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h> 

typedef size_t socklen_t;

//----------------------------------------
// глобальные переменные и константы

#define AppWindowName "CryptoChat"

#define LENGTH_STR_ERROR_HEX 9
#define LENGTH_STR_DATETIME  29
#define LENGTH_STR_PORT      6
#define LENGTH_STR_INT_DEC   11

// идентификаторы edit'ов
#define EDIT_CONSOLE   50   // идентификатор консоли
#define EDIT_INPUT     51   // идентификатор поля ввода сообщений

// идентификаторы кнопок
#define BT_LISTEN     200
#define BT_CONNECT    201
#define BT_DISCONNECT 202

// идентификаторы static'ов
#define IP_STATIC     500
#define PORT_STATIC   501

// идентификатор поля ввода IP адреса
#define IP_ADDRESS    1000   

HINSTANCE hIns;             // описатель приложения

WNDPROC EditDefault;       // указатель на стандартную функцию обработчик событий edit'a

// будем считать что пока сокет заполнен NULL'ом соединение с абонентом отсутствует
SOCKET sock = NULL;         // сокет соединения с абонентом

//----------------------

// структура, связанная с главным окном
typedef struct _RWinStruct {

    HWND hwnd;              // описатель основного окна приложения
    unsigned int width;     // ширина окна
    unsigned int height;    // высота окна

	// Edits
	HWND editConsole;       // описатель окна Консоли  
	HWND editInput;         // описатель окна ввода сообщений
	HWND editPort;

	HWND IpAddress;			// описатель окна ввода IP адреса


	// Statics
	HWND IpStatic;  
	HWND PortStatic; 

	// Buttons
	HWND buttonListen;      // перевести программу в режим ожидания подключения
	HWND buttonConnect;     // подключится к хосту
	HWND buttonDisconnect;  // разоравать соединение

} RWinStruct;

RWinStruct *rws;


//----------------------------------------
// прототипы функций


BOOL DefaultsInitialization(void);

BOOL Disconnect(void);

int sendBuf (SOCKET sock, char *buf);

bool sendCryptoParameters(SOCKET sock,
						  char *buf_a,
						  char *buf_b,
						  char *buf_p,
						  char *buf_x_G,
						  char *buf_y_G);

unsigned int sendEncryptedMessage(SOCKET sock, encrypted_message *enc_msg);

unsigned int recvEncryptedMessage(SOCKET sock, encrypted_message *enc_msg);

unsigned __stdcall Connect(void* pArguments);

unsigned __stdcall Listen(void* pArguments);

void SendMessageOnConsole(char *msg, ...);

ATOM RegisterClassMainWindow (void);

HWND CreateMainWindow (void);

void CreateControlWindowsMain (HWND hwnd);

LRESULT CALLBACK WndProcMain (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK EditInputWndProc(HWND, UINT, WPARAM, LPARAM); 


