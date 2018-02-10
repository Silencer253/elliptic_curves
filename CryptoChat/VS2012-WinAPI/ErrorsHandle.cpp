/****************************************************************************

    Модуль ErrorsHandle.cpp
	Класс обработчика ошибок.

****************************************************************************/

#include "ErrorsHandle.h"

class ErrorsHandle {
	public:
		unsigned int number;

	ErrorsHandle(unsigned int number) {
		switch (number) {
			case ERROR_ALLOCATION_MEMORY:
			case ERROR_CONNECTION_TRUBLE:

			default:
				break;
		}
	}
};