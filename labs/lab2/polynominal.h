#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cassert>

class Polynomial
{
private:
    std::unordered_map<int, double> polynomial; // хеш-таблица степень - коэф
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

    std::vector<std::pair<int, double> > monomials;

    for (auto &x: p.polynomial)
    {
        monomials.emplace_back(x.first, x.second);
    }

    std::sort(monomials.begin(), monomials.end(), std::greater<>()); // logN, че...(

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
    if (p1.polynomial == p2.polynomial)
        return true;
    else return false;
}

bool operator != (const Polynomial &p1, const Polynomial &p2)
{
    return (!(p1 == p2));
}

Polynomial &Polynomial::operator -= (const Polynomial &p)
{
    auto iter = p.polynomial.begin();
    unsigned int cur_deg = this->deg;
    for (; iter != p.polynomial.end(); ++iter)
    {
        auto iter_2 = this->polynomial.find(iter->first);
        if (iter_2 == p.polynomial.end())
        {
            this->polynomial[iter->first] = -(iter->second);
            if (iter->first > this->deg) this->deg = iter->first;
        }
        else if (iter->first == iter_2->first)
            this->polynomial[iter->first] = iter_2->second - iter->second;
        if (iter->first > cur_deg) cur_deg = iter->first;
    }
    this->count = this->polynomial.size();

    return *this;
}

Polynomial &Polynomial::operator += (const Polynomial &p)
{
    auto iter = p.polynomial.begin();
    unsigned int cur_deg = this->deg;
    for (; iter != p.polynomial.end(); ++iter)
    {
        auto iter_2 = this->polynomial.find(iter->first);
        if (iter_2 == p.polynomial.end())
        {
            this->polynomial[iter->first] = iter->second;
            if (iter->first > this->deg) this->deg = iter->first;
        }
        else if (iter->first == iter_2->first)
            this->polynomial[iter->first] = iter_2->second + iter->second;
        if (iter->first > cur_deg) cur_deg = iter->first;
    }
    this->count = this->polynomial.size();

    return *this;
}

Polynomial &Polynomial::operator + (const Polynomial &right)
{
    return *this += right;
}

Polynomial &Polynomial::operator - (const Polynomial &right)
{
    return *this -= right;
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


Polynomial &Polynomial::operator *= (const double multiplier)
{
    if (multiplier == 0)
    {
        this->polynomial.clear();
        this->polynomial[0] = 0;
        this->deg = 1;
        this->count = 1;
    }
    else for (auto &x: this->polynomial)
            if (x.second != 0) x.second *= multiplier;

    return *this;
}

Polynomial &Polynomial::operator /= (const double divider)
{
    assert(divider != 0 && "You're dividing by zero!");

    for (auto &x: this->polynomial)
        if (x.second != 0) x.second /= divider;

    return *this;
}

Polynomial &Polynomial::operator * (const double multiplier)
{
    return *this *= multiplier;
}


Polynomial &Polynomial::operator / (const double divider)
{
    return *this /= divider;
}

Polynomial &Polynomial::operator *= (const Polynomial &p)
{
    std::pair<int, double> multiply;

    auto result = new Polynomial();
    result->deg = 0;

    for (auto &x: this->polynomial)
        for (auto &y: p.polynomial)
        {
            multiply.first = x.first + y.first;
            multiply.second = x.second * y.second;
            result->polynomial[multiply.first] += multiply.second;
            if (result->deg < multiply.first) result->deg = multiply.first;
        }

    result->count = result->polynomial.size();

    *this = *result;
    delete result;

    return *this;
}

Polynomial &Polynomial::operator * (const Polynomial &p)
{
    return *this *= p;
}

double Polynomial::operator [] (int i)
{
    return polynomial[i];
}