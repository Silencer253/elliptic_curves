/****************************************************************************

    Модуль main.cpp


****************************************************************************/

#include "main.h"

//--------------------
//
// Инициализация полей значениями по умолчанию
//
BOOL DefaultsInitialization(void) {

PDWORD pIpAddress = (PDWORD)malloc(sizeof(DWORD));

	*pIpAddress = 0x7f000001; // 127.0.0.1
	SendMessage(rws->IpAddress, IPM_SETADDRESS, 0, (LPARAM) *pIpAddress); 
	SetWindowText(rws->editPort, "7070");

	return FALSE;
}

//--------------------
//
// Разрывает соединение
//
BOOL Disconnect(void) {
	
	closesocket(sock);
	sock = NULL;
	SendMessageOnConsole("Disconnected.", 0);

	return 0;
}

//--------------------
//
// Отправляет буфер в сокет
//
int sendBuf (SOCKET sock, char *buf) {

char *lengthBufStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
unsigned int lengthBuf;
	
	lengthBuf = strlen(buf) + 1;
	_itoa_s(lengthBuf, lengthBufStr, LENGTH_STR_INT_DEC, 10);
	send(sock, lengthBufStr, LENGTH_STR_INT_DEC, 0);
	send(sock, buf, lengthBuf, 0);

	return 0;
}

//--------------------
//
// Отправляет параметры шифрования
//
bool sendCryptoParameters (SOCKET sock,
						  char *buf_a,
						  char *buf_b,
						  char *buf_p,
						  char *buf_x_G,
						  char *buf_y_G,
						  char *buf_order_G) {

	sendBuf (sock, buf_a);
	sendBuf (sock, buf_b);
	sendBuf (sock, buf_p);
	sendBuf (sock, buf_x_G);
	sendBuf (sock, buf_y_G);
	sendBuf (sock, buf_order_G);

	return 0;
}

//--------------------
//
// Отправляет зашифрованное сообщение
// 0 - функция завершилась без ошибок
// 1 - не удалось выделить память
unsigned int sendEncryptedMessage(SOCKET sock, encrypted_message *enc_msg) {

char *lengthMsgStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char)),
	*lengthStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));

	if(lengthMsgStr == NULL || lengthStr == NULL)
		return 1;

	_itoa_s(enc_msg->length, lengthMsgStr, LENGTH_STR_INT_DEC, 10);
	send(sock, lengthMsgStr, LENGTH_STR_INT_DEC, 0);
	
	for(unsigned int i = 0; i < enc_msg->length; ++i) {
		sendBuf(sock, enc_msg->x_P1[i]);
		sendBuf(sock, enc_msg->y_P1[i]);
		sendBuf(sock, enc_msg->x_P2[i]);
		sendBuf(sock, enc_msg->y_P2[i]);
	}

	return 0;
}

//--------------------
//
// Принимает зашифрованное сообщение
// 0 - функция завершилась без ошибок
// 1 - не удалось выделить память
// SOCKET_ERROR - ошибка при приёме данных
unsigned int recvEncryptedMessage(SOCKET sock, encrypted_message *enc_msg) {

char *lengthMsgStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char)),
	*lengthStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
size_t length;

	if(lengthMsgStr == NULL || lengthStr == NULL)
		return 1;

	if(recv(sock, lengthMsgStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	enc_msg->length = atoi(lengthMsgStr);

	// Выделяем память под массивы указателей на байтовые буферы
	enc_msg->x_P1 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->x_P1 == NULL)
		return 1;
	enc_msg->y_P1 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->y_P1 == NULL)
		return 1;
	enc_msg->x_P2 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->x_P2 == NULL)
		return 1;
	enc_msg->y_P2 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->y_P2 == NULL)
		return 1;

	for(unsigned int i = 0; i < enc_msg->length; ++i) {
		if(recv(sock, lengthStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		} else {
			length = atoi(lengthStr);
			enc_msg->x_P1[i] = (char *)malloc(length * sizeof(char));
			if(recv(sock, enc_msg->x_P1[i], length, 0) == SOCKET_ERROR) {
				return SOCKET_ERROR;
			}
		}
	
		if(recv(sock, lengthStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		} else {
			length = atoi(lengthStr);
			enc_msg->y_P1[i] = (char *)malloc(length * sizeof(char));
			if(recv(sock, enc_msg->y_P1[i], length, 0) == SOCKET_ERROR) {
				return SOCKET_ERROR;
			}
		}

		if(recv(sock, lengthStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		} else {
			length = atoi(lengthStr);
			enc_msg->x_P2[i] = (char *)malloc(length * sizeof(char));
			if(recv(sock, enc_msg->x_P2[i], length, 0) == SOCKET_ERROR) {
				return SOCKET_ERROR;
			}
		}

		if(recv(sock, lengthStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		} else {
			length = atoi(lengthStr);
			enc_msg->y_P2[i] = (char *)malloc(length * sizeof(char));
			if(recv(sock, enc_msg->y_P2[i], length, 0) == SOCKET_ERROR) {
				return SOCKET_ERROR;
			}
		}
	}

	return 0;
}

//--------------------
//
// Подключается по указанному адресу
//
unsigned __stdcall Connect(void* pArguments) {

struct sockaddr_in addr;
socklen_t size = sizeof(addr);
WSADATA wsaData;
char *ErrorNumber = (char *)malloc(LENGTH_STR_ERROR_HEX * sizeof(char));
char *strPort = (char *)malloc(LENGTH_STR_PORT * sizeof(char));
char *lengthMsgStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
char *message;
char *buf_a, *buf_b, *buf_p, *buf_x_G, *buf_y_G, *buf_order_G, *buf_x_P, *buf_y_P;
Integer a, b, p, x_G, y_G, order_G, k, n, x_P_A, y_P_A, x_P_B, y_P_B;
unsigned int retValue;
u_short Port;
PDWORD pIpAddress = (PDWORD)malloc(sizeof(DWORD));
DWORD IpAddress = 0;

	// инициализируем библиотеку
    if (WSAStartup (MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		SendMessageOnConsole("Error init library", 0);
        return 1;
    }

	// создаем сокет TCP/IP
    sock = socket (AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock == INVALID_SOCKET) {
		_itoa_s(WSAGetLastError(), ErrorNumber, LENGTH_STR_ERROR_HEX, 16);
		SendMessageOnConsole("Error create socket ", ErrorNumber, 0);
        WSACleanup();
        return 1;
    }

	// заполняем структуру с адресом удаленного узла
    memset (&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    GetWindowText(rws->editPort, strPort, LENGTH_STR_PORT);
	Port = atoi(strPort);
    addr.sin_port = htons (Port);
	SendMessage(rws->IpAddress, IPM_GETADDRESS, 0, (LPARAM) pIpAddress);  
	IpAddress += FOURTH_IPADDRESS ((LPARAM) *pIpAddress) << 24;
	IpAddress += THIRD_IPADDRESS ((LPARAM) *pIpAddress) << 16;
	IpAddress += SECOND_IPADDRESS ((LPARAM) *pIpAddress) << 8;
	IpAddress += FIRST_IPADDRESS ((LPARAM) *pIpAddress);
    addr.sin_addr.s_addr = IpAddress; 

	// подсоединяемся к удаленного узлу
    if (connect (sock, (struct sockaddr*)&addr, size) < 0){
		_itoa_s(WSAGetLastError(), ErrorNumber, LENGTH_STR_ERROR_HEX, 16);
		SendMessageOnConsole("Error connect ", ErrorNumber, ".", 0);
        WSACleanup();
        return 1;
    }
    else {
		SendMessageOnConsole("Connect success.", 0);

		/* Генерируем параметры шифрования и преобразуем их в байтовые массивы для дальнейшей 
		передачи */
		retValue = generateCryptoParameters(&a, &b, &p, &x_G, &y_G, &k, &n, &x_P_A, &y_P_A, &order_G);
		if(!retValue) {
			IntegerToBuf(a, &buf_a);
			IntegerToBuf(b, &buf_b);
			IntegerToBuf(p, &buf_p);
			IntegerToBuf(x_G, &buf_x_G);
			IntegerToBuf(y_G, &buf_y_G);
			IntegerToBuf(x_P_A, &buf_x_P);
			IntegerToBuf(y_P_A, &buf_y_P);
			IntegerToBuf(order_G, &buf_order_G);
	
			// Отправляем параметры шифрования и освобождаем память выделенную под них
			sendCryptoParameters(sock, buf_a, buf_b, buf_p, buf_x_G, buf_y_G, buf_order_G);
			free(buf_a);
			free(buf_b);
			free(buf_p);
			free(buf_x_G);
			free(buf_y_G);
			free(buf_order_G);

			// Обмениваемся открытыми ключами и освобождаем память выделенную под них
			sendPublicKey(sock, buf_x_P, buf_y_P);
			free(buf_x_P);
			free(buf_y_P);
			recvPublicKey(sock, &buf_x_P, &buf_y_P, &x_P_B, &y_P_B);
			free(buf_x_P);
			free(buf_y_P);
		} else if(retValue == 1) {
			SendMessageOnConsole("Error! Invalid crypto parameters.", 0);
			return 1;
		}
		else if(retValue == 1) {
			SendMessageOnConsole("Error! Curve is singular.", 0);
			return 1;
		}

		SendMessageOnConsole("Session created.", 0);

		// принимаем сообщения
		while(sock) {
			encrypted_message *enc_msg = (encrypted_message *)malloc(sizeof(encrypted_message));
			if(recvEncryptedMessage(sock, enc_msg) == SOCKET_ERROR) {
				Disconnect();
				return 1;
			}
			// расшифровываем сообщение
			decrypt(&message, enc_msg);
			free(enc_msg->x_P1);
			free(enc_msg->x_P2);
			free(enc_msg->y_P1);
			free(enc_msg->y_P2);
			free(enc_msg);

			// выводим сообщение на консоль
			SendMessageOnConsole("Interlocutor: ", message, 0);
			free(message);
		}
    }

	free(ErrorNumber);
	free(strPort);
	free(lengthMsgStr);
	free(pIpAddress);
	WSACleanup();
	return 0;
}

//--------------------
//
// Переводит приложение в режим прослушивания
//
unsigned __stdcall Listen(void* pArguments) {

SOCKET mySocket;
struct sockaddr_in addr;
struct sockaddr_in client_addr;
WSADATA wsaData;
char *ErrorNumber = (char *)malloc(LENGTH_STR_ERROR_HEX * sizeof(char));
char *strPort = (char *)malloc(LENGTH_STR_PORT * sizeof(char));
char *buf_x_P, *buf_y_P;
ostringstream stream_x_P, stream_y_P;
Integer a, b, p, x_G, y_G, order_G, k, n, x_P_A, y_P_A, x_P_B, y_P_B;
char *lengthMsgStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
char *message;
u_short Port;

	// инициализируем библиотеку
    if (WSAStartup (MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		SendMessageOnConsole("Error init library", 0);
        return 1;
    }

	// создаем сокет TCP/IP
    mySocket = socket (AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (mySocket == INVALID_SOCKET) {
		_itoa_s(WSAGetLastError(), ErrorNumber, LENGTH_STR_ERROR_HEX, 16);
		SendMessageOnConsole("Error create socket ", ErrorNumber, 0);
        WSACleanup();
        return 1;
    }

	// заполняем структуру с прослушиваемым адресом
    memset (&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	GetWindowText(rws->editPort, strPort, LENGTH_STR_PORT);
	Port = atoi(strPort);
    addr.sin_port = htons (Port);
    addr.sin_addr.s_addr = 0;       // слушаем любой интерфейс

	// привязываем адрес к порту
    if (bind (mySocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		_itoa_s(WSAGetLastError(), ErrorNumber, LENGTH_STR_ERROR_HEX, 16);
		SendMessageOnConsole("Error bind ", ErrorNumber, 0);
        WSACleanup();
        return 1;
    }

    // переводим сокет в режим прослушивания
    if (listen (mySocket, 100) == SOCKET_ERROR) {
		_itoa_s(WSAGetLastError(), ErrorNumber, LENGTH_STR_ERROR_HEX, 16);
		SendMessageOnConsole("Error listen ", ErrorNumber, 0);
        WSACleanup();
        return 1;
    }

	// ожидаем входящее соединение и получаем сокет соединения
	SendMessageOnConsole("Listening...", 0);
	while((sock = accept(mySocket, (sockaddr *) &client_addr, NULL))) {
		if (sock == -1) {
			SendMessageOnConsole("Error accept", 0);
			continue;
		}
		SendMessageOnConsole("Connect success.", 0);

		// Принимаем параметры шифрования
		recvCryptoParameters(sock, &a, &b, &p, &x_G, &y_G, &order_G);
		
		// Принимаем открытый ключ собеседника
		Integer x_P_B, y_P_B;
		recvPublicKey(sock, &buf_x_P, &buf_y_P, &x_P_B, &y_P_B);
		Integer x_P(buf_x_P), y_P(buf_y_P);
		free(buf_x_P);
		free(buf_y_P);

		// Генерируем наш секретный и открытый ключ
		generateKeys(&a, &b, &p, &x_G, &y_G, &k, &order_G, &n, &x_P, &y_P);
		stream_x_P << x_P; 
		string str_x_P(stream_x_P.str()); 
		buf_x_P = (char *)str_x_P.c_str();
		stream_y_P << y_P; 
		string str_y_P(stream_y_P.str()); 
		buf_y_P = (char *)str_y_P.c_str();

		// Отправляем наш открытый ключ
		sendPublicKey(sock, buf_x_P, buf_y_P);

		SendMessageOnConsole("Session created.", 0);

		// принимаем сообщения
		while(sock) {
			encrypted_message *enc_msg = (encrypted_message *)malloc(sizeof(encrypted_message));
			if(recvEncryptedMessage(sock, enc_msg) == SOCKET_ERROR) {
				Disconnect();
				return 1;
			}
			decrypt(&message, enc_msg);

			// выводим сообщение на консоль
			SendMessageOnConsole("Interlocutor: ", message, 0);
			free(message);
		}
	}

	free(ErrorNumber);
	free(strPort);
	free(lengthMsgStr);
	WSACleanup();
	return 0;
}


//--------------------
//
// Отправляет сообщение на Консоль
// 
void SendMessageOnConsole(char *msg, ...) {

time_t rawtime;
char *str;
struct tm timeinfo;
char **cp = &msg;                        // адрес первого указателя
unsigned int len = LENGTH_STR_DATETIME;  // длина строки (в начале инициализируем длиной строки с датой и временем)
int ndx = GetWindowTextLength(rws->editConsole); // определяем длину текста в консоле

	// перемещаем каретку в конец текста
    SendMessage (rws->editConsole, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);

	// формируем строку с датой и временем в str
	time ( &rawtime );
	localtime_s (&timeinfo, &rawtime );
	str = (char *)malloc(len * sizeof(char));

	strcpy_s(str, len, "[");
	strcat_s(str, len, asctime (&timeinfo));
	strcat_s(str, len, "] ");

	// выводим дату и время сообщения
	SendMessage(rws->editConsole, EM_REPLACESEL, TRUE, (LPARAM)str);
	
	// цикл для определения общей длины сцепляемых строк
	while (*cp) { len += strlen(*cp); cp++; } 
	++len;                              // не забываем про завершающий ноль :)
	str = (char *)malloc(len * sizeof(char));
	str[0] = 0;
	// цикл для сцепления строк
	cp = &msg;							// опять установка на 1-й параметр
	while (*cp) {
		strcat_s(str, len, *cp);        // прицепляем первую (и следующие)
		cp++;							// перемещаемся на следующую
	}         
	SendMessage(rws->editConsole, EM_REPLACESEL, TRUE, (LPARAM)str);
	SendMessage(rws->editConsole, EM_REPLACESEL, TRUE, (LPARAM)"\r\n");
	
	return;
}


//--------------------
//
// Основная функция оконных приложений
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){

MSG msg;
HWND hWnd;
    
    hIns = hInstance;

	hWnd = CreateMainWindow();
	if(!hWnd) {
        MessageBox(NULL,"Cannot create window","Error",MB_OK);
        return 0;
    }

// Консоль для отладки
#ifdef _DEBUGV
	ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    AllocConsole();
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
 
    int h = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	 *stdout = *(_fdopen((int)h, "w"));
    setvbuf(stdout, NULL, _IONBF, 0);
 
    cout << "Hello! Welcome to Debug mode. :)" << endl;
#endif

    // Основной цикл обработки сообщений
    while ( GetMessage(&msg, NULL, 0, 0) ) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

    return msg.wParam;
}


//--------------------
//
// Регистрация класса основного окна приложения
//
ATOM RegisterClassMainWindow (void){

WNDCLASSEX WndClass;

    WndClass.cbSize         = sizeof (WndClass);            // размер структуры класса
	WndClass.style          = CS_DBLCLKS;
    WndClass.lpfnWndProc    = WndProcMain;                  // адрес оконной процедуры класса
    WndClass.cbClsExtra     = 0;                            // размер дополнительной памяти класса
    WndClass.cbWndExtra     = sizeof(void*);                // размер дополнительной памяти окна
    WndClass.hInstance      = hIns;                         // описатель приложения
    WndClass.hIcon          = LoadIcon (hIns, "MainIcon");
    WndClass.hCursor        = LoadCursor (NULL, IDC_ARROW); // курсор окна
    WndClass.hbrBackground  = (HBRUSH) GetStockObject (BLACK_BRUSH);
    WndClass.lpszMenuName   = NULL;
    WndClass.lpszClassName  = AppWindowName;                // имя класса
    WndClass.hIconSm        = LoadIcon (hIns, "MainIcon");

    return RegisterClassEx (&WndClass);
}

//--------------------
//
// Создание основного окна приложения
//
HWND CreateMainWindow (void) {

HWND hwnd;

    RegisterClassMainWindow();

	hwnd = CreateWindowEx(WS_EX_CONTROLPARENT | WS_EX_APPWINDOW | DS_3DLOOK,
                          AppWindowName,
                          "CryptoChat",
                          WS_OVERLAPPEDWINDOW,
                          10, 10,
                          600, 550,
                          NULL,                 // описатель родительского окна
                          NULL,                 // описатель главного меню (для главного окна)
                          hIns,NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Ошибка создания основного окна приложения", "", MB_OK);
        return NULL;
    }

    // отображение окна
    ShowWindow (hwnd, SW_SHOWNORMAL);
    UpdateWindow (hwnd);

    return hwnd;
}


//--------------------
//
// Создание управляющих элементов (контролов) главного окна
//
void CreateControlWindowsMain (HWND hwnd) {

	rws->editConsole = CreateWindow("Edit", NULL,
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER |
							ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
                            0, 0,
                            400, 400,
                            hwnd, (HMENU)EDIT_CONSOLE, hIns, NULL);

	rws->editInput = CreateWindow("Edit", NULL,
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | WS_TABSTOP | 
							ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                            0, 410,
                            400, 100,
                            hwnd, (HMENU)EDIT_INPUT, hIns, NULL);
	 // получаем указатель на стандартную функцию обработчика событий для edit'а
	EditDefault = (WNDPROC)GetWindowLongPtr(rws->editInput, GWLP_WNDPROC);
	// меняем указатель на нашу функцию
	SetWindowLongPtr(rws->editInput, GWLP_WNDPROC, (LONG_PTR)EditInputWndProc); 

    rws->buttonListen = CreateWindow("Button", "Listen",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
                            420, 10,
                            60, 50,
                            hwnd, (HMENU)BT_LISTEN, hIns, NULL);

	rws->buttonConnect = CreateWindow("Button", "Connect",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
                            420, 80,
                            60, 50,
                            hwnd, (HMENU)BT_CONNECT, hIns, NULL);

	rws->buttonDisconnect = CreateWindow("Button", "Disconnect",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
                            500, 80,
                            80, 50,
                            hwnd, (HMENU)BT_DISCONNECT, hIns, NULL);

	rws->IpStatic = CreateWindow("Static", "IP:", 
                            WS_CHILD | WS_VISIBLE | WS_GROUP | SS_LEFT,
                            405, 200,
                            20, 20,
                            hwnd, (HMENU)IP_STATIC, hIns, NULL);

	rws->IpAddress = CreateWindow("SysIPAddress32", NULL,
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | 80,
                            425, 200,
                            125, 20,
                            hwnd, (HMENU)IP_ADDRESS, hIns, NULL);

	rws->PortStatic = CreateWindow("Static", "Port:", 
                            WS_CHILD | WS_VISIBLE | WS_GROUP | SS_LEFT,
                            405, 240,
                            30, 20,
                            hwnd, (HMENU)PORT_STATIC, hIns, NULL);

	rws->editPort = CreateWindow("Edit", NULL,
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_NUMBER,
                            435, 240,
                            45, 20,
                            hwnd, (HMENU)IP_ADDRESS, hIns, NULL);

    return;
}


//--------------------
//
// Функция обработки сообщений главного окна приложения.
//
LRESULT CALLBACK WndProcMain(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

HANDLE hThread;
unsigned int threadID;


    switch (iMsg) {

        case WM_CREATE:
			// создание структуры для окна
            rws = (RWinStruct*) malloc(sizeof(RWinStruct));
            if(rws == NULL)
                return -1;
            rws->hwnd = hwnd;

            // связываем указатель на структуру с окном
            SetWindowLong(hwnd, 0, (LONG)rws);
			
            CreateControlWindowsMain(hwnd);
			DefaultsInitialization();
			SendMessageOnConsole("Welcome! :)", 0);
            return 0 ;

        case WM_DESTROY:
			free(rws);
            PostQuitMessage(0);
            return 0;


        // сообщение посылается при нажатии кнопок
        case WM_COMMAND:

            // в параметре wParam идентификатор нажатой кнопки
            switch(LOWORD (wParam)) {

                case BT_LISTEN:
					// Создаём дополнительный поток, в котором ждём подключений
					hThread = (HANDLE)_beginthreadex( NULL, 0, &Listen, NULL, 0, &threadID );
                    break;

				case BT_CONNECT:
					hThread = (HANDLE)_beginthreadex( NULL, 0, &Connect, NULL, 0, &threadID );
					if(threadID) {
						closesocket(sock);
						sock = NULL;
					}
					break;

				case BT_DISCONNECT:
					Disconnect();
					break;

                default:
                    break;			
            }

			return 0;


        case WM_LBUTTONDOWN:
            SetFocus (hwnd);
            break;
    }

    return DefWindowProc(hwnd,iMsg,wParam,lParam);
}

//--------------------
//
// Функция обработки сообщений окна ввода приложения.
//
LRESULT CALLBACK EditInputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

unsigned int lengthMsg;
unsigned char *msg;
char *lengthMsgStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
encrypted_message *enc_msg;

	if(lengthMsgStr == NULL)
		return -1;

    switch(message) {
		case WM_CHAR:
			switch (LOWORD (wParam)) {
				// возврат каретки
				case 0x0D:
					break;

				// перевод строки
				case 0x0A:
					break;

				default:
					return CallWindowProc(EditDefault, hWnd, message, wParam, lParam);
					break;
			}
			break;
		case WM_KEYDOWN:
			switch (LOWORD (wParam)) {
					// Если нажата клавиша Enter
					case VK_RETURN:
						lengthMsg = GetWindowTextLength(rws->editInput);
						// Если активно соединение и поле ввода не пусто, то отправляем сообщение
						if(sock != NULL && lengthMsg != 0) {
							++lengthMsg; // завершающий ноль
							msg = (unsigned char *)malloc(lengthMsg * sizeof(char));
							if(msg == NULL)
								return -1;
							GetWindowText(rws->editInput, (char *)msg, lengthMsg);
							// создание структуры для зашифрованного сообщения
							enc_msg = (encrypted_message *) malloc(sizeof(encrypted_message));
							if(enc_msg == NULL)
								return -1;
							if(encrypt(msg, lengthMsg, enc_msg)) {
								SetWindowText(rws->editInput, "");
								return -1;
							}
							SetWindowText(rws->editInput, "");

							// отправляем зашифрованное сообщение
							sendEncryptedMessage(sock, enc_msg);

							SendMessageOnConsole("Me: ", msg, 0);
						}

					default:
						break;	
			}
			break;

		// все остальные события пусть обрабатывает стандартный обработчик
		default:
			return CallWindowProc(EditDefault, hWnd, message, wParam, lParam);
			break;
    }
    return 0;
}
