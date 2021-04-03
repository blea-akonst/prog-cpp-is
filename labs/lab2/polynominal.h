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
    int deg; // степень полинома
public:
    Polynomial(): deg(0) {};
    Polynomial(const Polynomial &);
    ~Polynomial() = default;

    Polynomial &operator = (const Polynomial &);

    bool operator == (const Polynomial &) const;
    bool operator != (const Polynomial &) const;

    Polynomial operator + (const Polynomial &) const; // бинарный плюс
    Polynomial operator - (const Polynomial &) const; // бинарный минус
    Polynomial operator - () const; // унарный минус
    Polynomial &operator += (const Polynomial &);
    Polynomial &operator -= (const Polynomial &);

    Polynomial operator * (double) const;
    Polynomial operator * (const Polynomial &) const;
    Polynomial operator / (double) const;
    Polynomial &operator *= (double);
    Polynomial &operator *= (const Polynomial &);
    Polynomial &operator /= (double);

    friend std::istream &operator >> (std::istream &, Polynomial &);
    friend std::ostream &operator << (std::ostream &, Polynomial &);
    double &operator [] (int); // константным не совсем получилось ( : для этого нужна конст мапа (а это нам не надо)
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
        int cur_deg;
        double coeff;

        std::cout << "\nEnter the " << i + 1 << " monomial's degree: ";
        in >> cur_deg;

        std::cout << "Enter the " << i + 1 << " monomial's coefficient: ";
        in >> coeff;

        p.deg = std::max(cur_deg, p.deg);

        if (p.polynomial[cur_deg] == 0)
        {
            p.polynomial[cur_deg] = coeff;
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
    out << "\nThe count of monomials of your polynomial: " << p.polynomial.size() << "\n";
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
    polynomial = f.polynomial;
}

Polynomial &Polynomial::operator= (const Polynomial &f) = default;

bool Polynomial::operator==(const Polynomial &p) const
{
    if (this->polynomial == p.polynomial)
        return true;
    else return false;
}

bool Polynomial::operator != (const Polynomial &p) const
{
    return (!(*this == p));
}

Polynomial &Polynomial::operator -= (const Polynomial &p)
{
    for (auto const &monom_sec: p.polynomial)
    {
        this->polynomial[monom_sec.first] -= monom_sec.second;
        if (this->deg < monom_sec.first) this->deg = monom_sec.first;
    }

    return *this;
}

Polynomial &Polynomial::operator += (const Polynomial &p)
{
    for (auto const &monom_sec: p.polynomial)
    {
        this->polynomial[monom_sec.first] += monom_sec.second;
        if (this->deg < monom_sec.first) this->deg = monom_sec.first;
    }

    return *this;
}

Polynomial Polynomial::operator + (const Polynomial &right) const
{
    Polynomial result(*this);
    result += right;
    return result;
}

Polynomial Polynomial::operator - (const Polynomial &right) const
{
    Polynomial result(*this);
    result += right;
    return result;
}

Polynomial Polynomial::operator - () const
{
    Polynomial result (*this);

    for (auto &monom: result.polynomial)
    {
        monom.second = -(monom.second);
    }

    return result;
}


Polynomial &Polynomial::operator *= (const double multiplier)
{
    if (multiplier == 0)
    {
        this->polynomial.clear();
        this->polynomial[0] = 0;
        this->deg = 1;
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

Polynomial Polynomial::operator * (const double multiplier) const
{
    Polynomial result(*this);
    result *= multiplier;
    return result;
}


Polynomial Polynomial::operator / (const double divider) const
{
    Polynomial result(*this);
    result *= divider;
    return result;
}

Polynomial &Polynomial::operator *= (const Polynomial &p)
{
    std::pair<int, double> multiply;

    Polynomial result;

    for (auto const &x: this->polynomial)
        for (auto const &y: p.polynomial)
        {
            multiply.first = x.first + y.first;
            multiply.second = x.second * y.second;
            result.polynomial[multiply.first] += multiply.second;
            if (result.deg < multiply.first) result.deg = multiply.first;
        }

    *this = result;

    return *this;
}

Polynomial Polynomial::operator * (const Polynomial &p) const
{
    Polynomial result(*this);
    result *= p;
    return result;
}

double &Polynomial::operator [] (const int i)
{
    return polynomial[i - 1];
}