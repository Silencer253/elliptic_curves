#include "dll.h"
using CryptoPP::ModularArithmetic;
using CryptoPP::Integer;
using std::cout;
using std::endl;

#pragma once
class Polynom {
	public:
		ModularArithmetic ms_fixedRing;
		bool Y;
		//! \name CREATORS
		//@{
			//! creates the zero polynomial
			Polynom() {Y = 0;}

			//!
			Polynom(const ModularArithmetic &ring, unsigned int count) : m_coefficients((size_t)count, ring.Identity()), ms_fixedRing(ring) {Y = 0;}

			//! convert from string
			Polynom(const char *str, const ModularArithmetic &ring) : ms_fixedRing(ring) {Y = 0;FromStr(str, ring);}
		//@}
	private:
		void FromStr(const char *str, const ModularArithmetic &ring);
		std::vector<Integer> m_coefficients;
	public:
	//! \name ACCESSORS
	//@{
		//! the zero polynomial will return a degree of -1
		int Degree(const ModularArithmetic &ring) const {return int(CoefficientCount(ring))-1;}
		//!
		unsigned int CoefficientCount(const ModularArithmetic &ring) const;
		//! return coefficient for x^i
		Integer GetCoefficient(unsigned int i, const ModularArithmetic &ring) const;
	//@}

	//! \name MANIPULATORS
	//@{
		//!
		Polynom&  operator=(const Polynom& t);

		//! set the coefficient for x^i to value
		void SetCoefficient(unsigned int i, const Integer &value, const ModularArithmetic &ring);
	//@}

	//! \name BASIC ARITHMETIC ON POLYNOMIALS
	//@{
		bool Equals(const Polynom &t, const ModularArithmetic &ring) const;
		//!
		bool IsZero(const ModularArithmetic &ring) const {return CoefficientCount(ring)==0;}
		//!
		Polynom Plus(const Polynom &t, const ModularArithmetic &ring) const;
		//!
		Polynom Minus(const Polynom &t, const ModularArithmetic &ring) const;
		//!
		Polynom Times(const Polynom &t, const ModularArithmetic &ring) const;
		//!
		Polynom Modulo(const Polynom &t, const ModularArithmetic &ring) const;
		//!
		Polynom DividedBy(const Polynom& t, const ModularArithmetic &ring) const;

		//! return the polynom (a^k) % n
		static Polynom powmod(Polynom a, Integer k, const Polynom &n);

		//! calculate r and q such that (a == d*q + r) && (0 <= degree of r < degree of d)
		static void Polynom::Divide(Polynom &r, Polynom&q, const Polynom &a, const Polynom &d, const ModularArithmetic &ring);
	//@}

	//! \name INPUT/OUTPUT
	//@{
		//!
		friend std::istream& operator>>(std::istream& in, Polynom &a);
		//!
		friend std::ostream& operator<<(std::ostream& out, const Polynom &a);
	//@}
};

//!
inline Polynom operator+(const Polynom &a, const Polynom &b) {return a.Plus(b, a.ms_fixedRing);}
//!
inline Polynom operator-(const Polynom &a, const Polynom &b) {return a.Minus(b, a.ms_fixedRing);}
//!
inline Polynom operator*(const Polynom &a, const Polynom &b) {return a.Times(b, a.ms_fixedRing);}
//!
inline Polynom operator%(const Polynom &a, const Polynom &b) {return a.Modulo(b, a.ms_fixedRing);}
//!
inline Polynom operator/(const Polynom &a, const Polynom &b) {return Polynom(a.DividedBy(b, a.ms_fixedRing));}
//!
inline bool operator==(const Polynom &a, const Polynom &b)
	{return a.Equals(b, a.ms_fixedRing);}
//!
inline bool operator!=(const Polynom &a, const Polynom &b)
	{return !(a==b);}
//!
inline bool operator> (const Polynom &a, const Polynom &b)
{return a.Degree(a.ms_fixedRing) > b.Degree(b.ms_fixedRing);}