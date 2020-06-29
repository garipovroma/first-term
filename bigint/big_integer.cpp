#include "big_integer.h"

big_integer::big_integer() {
    sign = true;
    mas = std::vector<uint32_t>();
}

big_integer::~big_integer() = default;

big_integer::big_integer(int a) {
    sign = true;
    if (a == 0) {
        return;
    }
    long long x = a;
    mas.push_back((x > 0) ? x : -x);
    if (x < 0) {
        *this = -(*this);
    }
    sign = (x > 0);
}

big_integer& big_integer::shrink_to_fit() {
    uint32_t to_delete = (sign ? 0 : UINT32_MAX);
    int32_t nw_size = mas.size();
    while (nw_size > 0 && mas[nw_size - 1] == to_delete) {
        nw_size--;
    }
    mas.resize(nw_size);
    return *this;
}

void swap(big_integer& a, big_integer& b) {
    std::swap(a.mas, b.mas);
    std::swap(a.sign, b.sign);
}

void big_integer::fill(size_t size) {
    if (size < mas.size()) return;
    size_t iterations_num = size - mas.size();
    uint32_t to_fill = (sign ? 0 : UINT32_MAX);
    for (size_t i = 0; i < iterations_num; i++) {
        mas.push_back(to_fill);
    }
}

big_integer::big_integer(const big_integer& other) {
    mas = other.mas;
    sign = other.sign;
}

big_integer& big_integer::operator=(const big_integer& other) {
    big_integer copy = big_integer(other);
    swap(copy, *this);
    return *this;
}

big_integer::big_integer(std::string const& str) : big_integer() {
    if (str.empty()) {
        return;
    }
    int loop_beg = (str[0] == '-' ? 1 : 0);
    big_integer ten = 10;
    big_integer cur;
    for (size_t i = loop_beg; i != str.size(); i++) {
        cur = int(str[i] - '0');
        *this *= ten;
        *this += cur;
    }
    if (loop_beg == 1) {
        *this = -(*this);
    }
}

std::string to_string(const big_integer& a) {
    std::string res;
    big_integer x = (a.sign ? a : -a);
    big_integer ten = 10;
    int digit;
    while (x.mas.size() != 0) {
        big_integer cur = x % ten;
        if (cur.mas.size() == 0) {
            digit = cur.sign ? 0 : UINT32_MAX;
        } else {
            digit = cur.mas[0];
        }
        res.push_back('0' + digit);
        x /= ten;
    }
    if (res.empty()) {
        res = "0";
    }
    if (!a.sign) {
        res.push_back('-');
    }
    std::reverse(res.begin(), res.end());
    return res;
}

big_integer big_integer::operator+() const {
    big_integer res;
    res.mas = mas;
    res.sign = sign;
    return res;
}

big_integer& big_integer::operator+=(const big_integer& rhs) {
    size_t sz = std::max(rhs.mas.size(), mas.size()) + 1;
    fill(sz);
    uint64_t rem = 0;
    for (size_t i = 0; i < sz; i++) {
        uint64_t cur = rem + (*this)[i] + rhs[i];
        mas[i] = static_cast<uint32_t>(cur);
        rem = (cur >> 32u);
    }
    sign = !(mas.back() >> 31u);
    return shrink_to_fit();
}

big_integer& big_integer::operator-=(const big_integer& rhs) {
    return ((*this) += (-rhs));
}

big_integer big_integer::operator~() const {
    big_integer res;
    res.mas = mas;
    res.sign = !(sign);
    for (size_t i = 0; i < mas.size(); i++) {
        res.mas[i] = (mas[i] ^ UINT32_MAX);
    }
    return res.shrink_to_fit();
}

big_integer big_integer::operator-() const {
    if ((*this) == 0) {
        return *this;
    }
    big_integer res = ~(*this) + 1;
    return res;
}

big_integer& big_integer::operator++() {
    return ((*this) += 1);
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    ++*this;
    return res;
}

big_integer& big_integer::operator--() {
    return ((*this) -= 1);
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    --*this;
    return res;
}

big_integer operator+(big_integer a, const big_integer& b) {
    a += b;
    return a;
}

big_integer operator-(big_integer a, const big_integer& b) {
    a -= b;
    return a;
}

bool operator==(const big_integer& a, const big_integer& b) {
    return (a.sign == b.sign && a.mas == b.mas);
}

bool operator!=(const big_integer& a, const big_integer& b) {
    return !(a == b);
}

bool operator>(const big_integer& a, const big_integer& b) {
    return !(a <= b);
}

bool operator<=(const big_integer& a, const big_integer& b) {
    if (a.sign != b.sign) {
        return (b.sign);
    }
    big_integer x = a;
    x.shrink_to_fit();
    big_integer y = b;
    y.shrink_to_fit();
    if (x.mas.size() != y.mas.size()) {
        return (x.mas.size() < y.mas.size());
    }
    std::reverse(x.mas.begin(), x.mas.end());
    std::reverse(y.mas.begin(), y.mas.end());
    return (x.mas <= y.mas);
    /*for (int32_t i = x.mas.size() - 1; i >= 0; i--) {
        if (x.mas[i] != y.mas[i])
            return (x.mas[i] <= y.mas[i]);
    }
    return true;*/
}

bool operator<(const big_integer& a, const big_integer& b) {
    return !(b <= a);
}

bool operator>=(const big_integer& a, const big_integer& b) {
    return (b <= a);
}

big_integer& big_integer::mul(uint32_t rhs) {
    uint64_t rem = 0;
    uint64_t to_mul = rhs;
    for (size_t i = 0; i < mas.size(); i++) {
        uint64_t cur = static_cast<uint64_t>(mas[i]) * to_mul + rem;
        mas[i] = static_cast<uint32_t>(cur);
        rem = (cur >> 32u);
    }
    if (rem != 0) {
        mas.push_back(rem);
    }
    return shrink_to_fit();
}

std::vector<uint32_t> multiply(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) {
    size_t n = a.size(), m = b.size();
    std::vector<uint32_t> res(n + m);
    for (size_t i = 0; i < n; i++) {
        uint64_t rem = 0;
        for (size_t j = 0; j < m; j++) {
            uint64_t cur = static_cast<uint64_t>(a[i]) * static_cast<uint64_t>(b[j]) + res[i + j] + rem;
            res[i + j] = static_cast<uint32_t>(cur);
            rem = (cur >> 32u);
        }
        if (rem != 0) {
            res[i + m] += static_cast<uint32_t>(rem);
        }
    }
    return res;
}

big_integer &big_integer::operator*=(const big_integer &rhs) {
    if (rhs == 0) {
        *this = 0;
        return *this;
    }
    std::vector<uint32_t> new_mas;
    bool new_sign = true;
    if (sign != rhs.sign) {
        new_mas = (sign ? multiply(mas, (-rhs).mas) : multiply((-(*this)).mas, rhs.mas));
        new_sign = false;
    } else {
        new_mas = (sign ? multiply(mas, rhs.mas) : multiply((-(*this)).mas, (-rhs).mas));
    }
    sign = true;
    mas = new_mas;
    shrink_to_fit();
    if (!new_sign) {
        *this = -(*this);
    }
    return *this;
}

big_integer operator*(big_integer a, const big_integer& b) {
    a *= b;
    return a;
}

big_integer big_integer::div(uint32_t b) {
    uint64_t rem = 0;
    big_integer res;
    res.fill(mas.size());
    for (int i = mas.size() - 1; i >= 0; i--) {
        res.mas[i] = static_cast<uint32_t>(((*this)[i] + (rem << 32u)) / b);
        rem = ((*this)[i] + (rem << 32u)) % b;
    }
    return res.shrink_to_fit();
}

big_integer& big_integer::operator/=(const big_integer& rhs) {
    big_integer a = (sign ? *this : -(*this));
    big_integer b = (rhs.sign ? rhs : -rhs);
    bool new_sign = (sign != rhs.sign);
    if (b <= a && b.mas.size() > 1) {
        big_integer bx;
        a.mas.push_back(0);
        size_t n = a.mas.size();
        size_t m = b.mas.size();
        *this = 0;
        fill(n - m + 1);
        __uint128_t y = b[m - 1], z = b[m - 2];
        int32_t N = n - m - 1;
        for (int32_t t = 0; t <= N; t++) {
            __uint128_t v = a[n - t - 1], w = a[n - t - 2], x = a[n - t - 3];
            __uint128_t trial = (x ^ (w << 32u) ^ (v << 64u)) / ((y << 32u) ^ z);
            uint32_t cur = std::min(static_cast<uint32_t>(trial), UINT32_MAX);
            bx = b;
            bx.mul(cur);
            bool decrease = false;
            int32_t i = n - m - t - 1;
            for (int32_t j = m + i, k = j - i; j >= i; j--, k--) {
                if (a[j] != bx[k]) {
                    decrease = a[j] < bx[k];
                    break;
                }
            }
            if (decrease) {
                cur--;
                bx -= b;
            }
            mas[i] = cur;
            uint64_t tmp;
            cur = 0;
            for (size_t j = i; j <= i + m; j++) {
                tmp = (static_cast<uint64_t>(a[j]) - bx[j - i] - cur);
                cur = (bx[j - i] + cur) > a[j];
                a.mas[j] = static_cast<uint32_t>(tmp);
            }
        }
        if (new_sign) {
            *this = -(*this);
        }
        return shrink_to_fit();
    } else {
        if (b.mas.size() == 1) {
            *this = a.div(b[0]);
            if (new_sign) {
                *this = -(*this);
            }
        } else {
            *this = 0;
        }
        return *this;
    }
}

std::ostream& operator<<(std::ostream& s, const big_integer& a) {
    s << to_string(a);
    return s;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    *this = (*this - (*this / rhs) * rhs);
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    fill(rhs.mas.size());
    for (size_t i = 0; i < mas.size(); i++) {
        mas[i] &= rhs[i];
    }
    sign = !((!sign) & (!rhs.sign));
    return shrink_to_fit();
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    fill(rhs.mas.size());
    for (size_t i = 0; i < mas.size(); i++) {
        mas[i] |= rhs[i];
    }
    sign = !((!sign) | (!rhs.sign));
    return shrink_to_fit();
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    fill(rhs.mas.size());
    for (size_t i = 0; i < mas.size(); i++) {
        mas[i] ^= rhs[i];
    }
    sign = !((!sign) ^ (!rhs.sign));
    return shrink_to_fit();
}

big_integer& big_integer::operator<<=(int rhs) {
    big_integer TWO = 2;
    size_t n = rhs;
    for (size_t i = 0; i < n; i++) {
        (*this) *= TWO;
    }
    return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
    big_integer x = (sign ? *this : -(*this));
    big_integer TWO = 2;
    size_t n = rhs;
    for (size_t i = 0; i < n; i++) {
        x /= TWO;
    }
    *this = (sign ? x : -x - 1);
    return *this;
}

uint32_t big_integer::operator[](size_t pos) const {
    return static_cast<uint32_t>((pos >= mas.size()) ? (sign ? 0 : UINT32_MAX) : mas[pos]);
}

big_integer operator/(big_integer a, big_integer const& b) { return (a /= b); }

big_integer operator%(big_integer a, big_integer const& b) { return (a %= b); }

big_integer operator&(big_integer a, big_integer const& b) { return (a &= b); }

big_integer operator|(big_integer a, big_integer const& b) { return (a |= b); }

big_integer operator^(big_integer a, big_integer const& b) { return (a ^= b); }

big_integer operator<<(big_integer a, int b) { return (a <<= b); }

big_integer operator>>(big_integer a, int b) { return (a >>= b); }