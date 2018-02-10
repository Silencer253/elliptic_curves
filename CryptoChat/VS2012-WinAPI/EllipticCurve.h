/****************************************************************************

    Файл EllipticCurve.h

    Заголовочный файл модуля EllipticCurve.cpp.
 

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>

#include "dll.h"
using CryptoPP::Integer;

using namespace std;

#define LENGTH_STR_INT_DEC   11

// Структура зашифрованного сообщения
struct encrypted_message {
	unsigned int length;
	char **x_P1;
	char **y_P1;
	char **x_P2;
	char **y_P2;
};

//----------------------------------------
// прототипы функций

unsigned int generateCryptoParameters(Integer *a,
								Integer *b,
								Integer *p,
								Integer *x_G,
								Integer *y_G,
								Integer *k,
								Integer *n,
								Integer *x_P,
								Integer *y_P,
								Integer *order_G);
unsigned int generateKeys(Integer *a,
						Integer *b,
						Integer *p,
						Integer *x_G,
						Integer *y_G,
						Integer *k,
						Integer *order_G,
						Integer *n,
						Integer *x_P,
						Integer *y_P);

unsigned int recvPublicKey(SOCKET sock,
					char **buf_x_P,
					char **buf_y_P,
					Integer *x_P,
					Integer *y_P);

bool sendPublicKey(SOCKET sock,
					char *buf_x_P,
					char *buf_y_P);

unsigned int recvCryptoParameters(SOCKET sock,
						  Integer *a,
						  Integer *b,
						  Integer *p,
						  Integer *x_G,
						  Integer *y_G,
						  Integer *order_G);

unsigned int IntegerToBuf(Integer a, char **buf);

unsigned int encrypt(unsigned char *msg,
					 unsigned int length, 
					 encrypted_message *enc_msg);

unsigned int decrypt(char **msg,
					 encrypted_message *enc_msg);
