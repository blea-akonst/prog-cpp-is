#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

class Polynomial
{
private:
    std::map<int, double> polynomial; // ассоциативный массив степень - коэффициент
    unsigned int deg; // степень полинома
    unsigned int count; // количество одночленов
public:
    Polynomial(): deg(0), count(0) {};
    Polynomial(const Polynomial &);
    ~Polynomial() = default;

    Polynomial &operator = (const Polynomial &);

    friend bool operator == (const Polynomial &, const Polynomial &);
    friend bool operator != (const Polynomial &, const Polynomial &);

    Polynomial &operator + (const Polynomial &); // бинарный плюс
    Polynomial &operator - (const Polynomial &); // бинарный минус
    Polynomial &operator - (); // унарный минус
    Polynomial &operator += (const Polynomial &);
    Polynomial &operator -= (const Polynomial &);

    Polynomial &operator * (double);
    Polynomial &operator * (const Polynomial &);
    Polynomial &operator / (double);
    Polynomial &operator *= (double);
    Polynomial &operator *= (const Polynomial &);
    Polynomial &operator /= (double);

    friend std::istream &operator >> (std::istream &, Polynomial &);
    friend std::ostream &operator << (std::ostream &, Polynomial &);
    double operator [] (int);
};

std::istream& operator >> (std::istream &in, Polynomial &p)
{
    std::cout << "Enter the monomials count of your polynomial: ";
    int count;
    in >> count;

    if (count < 2)
    {
        std::cout << "It's a monomial!\n";
        exit(0);
    }

    for (int i = 0; i < count; ++i)
    {
        unsigned int cur_deg;
        double coeff;

        std::cout << "\nEnter the " << i + 1 << " monomial's degree: ";
        in >> cur_deg;

        std::cout << "Enter the " << i + 1 << " monomial's coefficient: ";
        in >> coeff;

        p.deg = std::max(cur_deg, p.deg);

        if (p.polynomial[cur_deg] == 0)
        {
            p.polynomial[cur_deg] = coeff;
            ++p.count;
        }
        else
        {
            std::cout << "Monomial with this degree is already in polynomial! ";
            p.polynomial[cur_deg] += coeff;
            std::cout << "New coefficient of " << cur_deg << " degree is: " << p.polynomial[cur_deg] << ".\n";
        }
    }

    return in;
}

std::ostream& operator << (std::ostream &out, Polynomial &p)
{
    out << "\nThe count of monomials of your polynomial: " << p.count << "\n";
    out << "The degree of your polynomial: " << p.deg << "\n";

    auto iter = p.polynomial.begin();

    std::vector<std::pair<int, double> > monomials;

    for (; iter != p.polynomial.end(); ++iter)
    {
        monomials.emplace_back(iter->first, iter->second);
    }

    std::reverse(monomials.begin(), monomials.end());

    out << "Your polynomial: ";

    for (int i = 0; i < monomials.size(); ++i)
    {
        if (i != 0)
        {
            if (monomials[i].first != 0 && monomials[i].first != 1)
            {
                if (monomials[i].second > 0) out << " + " << monomials[i].second << "x^" << monomials[i].first;
                else if (monomials[i].second < 0) out << " - " << abs(monomials[i].second) << "x^" << monomials[i].first;
            }
            else if (monomials[i].first == 0)
            {
                if (monomials[i].second > 0) out << " + " << monomials[i].second;
                else if (monomials[i].second < 0) out << " - " << abs(monomials[i].second);
            }
            else
            {
                if (monomials[i].second > 0) out << " + " << monomials[i].second << "x";
                else if (monomials[i].second < 0) out << " - " << abs(monomials[i].second) << "x";
            }
        }
        else if (monomials[i].first != 1) out << monomials[i].second << "x^" << monomials[i].first;
        else out << monomials[i].second << "x";
    }

    out << "\n";

    return out;
}

Polynomial::Polynomial(const Polynomial &f)
{
    deg = f.deg;
    count = f.count;
    polynomial = f.polynomial;
}

Polynomial &Polynomial::operator= (const Polynomial &f) = default;

bool operator==(const Polynomial &p1, const Polynomial &p2)
{
    if ( (p1.deg != p2.deg) || (p1.count != p2.count) ) return false;

    auto iter = p1.polynomial.begin();

    for (; iter != p1.polynomial.end(); ++iter)
    {
        auto iter_2 = p2.polynomial.find(iter->first);

        if (iter_2 == p2.polynomial.end())
            return false;
        else if (iter->second != iter_2->second)
            return false;

        else continue;
    }

    return true;
}

bool operator != (const Polynomial &p1, const Polynomial &p2)
{
    return (!(p1 == p2));
}

Polynomial &Polynomial::operator + (const Polynomial &p)
{
    auto *result = new Polynomial();

    result->deg = this->deg;
    result->count = this->count;

    auto iter = p.polynomial.begin();
    for (; iter != p.polynomial.end(); ++iter)
    {
        auto iter_2 = this->polynomial.find(iter->first);
        if (iter_2 == p.polynomial.end())
        {
            this->polynomial[iter->first] = (iter->second);
            if (iter->first > this->deg) this->deg = iter->first;
        }
        else if (iter->first == iter_2->first)
            result->polynomial[iter->first] = iter_2->second + iter->second;
    }

    iter = result->polynomial.end();
    --iter;
    result->deg = iter->first;
    result->count = result->polynomial.size();

    return *result;
}

Polynomial &Polynomial::operator - (const Polynomial &p)
{
    auto *result = new Polynomial();

    result->deg = this->deg;

    auto iter = p.polynomial.begin();
    for (; iter != p.polynomial.end(); ++iter)
    {
        auto iter_2 = this->polynomial.find(iter->first);
        if (iter_2 == p.polynomial.end())
        {
            this->polynomial[iter->first] = -(iter->second);
            if (iter->first > this->deg) this->deg = iter->first;
        }
        else if (iter->first == iter_2->first)
            result->polynomial[iter->first] = iter_2->second - iter->second;
    }

    iter = result->polynomial.end();
    --iter;
    result->deg = iter->first;
    result->count = result->polynomial.size();

    return *result;
}

Polynomial &Polynomial::operator - ()
{
    auto iter = polynomial.begin();

    for (; iter != polynomial.end(); ++iter)
    {
        this->polynomial[iter->first] = -(iter->second);
    }

    return *this;
}

Polynomial &Polynomial::operator += (const Polynomial &p)
{
    *this = *this + p;
    return *this;
}

Polynomial &Polynomial::operator -= (const Polynomial &p)
{
    *this = *this - p;
    return *this;
}

Polynomial &Polynomial::operator * (const double multiplier)
{
    auto *result = new Polynomial();

    auto iter = polynomial.begin();
    for (; iter != polynomial.end(); ++iter)
    {
        if (iter -> second != 0) result->polynomial[iter->first] *= multiplier;
    }

    return *result;
}

Polynomial &Polynomial::operator / (const double divider)
{
    assert(divider != 0 && "You're dividing by zero!");
    auto *result = new Polynomial();

    auto iter = polynomial.begin();
    for (; iter != polynomial.end(); ++iter)
    {
        if (iter -> second != 0) result->polynomial[iter->first] /= divider;
    }

    return *result;
}

Polynomial &Polynomial::operator *= (const double multiplier)
{
    *this = *this * multiplier;
    return *this;
}



Polynomial &Polynomial::operator /= (const double divider)
{
    *this = *this / divider;
    return *this;
}

Polynomial &Polynomial::operator * (const Polynomial &p)
{
    auto *result = new Polynomial();
    std::pair<int, double> multiply;

    auto iter_1 = this->polynomial.begin();
    std::vector<std::pair<int, double> > pol1;

    for (; iter_1 != this->polynomial.end(); ++iter_1)
    {
        pol1.emplace_back(iter_1->first, iter_1->second);
    }

    auto iter_2 = p.polynomial.begin();

    std::vector<std::pair<int, double> > pol2;
    for (; iter_2 != p.polynomial.end(); ++iter_2)
    {
        pol2.emplace_back(iter_2->first, iter_2->second);
    }

    for (auto &m: pol1)
    {
        for (auto &n: pol2)
        {
            multiply.first = m.first + n.first;
            multiply.second = m.second * n.second;
            result->polynomial[multiply.first] += multiply.second;
        }
    }

    iter_1 = result->polynomial.end();
    --iter_1;
    result->deg = iter_1->first;
    result->count = result->polynomial.size();
    return *result;
}

Polynomial &Polynomial::operator *= (const Polynomial &p)
{
    *this = *this * p;
    return *this;
}

double Polynomial::operator [] (int i)
{
    assert( (i + 1 <= count) && "i-number is greater than count of monomials!" );

    auto iter = polynomial.end();
    iter--;
    auto end = polynomial.begin();
    double coefficient = 0;

    for (int j = 0; iter != end; ++j, --iter)
    {
        if (j == i)
        {
            coefficient = iter->second;
            break;
        }
    }

    if (coefficient == 0) coefficient = end->second;

    return coefficient;
}