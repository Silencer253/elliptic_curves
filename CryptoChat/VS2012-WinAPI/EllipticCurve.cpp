/****************************************************************************

    Модуль EllipticCurve.cpp

В данном модуле реализованы классы эллиптической кривой и точки на эллиптической кривой,
а также все функции связанные с шифрованием в приложении CryptoChat.

****************************************************************************/

#include "EllipticCurve.h"
#include "Polynom.h"
#include <iosfwd>
#include <vector>

using CryptoPP::AutoSeededRandomPool;
using CryptoPP::RandomNumberGenerator;
using CryptoPP::ModularArithmetic;
using CryptoPP::a_exp_b_mod_c;
using CryptoPP::a_times_b_mod_c;
using namespace std;

//--------------------
//
// Класс эллиптических кривых. 
//
class EllipticCurve {

public:
	Integer *a, *b, *p;

	//--------------------
	//
	// Конструктор эллиптической кривой. 
	//
	/*EllipticCurve(char *string_parameter_a, char *string_parameter_b, char *string_parameter_p) {
		setParameters(string_parameter_a, string_parameter_b, string_parameter_p);
	}*/

	EllipticCurve(Integer *parameter_a, Integer *parameter_b, Integer *parameter_p) {
		setParameters(parameter_a, parameter_b, parameter_p);
	}

	//--------------------
	//
	// Метод позволяющий изменить параметры эллиптической кривой. 
	//
	void setParameters(Integer *parameter_a, Integer *parameter_b, Integer *parameter_p) {
		p = parameter_p;
		a = parameter_a;
		b = parameter_b;

		return;
	}

	//--------------------
	//
	// Метод вывода на консоль параметров эллиптической кривой. 
	//
	void printParameters() {
		cout << "E" << *p << "(" << *a << ", " << *b <<  ")";
	}

	//--------------------
	//
	// Метод подсчёта порядка кривой путём перебора. 
	//
	Integer order() {

	Integer n = 1, y_2;

		for(Integer x = 0; x < *p; ++x) {
			y_2 = (a_exp_b_mod_c(x, 3, *p) + (*a) * x + (*b)) % *p;
			if((y_2 == 0) || (a_exp_b_mod_c(y_2, (*p - 1) / 2, *p) == 1)) {
				Integer Sq = y_2.SquareRoot();
				n += 1;
				if((-Sq % *p) != 0) 
					n += 1;
			}
		}

		return n;
	}

	//--------------------
	//
	// Метод подсчёта порядка кривой с помощью алгоритма Шуфа. 
	//
	Integer order_by_schoof() {
		Polynom *psy = NULL;
		Integer lower_bound = 4 * (*(this->p)).SquareRoot(); // Нижняя граница 4*sqrt(p)
		Integer l = 2, product_of_a_sequence = l;
		unsigned int L;
		while(product_of_a_sequence <= lower_bound) {
			l++;
			if(IsPrime(l))
				product_of_a_sequence *= l;
		}
		L = static_cast<int>(l.ConvertToLong());
		#ifdef _DEBUG
			cout << "L = " << L << endl;
		#endif
		//L = l;
		this->PolynomsForSchoof(psy, L);
		this->schoof(psy);

		return 0;
	}

	//--------------------
	//
	// Вычислить полиномы деления для алгоритма Шуфа. 
	//
	void PolynomsForSchoof(Polynom *psy, const unsigned int L) {
		ModularArithmetic ring = ModularArithmetic(*(this->p)); 
		psy = new Polynom[L+3];
		Polynom poly_1("1 x^0", ring);
		Polynom poly_2("2 x^0", ring);
		Polynom poly_4("4 x^0", ring);
		Polynom Y(ring, 4);
		Y.SetCoefficient(0, (*(this->b)) % (*(this->p)), ring);
		Y.SetCoefficient(1, (*(this->a)) % (*(this->p)), ring);
		Y.SetCoefficient(3, 1, ring);
		Polynom f_2 = Y * Y;
		psy[1] = poly_1;
		psy[2] = poly_2;
		Polynom poly3(ring, 5);
		poly3.SetCoefficient(0, (-a_exp_b_mod_c((*(this->a)), 2, (*(this->p)))) % *(this->p), ring);
		poly3.SetCoefficient(1, a_times_b_mod_c((*(this->b)), 12, (*(this->p))), ring);
		poly3.SetCoefficient(2, a_times_b_mod_c((*(this->a)), 6, (*(this->p))), ring);
		poly3.SetCoefficient(4, 3 % (*(this->p)), ring);
		psy[3] = poly3;
		Polynom poly4(ring, 7);
		poly4.SetCoefficient(0, (-8 * a_exp_b_mod_c((*(this->b)), 2, (*(this->p))) - a_exp_b_mod_c((*(this->a)), 3, (*(this->p)))) % *(this->p), ring);
		poly4.SetCoefficient(1, ((-4) * a_times_b_mod_c(*(this->a), *(this->b), *(this->p))) % *(this->p), ring);
		poly4.SetCoefficient(2, ((-5) * a_exp_b_mod_c((*(this->a)), 2, (*(this->p)))) % *(this->p), ring);
		poly4.SetCoefficient(3, (*(this->b) * 20) % *(this->p), ring);
		poly4.SetCoefficient(4, (*(this->a) * 5) % *(this->p), ring);
		poly4.SetCoefficient(6, 1, ring);
		psy[4] = poly_4 * poly4;
		unsigned int n = 2;
		unsigned int L_2 = L + 1;
#undef _DEBUG;
		while(TRUE) {
			Polynom psy_1 = psy[n+1] * psy[n+1];
			Polynom psy_2 = psy[n+2] * psy[n];
			unsigned int n_2 = 2 * n;
			psy[n_2] = (psy_2 * psy[n-1] * psy[n-1] - psy[n] * psy[n-2] * psy_1) / poly_2;
			#ifdef _DEBUG
				cout << "psy[" << n_2 << "]\n" << psy[n_2] << endl;
			#endif
			if(n_2 == (L+1))
				break;
			if (((n+1) % 2) == 1) 
				psy[n_2+1] = f_2 * (psy_2 * psy[n] * psy[n]) - psy_1 * psy[n+1] * psy[n-1];
			else
				psy[n_2+1] = psy_2 * psy[n] * psy[n] - (psy_1 * psy[n+1] * psy[n-1]) * f_2;
			#ifdef _DEBUG
				cout << "psy[" << n_2+1 << "]\n" << psy[n_2+1] << endl;
			#endif
			if((n_2+1) == (L+1))
				break;
			++n;
		}
	}

	//--------------------
	//
	// Метод подсчёта порядка кривой с помощью алгоритма Шуффа (Простые числа криптографические и
	// вычислительные аспекты Second Edition (Крэндалл Р., Померанс К.(R.Crandall, C.Pomerance)) стр. 398). 
	//
	Integer schoof(Polynom *psy, Integer l = 2) {

		ModularArithmetic ring = ModularArithmetic(*(this->p));
		Polynom poly1("x^1", ring);
		Polynom poly2(ring, 4);

		// 1-ый пункт алгоритма, случай, когда l == 2
		// g(X) = НОД(X^p - X, X^3 + a*X + b)
		if(l == 2) {
			cout << *(this->p) << endl;
			poly2.SetCoefficient(0, (*(this->b)) % (*(this->p)), ring);
			poly2.SetCoefficient(1, (*(this->a)) % (*(this->p)), ring);
			poly2.SetCoefficient(3, 1, ring);
			Polynom r = Polynom::powmod(poly1, *(this->p), poly2) - poly1;
			Polynom g[3] = {poly2, r};

			unsigned int i0 = 0, i1 = 1, i2 = 2, t;
			while (!(g[i1].IsZero(g[i1].ms_fixedRing))) {
				g[i2] = g[i0] % g[i1];
				t = i0; i0 = i1; i1 = i2; i2 = t;
			}
			cout << g[i0] << endl;
			if(g[i0].Degree(ring) != 0) return 0;
			else return 1;
		}

		// 2-ой пункт алгоритма
		else {
			Integer p_ = *(this->p) % l;
			unsigned int l_int = static_cast<int>(l.ConvertToLong());
			Polynom u = Polynom::powmod(poly1, *(this->p), psy[l_int]);
			Polynom v = Polynom::powmod(poly2, (*(this->p) - 1)/2, psy[l_int]);
		}

		return 1;
	}

	//--------------------
	//
	// Метод проверки параметров эллиптической кривой.
	//
	static bool validParameters(EllipticCurve *curve) {
		if(!IsPrime(*(curve->p))) {
			return 0;
		}
		return 1;
	}

	//--------------------
	//
	// Метод проверки эллиптической кривой на сингулярность.
	// Возвращаемые значения: 
	// 1 - не сингулярна, 0 - сингулярна. 
	static bool isNotSingular(EllipticCurve *curve) {
		if((4*(*(curve->a))*(*(curve->a))*(*(curve->a)) + 27*(*(curve->b))*(*(curve->b))) != 0)
			return 1;
		else 
			return 0;
	}

	
};

//--------------------
//
// Класс точек эллиптической кривой. 
//
class Point {

public:
	Integer x, y;
	EllipticCurve *curve;
	/* Если флаг установлен в TRUE значит это точка является точкой в бесконечности,
	в этом случае координаты не будут иметь значения */ 
	bool point_at_infinity;

	//--------------------
	//
	// Конструктор точки.  
	//
	Point(Integer parameter_x = 0, Integer parameter_y = 0, EllipticCurve *parameter_curve = NULL, bool flag_point_at_infinity = 0) {
		setCoordinates(parameter_x, parameter_y, flag_point_at_infinity);
		setCurve(parameter_curve);
	}

	//--------------------
	//
	// Метод, позволяющий определить координаты точки.  
	//
	void setCoordinates(Integer parameter_x, Integer parameter_y, bool flag_point_at_infinity = 0) {
		x = parameter_x;
		y = parameter_y;
		point_at_infinity = flag_point_at_infinity;
	}

	//--------------------
	//
	// Метод, позволяющий определить какой кривой принадлежит точка.  
	//
	void setCurve(EllipticCurve *arg_curve) {
		curve = arg_curve;
	}

	//--------------------
	//
	// Сравнение двух точек.  
	//
	bool Compare(const Point& A) const {
		if((this->x == A.x) && (this->y == A.y) && (this->curve == A.curve))
			return 1;
		else
			return 0;
	}

	//--------------------
	//
	// Скложение двух точек.  
	//
	Point Plus (const Point &A) const {

	Integer lambda, x, y;

	// Проверяем ссылаются ли обе точки на одну кривую, если нет, то проверяем равны ли эти кривые
		if((this->curve != A.curve) || (this->curve->a != A.curve->a || this->curve->b != A.curve->b || this->curve->p != A.curve->p)) {
			x = 0;
			y = 0;
			Point R(x, y, NULL);
			return R;
		}
		if(A.point_at_infinity)
			return *this;
		else if(this->point_at_infinity)
			return A;
		else if(this->x != A.x) {
			lambda = (A.y - this->y) * ((A.x - this->x).InverseMod(*(this->curve->p)));
		}
		else if((this->x == A.x) && (this->y == A.y) && (this->y != 0) && (A.y != 0))
			lambda = (3 * (this->x) * (this->x) + *(this->curve->a)) * ((2 * (this->y)).InverseMod(*(this->curve->p)));
		else if( (this->x == A.x) && ((this->y == -A.y) || (-(this->y))%*(this->curve->p) == A.y) ) {
			x = 0;
			y = 0;
			Point R(x, y, this->curve, 1);
			return R;
		}
		
		x = (lambda * lambda - this->x - A.x) % *(this->curve->p);
		y = (lambda * (this->x  - x) - this->y) % *(this->curve->p);
		Point R(x, y, this->curve);

		return R;
	}

	bool operator==(const Point& A) {return this->Compare(A) == 1;}

	Point operator+(const Point& A) {return this->Plus(A);}

	//--------------------
	//
	// Метод умножения точки на число.  
	//
	Point power(Integer k) {
		Integer x = 0, y = 0;
		Point Q(x, y, this->curve, 1);
		unsigned int t = k.BitCount();
		for(int i = t - 1; i >= 0; --i) {
			Q = Q + Q;
			if(k.GetBit(i))
				Q = Q + *this;
		}
		if(k < 0)
			Q.y = -Q.y % *this->curve->p;

		return Q;
	}

	//--------------------
	//
	// Метод вычисления порядка точки.  
	//
	Integer order() {
	
	Point A(this->x, this->y, this->curve);
	Integer order = 1;

		while(!A.point_at_infinity) {
			A = A + *this;
			++order;
		}

		return order;
	}

	
	//--------------------
	//
	// Метод вывода на консоль параметров точки.  
	//
	void printParameters() {
		if(point_at_infinity)
			cout << "P" << "(inf, inf)";
		else
			cout << "P" << "(" << x << ", " << y <<  ")";
	}

	//--------------------
	//
	// Деструктор точки.  
	//
	/*~Point() {
		delete x;
		delete y;		
	}*/
};

// Структура с параметрами шифрования
typedef struct _crypto_parameters {
	Integer *a;
	Integer *b;
	Integer *p;
	Integer *k;
	Integer *n;
	Integer *x_G;
	Integer *y_G;
	Integer *x_P;
	Integer *y_P;
} crypto_parameters;

crypto_parameters *crypto_params;

//--------------------
//
// Функция генерации таблицы кодирования.  
//
bool generateCodebook(Point P[], EllipticCurve *curve) {

Integer x = -1, y1 = 0, y2 = 0, y_2;
int c = 0;

	while(c < 255) {
		do {
			x = (x + 1) % *(curve->p);
			if(x == 0 && c != 0)
				return 1;
			y_2 = (a_exp_b_mod_c(x, 3, *(curve->p)) + (*(curve->a)) * x + (*(curve->b))) % (*(curve->p));
		} while(!y_2.IsSquare());
		y1 = y_2.SquareRoot();
		P[c].setCoordinates(x, y1);
		P[c].setCurve(curve);
		++c;
		
		y2 = (-y1) % *(curve->p);
		P[c].setCoordinates(x, y2);
		P[c].setCurve(curve);
		++c;
	}
	return 0;
}

#ifndef _CONSOLE
//--------------------
//
// Функция вывода таблицы кодирования на консоль.  
//
void printCodebook(Point P[]) {

	printf("Алфавит точек эллиптической кривой\n+-ASCII code------Symbol----------Point-----+");
	for(int c = 0; c < 256; ++c) {
		if(c == 0)
			printf("\n|%d\t\t|(null)\t\t|", c, c);
		else if(c == 7)
			printf("\n|%d\t\t|(beep)\t\t|", c, c);
		else if(c == 8)
			printf("\n|%d\t\t|(BS)\t\t|", c, c);
		else if(c == 9)
			printf("\n|%d\t\t|(HTab)\t\t|", c, c);
		else if(c == 10)
			printf("\n|%d\t\t|(LF)\t\t|", c, c);
		else if(c == 13)
			printf("\n|%d\t\t|(CR)\t\t|", c, c);
		else if(c == 32)
			printf("\n|%d\t\t|(space)\t|", c, c);
		else
			printf("\n|%d\t\t|%c\t\t|", c, c);
		P[c].printParameters();
	}
	printf("\n");

	return;
}
#endif



//--------------------
//
// Функция генерации параметров шифрования.  
// 0 - функция завершилась без ошибок
// 1 - параметр p не является простым числом
// 2 - кривая является сингулярной
// 3 - не удалось выделить память
unsigned int generateCryptoParameters(Integer *a,
								Integer *b,
								Integer *p,
								Integer *x_G,
								Integer *y_G,
								Integer *k,
								Integer *n,
								Integer *x_P,
								Integer *y_P,
								Integer *order_G) {

	/* Тут вместо того чтобы инициализировать их статически
	необходимо реализовать алгоритм выбора кривой E и генерирующей точки G */
	*a = 62;
	*b = 31;
	*p = 132661;
	*x_G = 46272;
	*y_G = 316;
	EllipticCurve curve(a, b, p);
	if(!EllipticCurve::validParameters(&curve))
		return 1;
		if(!EllipticCurve::isNotSingular(&curve))
			return 2;
	Point G(*x_G, *y_G, &curve);

	//// Тестируем Шуффа
	//Integer *t1, *t2, *t3, 
	///* a */		t4("1452046121366725933991673688168680114377396846588"), 
	///* b */		t5("49"), 
	///* p */		t6("1452046121366725933991673688168680114377396846591"); //Integer::Power2(255) - 19; // 132661
	//t1 = &t4;
	//t2 = &t5;
	//t3 = &t6;
	//EllipticCurve curve1(t1, t2, t3);
	//Integer order_curve1;
	////order_curve1 = curve1.order();
	//order_curve1 = curve1.order_by_schoof();	
	
	// Выберем случайно k из полуинтервала [1, G.order())
	*order_G = G.order();
	AutoSeededRandomPool arngA;
	RandomNumberGenerator& rngA = *dynamic_cast<RandomNumberGenerator *>(&arngA);
	*k = Integer(rngA, 1, *order_G-1);

	// Выберем случайно наш секретный ключ n из полуинтервала [1, G.order())
	*n = Integer(rngA, 1, *order_G-1);

	// Генерируем наш открытый ключ
	Point P = G.power(*n);
	*x_P = P.x;
	*y_P = P.y;

	// создание и заполнение структуры содержащей в себе указатели на параметры шифрования
	crypto_params = (crypto_parameters *) malloc(sizeof(crypto_parameters));
	if(crypto_params == NULL)
		return 3;
	memset(crypto_params, 0, sizeof(crypto_parameters));
	crypto_params->a = a;
	crypto_params->b = b;
	crypto_params->p = p;
	crypto_params->k = k;
	crypto_params->n = n;
	crypto_params->x_G = x_G;
	crypto_params->y_G = y_G;

	return 0;
}

//--------------------
//
// Генерирует открытый и секретный ключи
// 0 - функция завершилась без ошибок
// 1 - параметр p не является простым числом
// 2 - кривая является сингулярной
unsigned int generateKeys(Integer *a,
						Integer *b,
						Integer *p,
						Integer *x_G,
						Integer *y_G,
						Integer *k,
						Integer *order_G,
						Integer *n,
						Integer *x_P,
						Integer *y_P) {

	EllipticCurve curve(a, b, p);
	if(!EllipticCurve::validParameters(&curve))
		return 1;
		if(!EllipticCurve::isNotSingular(&curve))
			return 2;

	Point G(*x_G, *y_G, &curve);

	// Выберем случайно k из полуинтервала [1, G.order())
	AutoSeededRandomPool arngA;
	RandomNumberGenerator& rngA = *dynamic_cast<RandomNumberGenerator *>(&arngA);
	*k = Integer(rngA, 1, *order_G-1);

	// Выберем случайно наш секретный ключ n из полуинтервала [1, G.order())
	*n = Integer(rngA, 1, *order_G-1);

	// Генерируем наш открытый ключ
	Point P = G.power(*n);
	*x_P = P.x;
	*y_P = P.y;
	
	crypto_params->k = k;
	crypto_params->n = n;

	return 0;
}

//--------------------
//
// Принимает открытый ключ
// 0 - функция завершилась без ошибок
// 1 - не удалось выделить память
// 2 - проблема с соединением
unsigned int recvPublicKey(SOCKET sock,
					char **buf_x_P,
					char **buf_y_P,
					Integer *x_P,
					Integer *y_P) {

char *lengthKeyStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
unsigned int lengthKey;

	if(lengthKeyStr == NULL)
		return 1;

	if(recv(sock, lengthKeyStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthKey = atoi(lengthKeyStr);
		*buf_x_P = (char *)malloc(lengthKey * sizeof(char));
		recv(sock, *buf_x_P, lengthKey, 0);
		Integer key(*buf_x_P);
		*x_P = key;
	}

	if(recv(sock, lengthKeyStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthKey = atoi(lengthKeyStr);
		*buf_y_P = (char *)malloc(lengthKey * sizeof(char));
		recv(sock, *buf_y_P, lengthKey, 0);
		Integer key(*buf_y_P);
		*y_P = key;
	}

	crypto_params->x_P = x_P;
	crypto_params->y_P = y_P;

	return 0;
}

//--------------------
//
// Отправляет наш открытый ключ
//
bool sendPublicKey(SOCKET sock,
					char *buf_x_P,
					char *buf_y_P) {

char *lengthKeyStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char));
unsigned int lengthKey;

	lengthKey = strlen(buf_x_P) + 1;
	_itoa_s(lengthKey, lengthKeyStr, LENGTH_STR_INT_DEC, 10);
	send(sock, lengthKeyStr, LENGTH_STR_INT_DEC, 0);
	send(sock, buf_x_P, lengthKey, 0);

	lengthKey = strlen(buf_y_P) + 1;
	_itoa_s(lengthKey, lengthKeyStr, LENGTH_STR_INT_DEC, 10);
	send(sock, lengthKeyStr, LENGTH_STR_INT_DEC, 0);
	send(sock, buf_y_P, lengthKey, 0);
	
	return 0;
}


//--------------------
//
// Принимает параметры шифрования
// 0 - функция завершилась без ошибок
// 1 - не удалось выделить память
// 2 - проблема с соединением
unsigned int recvCryptoParameters(SOCKET sock,
						  Integer *a,
						  Integer *b,
						  Integer *p,
						  Integer *x_G,
						  Integer *y_G,
						  Integer *order_G) {

char *lengthParameterStr = (char *)malloc(LENGTH_STR_INT_DEC * sizeof(char)),
	*buf_a, *buf_b, *buf_p, *buf_x_G, *buf_y_G, *buf_order_G;
unsigned int lengthParameter;

	if(lengthParameterStr == NULL)
		return 1;

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_a = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_a, lengthParameter, 0);
		Integer param(buf_a);
		*a = param;
	}

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_b = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_b, lengthParameter, 0);
		Integer param(buf_b);
		*b = param;
	}

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_p = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_p, lengthParameter, 0);
		Integer param(buf_p);
		*p = param;
	}

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_x_G = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_x_G, lengthParameter, 0);
		Integer param(buf_x_G);
		*x_G = param;
	}

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_y_G = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_y_G, lengthParameter, 0);
		Integer param(buf_y_G);
		*y_G = param;
	}

	if(recv(sock, lengthParameterStr, LENGTH_STR_INT_DEC, 0) == SOCKET_ERROR) {
		return 2;
	} else {
		lengthParameter = atoi(lengthParameterStr);
		buf_order_G = (char *)malloc(lengthParameter * sizeof(char));
		recv(sock, buf_order_G, lengthParameter, 0);
		Integer param(buf_order_G);
		*order_G = param;
	}

	// создание и заполнение структуры содержащей в себе указатели на параметры шифрования
	crypto_params = (crypto_parameters *) malloc(sizeof(crypto_parameters));
	if(crypto_params == NULL)
		return 1;
	memset(crypto_params, 0, sizeof(crypto_parameters));
	crypto_params->a = a;
	crypto_params->b = b;
	crypto_params->p = p;
	crypto_params->x_G = x_G;
	crypto_params->y_G = y_G;

	return 0;
}

//--------------------
//
// Функция преобразования числа типа Integer в буфер типа char *.  
// 0 - функция завершилась без ошибок
unsigned int IntegerToBuf(Integer a, char **buf) {

stringstream stream;

	stream << a; 
	string string_for_stream(stream.str());
	stream.str("");
	size_t length = strlen((char *)string_for_stream.c_str()) + 1;
	(*buf) = (char *)malloc(sizeof(char) * length);
	strcpy_s((*buf), length, (char *)string_for_stream.c_str());



	return 0;
}

//--------------------
//
// Функция шифрования.  
// 0 - функция завершилась без ошибок
// 1 - параметр p не является простым числом
// 2 - кривая является сингулярной
// 3 - не удалось выделить память
// 4 - структура с криптопараметрами не заполнена
unsigned int encrypt(unsigned char *msg, unsigned int length, encrypted_message *enc_msg) {

char *cord;
ostringstream stream_cord;
string str_cord;

	// Проверяем что структура с криптопараметрами заполнена
	if(crypto_params == NULL)
		return 4;
	else if(crypto_params->y_P == NULL)
		return 4;

	enc_msg->length = length;

	// Выделяем память под массивы указателей на байтовые буферы
	enc_msg->x_P1 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->x_P1 == NULL)
		return 3;
	enc_msg->y_P1 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->y_P1 == NULL)
		return 3;
	enc_msg->x_P2 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->x_P2 == NULL)
		return 3;
	enc_msg->y_P2 = (char **)malloc(sizeof(char *) * enc_msg->length);
	if(enc_msg->y_P2 == NULL)
		return 3;

	EllipticCurve curve(crypto_params->a, crypto_params->b, crypto_params->p);
	if(!EllipticCurve::validParameters(&curve))
		return 1;
		if(!EllipticCurve::isNotSingular(&curve))
			return 2;
	Point P[256];
	generateCodebook(P, &curve);

	// В цикле вычисляем пары точек соответствующие одному символу и преобразуем их координаты
	// в байтовые массивы
	for(unsigned int i = 0; i < length; ++i) {
		Point G(*crypto_params->x_G, *crypto_params->y_G, &curve);
		Point P1 = G.power(*crypto_params->k);
		stream_cord << P1.x; 
		str_cord = stream_cord.str(); 
		stream_cord.str("");
		cord = (char *)str_cord.c_str();
		enc_msg->x_P1[i] = (char *)malloc(sizeof(char) * strlen(cord) + 1);
		if(enc_msg->x_P1[i] == NULL)
			return 3;
		strcpy(enc_msg->x_P1[i], cord);

		stream_cord << P1.y; 
		str_cord = stream_cord.str(); 
		stream_cord.str("");
		cord = (char *)str_cord.c_str();
		enc_msg->y_P1[i] = (char *)malloc(sizeof(char) * strlen(cord) + 1);
		if(enc_msg->y_P1[i] == NULL)
			return 3;
		strcpy(enc_msg->y_P1[i], cord);

		Point P_B(*crypto_params->x_P, *crypto_params->y_P, &curve);
		Point P2 = P[msg[i]] + P_B.power(*crypto_params->k);
		stream_cord << P2.x; 
		str_cord = stream_cord.str(); 
		stream_cord.str("");
		cord = (char *)str_cord.c_str();
		enc_msg->x_P2[i] = (char *)malloc(sizeof(char) * strlen(cord) + 1);
		if(enc_msg->x_P2[i] == NULL)
			return 3;
		strcpy(enc_msg->x_P2[i], cord);
		
		stream_cord << P2.y; 
		str_cord = stream_cord.str(); 
		stream_cord.str("");
		cord = (char *)str_cord.c_str();
		enc_msg->y_P2[i] = (char *)malloc(sizeof(char) * strlen(cord) + 1);
		if(enc_msg->y_P2[i] == NULL)
			return 3;
		strcpy(enc_msg->y_P2[i], cord);
	}
	
	return 0;
}

//--------------------
//
// Функция расшифрования.  
// 0 - функция завершилась без ошибок
// 1 - параметр p не является простым числом
// 2 - кривая является сингулярной
// 3 - не удалось выделить память
// 4 - структура с криптопараметрами не заполнена
unsigned int decrypt(char **msg, encrypted_message *enc_msg) {
	
	// Проверяем что структура с криптопараметрами заполнена
	if(crypto_params == NULL)
		return 4;
	else if(crypto_params->y_P == NULL)
		return 4;

	*msg = (char *)malloc(sizeof(char) * enc_msg->length);

	EllipticCurve curve(crypto_params->a, crypto_params->b, crypto_params->p);
	if(!EllipticCurve::validParameters(&curve))
		return 1;
		if(!EllipticCurve::isNotSingular(&curve))
			return 2;
	Point P[256];
	generateCodebook(P, &curve);

	for(unsigned int i = 0; i < enc_msg->length; ++i) {
		Integer x_P2(enc_msg->x_P2[i]);
		Integer y_P2(enc_msg->y_P2[i]);
		Point P2(x_P2, y_P2, &curve);

		Integer x_P1(enc_msg->x_P1[i]);
		Integer y_P1(enc_msg->y_P1[i]);
		Point P1(x_P1, y_P1, &curve);

		Point P3 = P1.power(-*crypto_params->n);
		Point P4 = P2 + P3;
		for(unsigned int j = 0; j < 256; ++j) {
			if(P[j] == P4) {
				(*msg)[i] = (char)j;
				break;
			}
		}
	}

	return 0;
}
