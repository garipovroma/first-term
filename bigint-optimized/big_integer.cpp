#include "big_integer.h"

static uint32_t MAX_DIGIT = UINT32_MAX;

big_integer::big_integer() : mas(), sign(true) {}

big_integer::~big_integer() = default;

big_integer::big_integer(int a) {
    sign = true;
    if (a == 0) {
        return;
    }
    sign = (a > 0);
    int64_t x = a;
    if (x < 0) {
        mas.push_back(static_cast<uint32_t>(x));
    } else {
        mas.push_back(x);
    }
    shrink_to_fit();
}

uint32_t big_integer::get_end_of_mas() const {
    return (sign ? 0 : MAX_DIGIT);
}

big_integer& big_integer::shrink_to_fit() {
    uint32_t to_delete = get_end_of_mas();
    size_t nw_size = mas.size();
    while (nw_size > 0 && mas[nw_size - 1] == to_delete) {
        nw_size--;
    }
    mas.resize(nw_size);
    return *this;
}

void big_integer::fill(size_t size) {
    if (size < mas.size()) return;
    size_t new_elements_count = size - mas.size();
    uint32_t to_fill = get_end_of_mas();
    size_t cur_size = mas.size();
    mas.resize(cur_size + new_elements_count, to_fill);
}

big_integer::big_integer(const big_integer& other) : mas(other.mas), sign(other.sign) {}

big_integer& big_integer::operator=(const big_integer& other) {
    big_integer copy = other;
    mas = copy.mas;
    sign = copy.sign;
    return *this;
}

big_integer& big_integer::negate() {
    big_integer x = *this;
    x = ~x + 1;
    mas = x.mas;
    sign = x.sign;
    return *this;
}

big_integer::big_integer(std::string const& str) : big_integer() {
    if (str.empty()) {
        throw std::invalid_argument("empty string found");
    }
    for (size_t i = 0; i < str.size(); i++) {
        if (i != 0 && !isdigit(str[i])) {
            throw std::invalid_argument("string constains non-digit chars");
        }
        if (i == 0 && !isdigit(str[i]) && str[i] != '+' && str[i] != '-') {
            throw std::invalid_argument("string contains non-digit chars");
        }
    }
    int loop_beg = (str[0] == '-'  || str[0] == '+' ? 1 : 0);
    uint32_t ten = 10;
    big_integer cur;
    for (size_t i = loop_beg; i != str.size(); i++) {
        mul(ten);
        cur = static_cast<int>(str[i] - '0');
        *this += cur;
    }
    if (loop_beg == 1 && str[0] == '-') {
        negate();
    }
}

std::string to_string(const big_integer& a) {
    std::string res;
    big_integer x = a.abs();
    uint32_t ten = 10;
    int digit;
    while (x.mas.size() != 0) {
        big_integer cur = x % ten;
        if (cur.mas.size() == 0) {
            digit = 0;
        } else {
            digit = cur.mas[0];
        }
        res.push_back('0' + digit);
        x = x.div(ten);
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
    big_integer res = *this;
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
        res.mas[i] = ~res.mas[i];
    }
    return res.shrink_to_fit();
}

big_integer big_integer::operator-() const {
    if ((*this) == 0) {
        return *this;
    }
    big_integer res = *this;
    res.negate();
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
    if (a.mas.size() != b.mas.size()) {
        return (a.mas.size() < b.mas.size());
    }
    for (size_t i = a.mas.size(); i >= 1; i--) {
        if (a.mas[i - 1] != b.mas[i - 1])
            return (a.mas[i - 1] <= b.mas[i - 1]);
    }
    return true;
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

storage big_integer::multiply(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) {
    size_t n = a.size(), m = b.size();
    storage res;
    res.resize(n + m);
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

big_integer big_integer::abs() const {
    if (sign) {
        return *this;
    } else {
        return -(*this);
    }
}

big_integer &big_integer::operator*=(const big_integer &rhs) {
    if (rhs == 0) {
        return *this = 0;
    }
    storage new_mas;
    bool new_sign = true;
    if (sign != rhs.sign) {
        if (sign) {
            new_mas = multiply(mas.get_mas_copy(), (-rhs).mas.get_mas_copy());
        } else {
            new_mas = multiply((-(*this)).mas.get_mas_copy(), rhs.mas.get_mas_copy());
        }
        new_sign = false;
    } else {
        if (sign) {
            new_mas = multiply(mas.get_mas_copy(), rhs.mas.get_mas_copy());
        } else {
            new_mas = multiply((-(*this)).mas.get_mas_copy(), (-rhs).mas.get_mas_copy());
        }
    }
    sign = true;
    mas = new_mas;
    shrink_to_fit();
    if (!new_sign) {
        negate();
    }
    return *this;
}

big_integer operator*(big_integer a, const big_integer& b) {
    a *= b;
    return a;
}

big_integer& big_integer::div(uint32_t b) {
    if (b == 0) {
        throw std::runtime_error("divide by zero");
    }
    uint64_t rem = 0;
    storage new_mas = mas;
    for (size_t i = mas.size(); i >= 1; i--) {
        uint64_t tmp = (*this)[i - 1];
        uint32_t cur = static_cast<uint32_t>((tmp + (rem << 32u)) / b);
        new_mas[i - 1] = cur;
        rem = (tmp + (rem << 32u)) % b;
    }
    mas = new_mas;
    return shrink_to_fit();
}

big_integer& big_integer::operator/=(const big_integer& rhs) {
    big_integer a = abs();
    big_integer b = rhs.abs();
    bool new_sign = (sign != rhs.sign);
    if (b.mas.size() == 1) {
        *this = a.div(b[0]);
        if (new_sign) {
            negate();
        }
        return *this;
    }
    if (a < b) {
        return (*this = 0);
    }
    a.mas.push_back(0);
    size_t n = a.mas.size();
    size_t m = b.mas.size();
    *this = 0;
    fill(n - m + 1);
    __uint128_t y = b[m - 1], z = b[m - 2];
    size_t N = n - m - 1;
    for (size_t t = 0; t <= N; t++) {
        __uint128_t v = a[n - t - 1], w = a[n - t - 2], x = a[n - t - 3];
        __uint128_t trial = (x ^ (w << 32u) ^ (v << 64u)) / ((y << 32u) ^ z);
        uint32_t cur = std::min(static_cast<uint32_t>(trial), MAX_DIGIT);
        big_integer bx = b;
        bx.mul(cur);
        bool decrease = false;
        size_t i = n - m - t - 1;
        if (t != N) {
            for (size_t j = m + i, k = j - i; j >= i; j--, k--) {
                if (a[j] != bx[k]) {
                    decrease = a[j] < bx[k];
                    break;
                }
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
        negate();
    }
    return shrink_to_fit();
}

std::ostream& operator<<(std::ostream& s, const big_integer& a) {
    s << to_string(a);
    return s;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    *this = (*this - (*this / rhs) * rhs);
    return *this;
}

big_integer& big_integer::bit_operator(big_integer const& rhs,  const std::function<uint32_t(uint32_t, uint32_t)> &function) {
    fill(rhs.mas.size());
    for (size_t i = 0; i < rhs.mas.size(); i++) {
        mas[i] = function(rhs[i], mas[i]);
    }
    sign = !function(!sign, !rhs.sign);
    return shrink_to_fit();
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    return bit_operator(rhs, [](uint32_t x, uint32_t y) { return x & y;});
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    return bit_operator(rhs, [](uint32_t x, uint32_t y) { return x | y;});
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    return bit_operator(rhs, [](uint32_t x, uint32_t y) { return x ^ y;});
}

big_integer& big_integer::operator<<=(int rhs) {
    if (rhs < 0) {
        return ((*this) >>= -rhs);
    }
    size_t n = rhs / 32;
    size_t m = rhs % 32;
    big_integer cur = abs().mul(1u << m);
    cur.mas.reverse();
    size_t was = cur.mas.size();
    cur.mas.resize(was + n, 0);
    cur.mas.reverse();
    mas = cur.mas;
    if (!sign) {
        cur = -cur;
    }
    return (*this = cur);
}

big_integer& big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return ((*this) <<= -rhs);
    }
    size_t n = rhs / 32;
    size_t m = rhs % 32;
    big_integer cur = abs().div(1u << m);
    cur.mas.erase(0, n);
    *this = (sign ? cur : -cur - 1);
    return *this;
}

uint32_t big_integer::operator[](size_t pos) const {
    return static_cast<uint32_t>((pos >= mas.size()) ? get_end_of_mas() : mas[pos]);
}

big_integer operator/(big_integer a, big_integer const& b) { return (a /= b); }

big_integer operator%(big_integer a, big_integer const& b) { return (a %= b); }

big_integer operator&(big_integer a, big_integer const& b) { return (a &= b); }

big_integer operator|(big_integer a, big_integer const& b) { return (a |= b); }

big_integer operator^(big_integer a, big_integer const& b) { return (a ^= b); }

big_integer operator<<(big_integer a, int b) { return (a <<= b); }

big_integer operator>>(big_integer a, int b) { return (a >>= b); }