#include "Polynom.h"
#include <sstream>

#include "misc.h"
using CryptoPP::ModularArithmetic;
using CryptoPP::STDMAX;
using CryptoPP::STDMIN;

using std::ostream;

Polynom& Polynom::operator=(const Polynom& t) {
	if (this != &t) {
		m_coefficients.resize(t.m_coefficients.size());
		ms_fixedRing = t.ms_fixedRing;
		for (unsigned int i=0; i<m_coefficients.size(); i++)
			m_coefficients[i] = t.m_coefficients[i];
	}
	#ifdef _DEBUG
		else {
			cout << "Error!File: " << __FILE__ << "Line:" << __LINE__ << endl;
		}
	#endif

	return *this;
};

Polynom Polynom::Plus(const Polynom &t, const ModularArithmetic &ring) const {
	unsigned int i;
	unsigned int count = CoefficientCount(ring);
	unsigned int tCount = t.CoefficientCount(ring);

	if (count > tCount) {
		Polynom result(ring, count);

		for (i=0; i<tCount; i++)
			result.m_coefficients[i] = ring.Add(m_coefficients[i], t.m_coefficients[i]);
		for (; i<count; i++)
			result.m_coefficients[i] = m_coefficients[i];

		return result;
	}
	else
	{
		Polynom result(ring, tCount);

		for (i=0; i<count; i++)
			result.m_coefficients[i] = ring.Add(m_coefficients[i], t.m_coefficients[i]);
		for (; i<tCount; i++)
			result.m_coefficients[i] = t.m_coefficients[i];

		return result;
	}
}

Polynom Polynom::Minus(const Polynom &t, const ModularArithmetic &ring) const {
	unsigned int i;
	unsigned int count = CoefficientCount(ring);
	unsigned int tCount = t.CoefficientCount(ring);

	if (count > tCount)
	{
		Polynom result(ring, count);

		for (i=0; i<tCount; i++)
			result.m_coefficients[i] = ring.Subtract(m_coefficients[i], t.m_coefficients[i]);
		for (; i<count; i++)
			result.m_coefficients[i] = m_coefficients[i];

		return result;
	}
	else
	{
		Polynom result(ring, tCount);

		for (i=0; i<count; i++)
			result.m_coefficients[i] = ring.Subtract(m_coefficients[i], t.m_coefficients[i]);
		for (; i<tCount; i++)
			result.m_coefficients[i] = ring.Inverse(t.m_coefficients[i]);

		return result;
	}
}

Polynom Polynom::Times(const Polynom &t, const ModularArithmetic &ring) const {
	if (IsZero(ring) || t.IsZero(ring))
		return Polynom();

	unsigned int count1 = CoefficientCount(ring), count2 = t.CoefficientCount(ring);
	Polynom result(ring, count1 + count2 - 1);

	for (unsigned int i=0; i<count1; i++)
		for (unsigned int j=0; j<count2; j++)
			ring.Accumulate(result.m_coefficients[i+j], ring.Multiply(m_coefficients[i], t.m_coefficients[j]));

	return result;
}

#ifdef _DEBUG
ostream& operator<<(ostream& out, const Polynom &a) {
	for (size_t i = 0; i < a.m_coefficients.size(); ++i) 
		cout << a.m_coefficients[i] << " \n";
	return out;
}
#endif

void Polynom::Divide(Polynom &r, Polynom&q, const Polynom &a, const Polynom &d, const ModularArithmetic &ring) {
	unsigned int i = a.CoefficientCount(ring);
	const int dDegree = d.Degree(ring);

	r = a;
	q.ms_fixedRing = a.ms_fixedRing;
	q.m_coefficients.resize(STDMAX(0, int(i - dDegree)));
	while (i > (unsigned int)dDegree) {
		--i;
		q.m_coefficients[i-dDegree] = ring.Divide(r.m_coefficients[i], d.m_coefficients[dDegree]);
		for (int j=0; j<=dDegree; j++)
			ring.Reduce(r.m_coefficients[i-dDegree+j], ring.Multiply(q.m_coefficients[i-dDegree], d.m_coefficients[j]));
	}
	r.CoefficientCount(ring);	// resize r.m_coefficients
}

Polynom Polynom::Modulo(const Polynom& t, const ModularArithmetic &ring) const {
	Polynom remainder, quotient;
	Divide(remainder, quotient, *this, t, ring);
	return remainder;
}

Polynom Polynom::DividedBy(const Polynom& t, const ModularArithmetic &ring) const {
	Polynom remainder, quotient;
	Divide(remainder, quotient, *this, t, ring);
	return quotient;
}

Polynom Polynom::powmod(Polynom a, Integer k, const Polynom &n) {
	Polynom b("x^0", a.ms_fixedRing);

	while (k != 0) {
		if (k % 2==0) {
			k /= 2;
			a = (a * a) % n;
		}
		else {
			k--;
			b = (b * a) % n;
		}
	}
	return b;
}

void Polynom::FromStr(const char *str, const ModularArithmetic &ring) {
	std::istringstream in((char *)str);
	bool positive = true;
	Integer coef;
	unsigned int power;

	while (in)
	{
		std::ws(in);
		if (in.peek() == 'x')
			coef = ring.MultiplicativeIdentity();
		else
			in >> coef;

		std::ws(in);
		if (in.peek() == 'x')
		{
			in.get();
			std::ws(in);
			if (in.peek() == '^')
			{
				in.get();
				in >> power;
			}
			else
				power = 1;
		}
		else
			power = 0;

		if (!positive)
			coef = ring.Inverse(coef);

		SetCoefficient(power, coef, ring);

		std::ws(in);
		switch (in.get())
		{
		case '+':
			positive = true;
			break;
		case '-':
			positive = false;
			break;
		default:
			return;		// something's wrong with the input string
		}
	}
}

void Polynom::SetCoefficient(unsigned int i, const Integer &value, const ModularArithmetic &ring) {
	if (i >= m_coefficients.size())
		m_coefficients.resize(i+1, ring.Identity());
	m_coefficients[i] = value;
}

unsigned int Polynom::CoefficientCount(const ModularArithmetic &ring) const {
	unsigned count = m_coefficients.size();
	while (count && ring.Equal(m_coefficients[count-1], ring.Identity()))
		count--;
	const_cast<std::vector<Integer> &>(m_coefficients).resize(count);

	return count;
}

Integer Polynom::GetCoefficient(unsigned int i, const ModularArithmetic &ring) const {
	return (i < m_coefficients.size()) ? m_coefficients[i] : ring.Identity();
}

bool Polynom::Equals(const Polynom& t, const ModularArithmetic &ring) const {
	unsigned int count = CoefficientCount(ring);

	if (count != t.CoefficientCount(ring))
		return false;

	for (unsigned int i=0; i<count; i++)
		if (!ring.Equal(m_coefficients[i], t.m_coefficients[i]))
			return false;

	return true;
}
