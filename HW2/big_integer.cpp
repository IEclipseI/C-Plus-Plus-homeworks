#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "big_integer.h"

using namespace std;

big_integer::big_integer() : sign(false), number{0} {
}

big_integer::big_integer(big_integer const &other) : sign(other.sign), number(other.number) {
}

big_integer::big_integer(int a) {
    if (a >= 0) {
        sign = false;
        number.push_back(a);
    } else {
        sign = true;
        number.push_back(-(int64_t) a);
    }
}

big_integer::big_integer(vector<uint32_t> number, bool sign): sign(sign), number(number) {
}

big_integer::big_integer(string const &str) {
    sign = str[0] == '-';
    number.push_back(0);
    for (size_t i = static_cast<size_t>(sign); i < str.length(); ++i) {
        mul_long_short(10);
        add_long_short(static_cast<uint32_t>(str[i] - '0'));
    }
    delete_zeroes();
}

big_integer &
big_integer::mul_long_short(uint32_t a) {
    uint64_t carry = 0;
    for (size_t i = 0; i < number.size() || carry; ++i) {
        if (i == number.size()) {
            number.push_back(0);
        }
        uint64_t tmp = (uint64_t) number[i] * a + carry;
        carry = tmp >> 32;
        number[i] = (int) tmp;
    }
    return *this;
}

big_integer &
big_integer::add_long_short(uint32_t a) {
    uint64_t carry = 0;
    uint64_t tmp = (uint64_t) number[0] + a;
    carry = tmp >> 32;
    number[0] = (int) tmp;
    size_t i = 1;
    while (carry) {
        if (i == number.size()) {
            number.push_back(0);
        }
        uint64_t tmp = (uint64_t) number[i] + carry;
        carry = tmp >> 32;
        number[i] = (int) tmp;
        i++;
    }
    return *this;
}

uint32_t
big_integer::div_long_short(uint32_t a) {
    uint64_t carry = number[number.size() - 1] % a;
    number[number.size() - 1] /= a;

    for (int i = (int) number.size() - 2; i >= 0; --i) {
        uint64_t cur = number[i] + carry * (UINT32_MAX + (uint64_t) 1);
        number[i] = cur / a;
        carry = cur % a;
    }
    while (number.size() > 1 && number.back() == 0) {
        number.pop_back();
    }
    return carry;
}

bool
big_integer::is_zero() const {
    return number.size() == 1 && number[0] == 0;
}

void
big_integer::delete_zeroes() {
    while (number.size() > 1 && number.back() == 0) {
        number.pop_back();
    }
    if (is_zero()) {
        sign = false;
    }
}

big_integer &
big_integer::operator=(big_integer const &other) {
    this->number = other.number;
    this->sign = other.sign;
    return *this;
}

big_integer &
big_integer::operator+=(big_integer const &rhs) {
    int carry = 0;
    unsigned long max = std::max(rhs.number.size(), this->number.size());
    if (this->sign == rhs.sign) {
        uint64_t tmp;
        for (vector<uint32_t>::size_type i = 0; i < max || carry; ++i) {
            tmp = this->number[i];
            if (i == this->number.size()) {
                this->number.push_back(0);
            }
            this->number[i] += carry + (i < rhs.number.size() ? rhs.number[i] : 0);
            carry = this->number[i] < tmp;
        }
        return *this;
    } else {
        *this -= big_integer(rhs.number, !rhs.sign);
        return *this;
    }
}

big_integer &
big_integer::operator-=(big_integer const &rhs) {
    if (this->sign != rhs.sign) {
        *this += big_integer(rhs.number, !rhs.sign);
    } else {
        big_integer first(this->number, false);
        big_integer second(rhs.number, false);
        if (first < second) {
            swap(first, second);
            first.sign = !this->sign;
        } else {
            first.sign = sign;
        }
        int carry = 0;
        for (size_t i = 0; i < second.number.size() || carry; ++i) {
            uint32_t tmp = first.number[i];
            first.number[i] -= carry + (i < second.number.size() ? second.number[i] : 0);
            carry = first.number[i] > tmp;
        }
        first.delete_zeroes();
        *this = first;
    }
    return *this;
}

big_integer &
big_integer::operator*=(big_integer const &rhs) {
    this->sign = this->sign != rhs.sign;
    vector<uint32_t> result(rhs.number.size() + this->number.size() + 2);
    this->number.push_back(0);
    uint64_t tmp = 0;
    uint32_t carry = 0;
    uint32_t carry2;
    for (size_t i = 0; i < rhs.number.size(); ++i) {
        for (size_t j = 0; j < this->number.size() || carry; ++j) {
            tmp = (uint64_t) this->number[j] * rhs.number[i] + carry;
            carry2 = ((uint64_t) result[j + i] + ((uint32_t) tmp)) >> 32;
            result[j + i] += (uint32_t) tmp;
            carry = (uint32_t) (tmp >> 32) + carry2;
        }
    }
    this->number = result;
    delete_zeroes();
    return *this;
}

big_integer &
big_integer::operator/=(big_integer const &rhs) {
    *this = *this / rhs;
    return *this;
}

big_integer &
big_integer::operator%=(big_integer const &rhs) {
    big_integer tmp(*this / rhs);
    *this = *this - tmp * rhs;
    return *this;
}

big_integer &
big_integer::operator&=(big_integer const &rhs) {
    size_t max_len = min(this->number.size(), rhs.number.size());
    if (this->sign == rhs.sign) {
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] &= rhs.number[i];
        }
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();
    } else {
        big_integer r(rhs);
        if (this->sign) {
            for (unsigned int &i : this->number) {
                i = ~i;
            }
            this->sign = !this->sign;
            ++*this;
        } else {
            for (unsigned int &i : r.number) {
                i = ~i;
            }
            r.sign = !r.sign;
            ++r;
            r.sign = !r.sign;
        }
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] &= r.number[i];
        }
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();

    }
    return *this;
}

big_integer &
big_integer::operator|=(big_integer const &rhs) {
    vector<uint32_t>::size_type max_len = max(this->number.size(), rhs.number.size());
    for (size_t j = number.size(); j < max_len; ++j) {
        number.push_back(0);
    }
    big_integer r(rhs);
    for (size_t j = r.number.size(); j < max_len; ++j) {
        r.number.push_back(0);
    }
    if (this->sign == rhs.sign && !sign) {
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] |= rhs.number[i];
        }
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();
    } else {
        if (this->sign) {
            for (unsigned int &i : this->number) {
                i = ~i;
            }
            sign = !sign;
            ++*this;
        }
        if (r.sign) {
            for (unsigned int &i : r.number) {
                i = ~i;
            }
            r.sign = !r.sign;
            ++r;
        }
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] |= r.number[i];
        }
        for (unsigned int &i : this->number) {
            i = ~i;
        }
        ++*this;
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();
        sign = true;
    }
    return *this;
}

big_integer &
big_integer::operator^=(big_integer const &rhs) {
    vector<uint32_t>::size_type max_len = max(this->number.size(), rhs.number.size());
    for (size_t j = number.size(); j < max_len; ++j) {
        number.push_back(0);
    }
    big_integer r(rhs);
    for (size_t j = r.number.size(); j < max_len; ++j) {
        r.number.push_back(0);
    }
    if (this->sign == rhs.sign) {
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] ^= rhs.number[i];
        }
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();
    } else {
        if (this->sign) {
            for (unsigned int &i : this->number) {
                i = ~i;
            }
            sign = !sign;
            ++*this;
        }
        if (r.sign) {
            for (unsigned int &i : r.number) {
                i = ~i;
            }
            r.sign = !r.sign;
            ++r;
        }
        for (size_t i = 0; i < max_len; ++i) {
            this->number[i] ^= r.number[i];
        }
        for (unsigned int &i : this->number) {
            i = ~i;
        }
        ++*this;
        while (max_len > this->number.size()) {
            this->number.pop_back();
        }
        this->delete_zeroes();
        sign = true;
    }
    return *this;
}

big_integer &
big_integer::operator<<=(int rhs) {
    int count = rhs / 32;
    vector<uint32_t> tmp;
    for (int i = 0; i < count; ++i) {
        tmp.push_back(0);
    }
    tmp.push_back(0);
    count = rhs % 32;
    for (size_t j = 0; j < this->number.size(); ++j) {
        tmp.push_back(0);
        tmp[tmp.size() - 2] += (number[j] << count);
        uint64_t t = number[j];
        tmp[tmp.size() - 1] += (t >> (32 - count));
    }
    this->number = tmp;
    delete_zeroes();
    return *this;
}

big_integer &
big_integer::operator>>=(int rhs) {
    if (!sign) {
        vector<uint32_t> tmp;
        tmp.push_back(0);
        uint64_t t = number[rhs / 32];
        uint32_t mov = (rhs % 32);
        tmp[0] += ((uint32_t) (t >> mov));
        for (size_t j = (rhs / 32 + 1); j < this->number.size(); ++j) {
            t = number[j];
            tmp.push_back(0);
            tmp[tmp.size() - 2] += ((uint32_t) (t << (32 - mov)));
            tmp[tmp.size() - 1] += (t >> mov);
        }
        this->number = tmp;

    } else {
        this->number.push_back(0);
        vector<int32_t> tmp(this->number.size());
        vector<int32_t> tm(this->number.size(), 0);
        for (unsigned int &i : this->number) {
            i = ~i;
        }
        --*this;
        for (size_t j = 0; j < tmp.size(); ++j) {
            tmp[j] = this->number[j];
        }
        int shift = rhs / 32;
        int64_t t = tmp[shift];
        uint64_t t_plus = t;
        ++shift;
        uint32_t mov = rhs % 32;
        tm[0] += (t >> mov);
        for (size_t j = rhs / 32 + 1; j < tmp.size(); ++j) {
            t = tmp[j];
            t_plus = t;
            tm[shift - j] |= (t_plus << (32 - mov));
            tm[shift - j + 1] |= (t >> (32 - mov));
        }
        for (size_t j = 0; j < tmp.size(); ++j) {
            this->number[j] = tm[j];
        }
        delete_zeroes();
        for (unsigned int &i : this->number) {
            i = ~i;
        }
        --*this;
    }
    delete_zeroes();
    return *this;
}

big_integer
big_integer::operator+() const {
    return *this;
}

big_integer
big_integer::operator-() const {
    big_integer p(*this);
    if (is_zero()) {
        return p;
    }
    return big_integer(this->number, !sign);
}

big_integer
big_integer::operator~() const {
    return big_integer(big_integer(this->number, !sign)) - 1;
}

big_integer &
big_integer::operator++() {
    uint32_t carry = 0;
    uint32_t tmp = number[0];
    size_t i = 0;
    if (!sign) {
        number[i]++;
        carry = number[i] < tmp;
        while (carry) {
            i++;
            if (i == number.size()) {
                number.push_back(0);
            }
            tmp = number[i];
            number[i]++;
            carry = number[i] < tmp;
        }
    } else {
        if (number.size() == 1 && number[0] == 1) {
            number[0] = 0;
            sign = false;
            return *this;
        }
        number[i]--;
        carry = tmp < number[i];
        while (carry) {
            tmp = number[++i]--;
            carry = number[i] < tmp;
        }
        delete_zeroes();
    }
    return *this;
}

big_integer
big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &
big_integer::operator--() {
    sign = !sign;
    ++*this;
    sign = !sign;
    return *this;
}

big_integer
big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer
operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer
operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer
operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer
operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer
operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer
operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer
operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

//
big_integer
operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer
operator>>(big_integer a, int b) {
    return a >>= b;
}

bool
operator==(big_integer const &a, big_integer const &b) {
    if (a.is_zero() && b.is_zero()) {
        return true;
    }
    if (a.number.size() != b.number.size()) {
        return false;
    }
    size_t size = a.number.size();
    for (size_t i = 0; i < size; ++i) {
        if (a.number[i] != b.number[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign;
    } else {
        if (a.sign) {
            if (a.number.size() != b.number.size()) {
                return a.number.size() > b.number.size();
            } else {
                size_t i = 1;
                while (a.number[a.number.size() - i] == b.number[a.number.size() - i]) {
                    i++;
                    if (i > a.number.size()) {
                        return false;
                    }
                }
                return a.number[a.number.size() - i] > b.number[b.number.size() - i];
            }
        } else {
            if (a.number.size() != b.number.size()) {
                return a.number.size() < b.number.size();
            } else {
                size_t i = 1;
                while (a.number[a.number.size() - i] == b.number[a.number.size() - i]) {
                    i++;
                    if (i > a.number.size()) {
                        return false;
                    }
                }
                return a.number[a.number.size() - i] < b.number[b.number.size() - i];
            }
        }
    }
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return !a.sign;
    } else {
        if (a.sign) {
            if (a.number.size() != b.number.size()) {
                return a.number.size() < b.number.size();
            } else {
                size_t i = 1;
                while (a.number[a.number.size() - i] == b.number[a.number.size() - i]) {
                    i++;
                    if (i > a.number.size()) {
                        return false;
                    }
                }
                return a.number[a.number.size() - i] < b.number[b.number.size() - i];
            }
        } else {
            if (a.number.size() != b.number.size()) {
                return a.number.size() > b.number.size();
            } else {
                size_t i = 1;
                while (a.number[a.number.size() - i] == b.number[a.number.size() - i]) {
                    i++;
                    if (i > a.number.size()) {
                        return false;
                    }
                }
                return a.number[a.number.size() - i] > b.number[b.number.size() - i];
            }
        }
    }
}

string
to_string(big_integer const &a) {
    string res;
    vector<uint32_t> copy(a.number);
    big_integer c(copy, a.sign);
    if (c.number.size() == 1 && c.number[0] == 0) {
        return "0";
    }
    while (c.number.size() != 1 || c.number[0] != 0) {
        res.push_back((char) ('0' + c.div_long_short(10)));
    }
    if (c.sign) {
        res.push_back('-');
    }
    string ans;
    for (int i = res.length() - 1; i >= 0; --i) {
        ans.push_back(res[i]);
    }
    return ans;
}

bool
operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool
operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

ostream &
operator<<(ostream &s, big_integer const &a) {
    return s << to_string(a);
}

bool
smaller(vector<uint32_t> &r, vector<uint32_t> &q) {
    for (size_t i = r.size(); i > 0; --i) {
        if (r[i - 1] != q[i - 1]) {
            return r[i - 1] < q[i - 1];
        }
    }

    return 0;
}

void
difference(vector<uint32_t> &r, vector<uint32_t> &q) {
    int64_t diff;
    uint32_t borrow = 0;
    for (size_t i = 0; i < q.size(); ++i) {
        diff = (int64_t) r[i] - q[i] - borrow;
        r[i] = r[i] - q[i] - borrow;
        borrow = diff < 0;
    }
}

void
product(vector<uint32_t> &x, vector<uint32_t> &y, uint32_t k) {
    uint64_t carry = 0, m = y.size(), temp;
    x.resize(m + 1);
    for (size_t i = 0; i < m; ++i) {
        temp = (uint64_t) y[i] * k + carry;
        x[i] = temp;
        carry = temp >> 32;
    }
    x[m] = carry;
}

void
quotient(vector<uint32_t> &x, vector<uint32_t> &y, uint32_t k) {
    uint64_t carry = 0, m = y.size(), temp;
    for (unsigned int &j : x) {
        j = 0;
    }
    for (int i = m - 1; i >= 0; --i) {
        temp = carry * (UINT32_MAX + 1ll) + y[i];
        x[i] = temp / k;
        carry = temp % k;
    }
}

big_integer
operator/(big_integer a, big_integer const &b) {
    big_integer xx(a.number, false);
    big_integer yy(b.number, false);

    if (yy.is_zero()) {
        throw runtime_error("Division by zero");
    }
    if (xx < yy) {
        return 0;
    }

    if (yy.number.size() == 1) {
        vector<uint32_t> res(a.number.size(), 0);
        quotient(res, xx.number, yy.number[0]);
        big_integer ans(res, a.sign ^ b.sign);
        ans.delete_zeroes();
        return ans;
    } else {
        vector<uint32_t> x, y;
        xx.delete_zeroes();
        yy.delete_zeroes();
        x = xx.number;
        y = yy.number;
        size_t n = x.size(), m = y.size();
        uint32_t f = (uint32_t) ((UINT32_MAX + 1ull) / (y.back() + 1ull));
        product(x, x, f);
        product(y, y, f);
        if (x[x.size() - 1] == 0) {
            x.pop_back();
        }
        if (y[y.size() - 1] == 0) {
            y.pop_back();
        }
        size_t len = n - m + 1;
        vector<uint32_t> res(len, 0), r(m + 1, 0), q(m + 1, 0);
        for (size_t i = 0; i < m; ++i) {
            r[i] = x[n + i - m];
        }
        r[m] = x.size() > n ? x[n] : 0;
        uint32_t divisor = y.back();
        for (size_t i = 0; i < len; ++i) {
            r[0] = x[n - i - m];
            size_t rd = len - 1 - i;
            uint64_t trial = ((uint64_t(r[m]) << 32) + r[m - 1]) / divisor;
            uint32_t tq = trial > UINT32_MAX ? UINT32_MAX : trial;
            product(q, y, tq);
            while (smaller(r, q)) {
                product(q, y, --tq);
            }
            difference(r, q);
            for (size_t j = m; j > 0; --j) {
                r[j] = r[j - 1];
            }
            res[rd] = tq;
        }
        big_integer ans(res, a.sign ^ b.sign);
        ans.delete_zeroes();
        return ans;
    }
}
