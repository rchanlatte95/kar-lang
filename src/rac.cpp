#pragma warning(push, 0)// Prevent warnings from libraries I can't fix.
#include <iostream>
#include <limits>
#include <vector>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#pragma warning(pop)

#ifndef RAC
#define RAC

#define MAY_INLINE __inline
#define INLINE __forceinline
#define NO_INLINE __declspec(noinline)

typedef const char* cstr;   typedef char* mut_cstr;
typedef const char** cstrptr;   typedef char** mut_cstrptr;

typedef const int8_t i8;    typedef const uint8_t u8;
typedef int8_t mut_i8;      typedef uint8_t mut_u8;

typedef const int16_t i16;  typedef const uint16_t u16;
typedef int16_t mut_i16;    typedef uint16_t mut_u16;

typedef const int32_t i32;  typedef const uint32_t u32;
typedef int32_t mut_i32;    typedef uint32_t mut_u32;

typedef const int64_t i64;  typedef const uint64_t u64;
typedef int64_t mut_i64;    typedef uint64_t mut_u64;

typedef const int_fast8_t Int8;    typedef const uint_fast8_t UInt8;
typedef int_fast8_t mut_Int8;    typedef uint_fast8_t mut_UInt8;

typedef const int_fast16_t Int16;    typedef const uint_fast16_t UInt16;
typedef int_fast16_t mut_Int16;    typedef uint_fast16_t mut_UInt16;

typedef const int_fast32_t Int32;    typedef const uint_fast32_t UInt32;
typedef int_fast32_t mut_Int32;    typedef uint_fast32_t mut_UInt32;

typedef const int_fast64_t Int64;    typedef const uint_fast64_t UInt64;
typedef int_fast64_t mut_Int64;    typedef uint_fast64_t mut_UInt64;

typedef const float f32;    typedef const float& f32_ref;   typedef float mut_f32;
typedef const double f64;   typedef const double& f64_ref;  typedef double mut_f64;

typedef const uint8_t* utf8ptr; typedef const wchar_t* utf16ptr;
typedef uint8_t* mut_utf8ptr;   typedef wchar_t* mut_utf16ptr;

typedef void* mut_ptr;      typedef const void* ptr;

typedef const int8_t* i8ptr;    typedef int8_t* mut_i8ptr;
typedef const uint8_t* u8ptr;   typedef uint8_t* mut_u8ptr;

typedef const int16_t* i16ptr;  typedef int16_t* mut_i16ptr;
typedef const uint16_t* u16ptr; typedef uint16_t* mut_u16ptr;

typedef const int32_t* i32ptr;  typedef int32_t* mut_i32ptr;
typedef const uint32_t* u32ptr; typedef uint32_t* mut_u32ptr;

typedef const int64_t* i64ptr;  typedef int64_t* mut_i64ptr;
typedef const uint64_t* u64ptr; typedef uint64_t* mut_u64ptr;

typedef const float* f32ptr;  typedef int32_t* mut_f32ptr;
typedef const double* f64ptr; typedef uint32_t* mut_f64ptr;

namespace hopman_fast
{
    struct itostr_helper
    {
        static unsigned out[10000];

        itostr_helper()
        {
            for (int i = 0; i < 10000; i++)
            {
                unsigned v = i;
                char* o = (char*)(out + i);
                o[3] = v % 10 + '0';
                o[2] = (v % 100) / 10 + '0';
                o[1] = static_cast<char>((v % 1000) / 100) + '0';
                o[0] = static_cast<char>((v % 10000) / 1000);
                if (o[0])        o[0] |= 0x30;
                else if (o[1] != '0') o[0] |= 0x20;
                else if (o[2] != '0') o[0] |= 0x10;
                else                  o[0] |= 0x00;
            }
        }
    };

    unsigned itostr_helper::out[10000];

    itostr_helper hlp_init;

    template <typename T> void itostr(T o, std::string& out)
    {
        typedef itostr_helper hlp;

        unsigned blocks[3]{}, * b = blocks + 2;
        blocks[0] = o < 0 ? ~o + 1 : o;
        blocks[2] = blocks[0] % 10000; blocks[0] /= 10000;
        blocks[2] = hlp::out[blocks[2]];

        if (blocks[0])
        {
            blocks[1] = blocks[0] % 10000; blocks[0] /= 10000;
            blocks[1] = hlp::out[blocks[1]];
            blocks[2] |= 0x30303030;
            b--;
        }

        if (blocks[0])
        {
            blocks[0] = hlp::out[blocks[0] % 10000];
            blocks[1] |= 0x30303030;
            b--;
        }

        char* f = ((char*)b);
        f += 3 - (*f >> 4);

        char* str = (char*)blocks;
        if (o < 0) *--f = '-';
        out.assign(f, (str + 12) - f);
    }

    template <typename T> std::string itostr(T o)
    {
        std::string result;
        itostr(o, result);
        return result;
    }
}

#define RAC_RMV_UNREF_LOCAL_FUNC 4505
#pragma warning(push)
#pragma warning(disable: RAC_RMV_UNREF_LOCAL_FUNC)

namespace rac
{
    const u64 BYTES_IN_KB = 1024;
    const u64 BYTES_IN_WIN_PAGE = BYTES_IN_KB * 4;
    const u64 BYTES_IN_MB = BYTES_IN_KB * 1024;

    const u64 X86_STACK_BYTE_SIZE = BYTES_IN_MB;
    const u64 X64_STACK_BYTE_SIZE = BYTES_IN_MB * 4;

    namespace rnd
    {
        namespace xor64
        {
            mut_u64 u64_state = 88172645463325252LL;
            static u64 GetU64()
            {
                mut_u64 x = u64_state;
                x ^= x << 13;
                x ^= x >> 7;
                x ^= x << 17;
                return u64_state = x;
            }

            mut_i64 i64_state = 1680588011350901LL;
            static i64 GetI64()
            {
                mut_i64 x = (mut_i64)i64_state;
                x ^= x << 9;
                x ^= x >> 21;
                x ^= x << 11;
                return i64_state = x;
            }
        }
    }

    namespace logic
    {
        class Bool;
        typedef const Bool* Bool_ptr;   typedef Bool* mut_Bool_ptr;
        typedef const Bool& Bool_ref;   typedef Bool& mut_Bool_ref;

        class Bool
        {
        private:
            mut_Int8 value = 0;

        public:
            Bool() { value = 0; }
            Bool(bool b) { value = b ? 1 : 0; }

            Bool(i8 i) { value = i != 0 ? 1 : 0; }
            Bool(u8 u) { value = u != 0 ? 1 : 0; }

            Bool(i16 i) { value = i != 0 ? 1 : 0; }
            Bool(u16 u) { value = u != 0 ? 1 : 0; }

            Bool(i32 i) { value = i != 0 ? 1 : 0; }
            Bool(u32 u) { value = u != 0 ? 1 : 0; }

            Bool(i64 i) { value = i != 0 ? 1 : 0; }
            Bool(u64 u) { value = u != 0 ? 1 : 0; }

            operator bool() const { return value != 0; }

            cstr Cstr() const { return value ? "true" : "false"; }
            Int8 ToInt() const { return value ? 1 : 0; }

            INLINE Bool_ref operator=(bool rhs)
            {
                value = rhs ? 1 : 0;
                return *this;
            }

            INLINE Bool_ref operator=(ptr rhs)
            {
                value = rhs != nullptr ? 1 : 0;
                return *this;
            }
        };

        enum Comparison : Int8
        {
            Null = SCHAR_MIN,
            LessThan = -1,
            Equal = 0,
            GreaterThan = 1
        };

        class comp
        {
        private:
            Comparison comparison_value = Null;

        public:
            comp() {}

            comp(i8 val)
            {
                if (val == 0) comparison_value = Equal;
                else if (val < 0) comparison_value = LessThan;
                else comparison_value = GreaterThan;
            }
            comp(i16 val)
            {
                if (val == 0) comparison_value = Equal;
                else if (val < 0) comparison_value = LessThan;
                else comparison_value = GreaterThan;
            }
            comp(i32 val)
            {
                if (val == 0) comparison_value = Equal;
                else if (val < 0) comparison_value = LessThan;
                else comparison_value = GreaterThan;
            }
            comp(i64 val)
            {
                if (val == 0) comparison_value = Equal;
                else if (val < 0) comparison_value = LessThan;
                else comparison_value = GreaterThan;
            }

            const char* Cstr() const
            {
                switch (comparison_value)
                {
                case LessThan: return "Less Than";
                case Equal: return "Equal";
                case GreaterThan: return "Greater Than";
                default: return "Null";
                }
            }
        };

        class v2bool;
        typedef const v2bool* v2bool_ptr;   typedef const v2bool& v2bool_ref;
        typedef v2bool* mut_v2bool_ptr; typedef v2bool& mut_v2bool_ref;
        class v2bool
        {
            Bool x = false;
            Bool y = false;
        };

        class v3bool;
        typedef const v3bool* v3bool_ptr;   typedef const v3bool& v3bool_ref;
        typedef v3bool* mut_v3bool_ptr; typedef v3bool& mut_v3bool_ref;
        class v3bool
        {
            Bool x = false;
            Bool y = false;
            Bool z = false;
        };
    }

    namespace string
    {
        #define RAC_DIGIT_TO_CHAR(x) (x + 48)
        const i32 SSTR_BASE_BYTE_SIZE = UINT8_MAX;
        const i32 SSTR_CAPACITY = SSTR_BASE_BYTE_SIZE - sizeof(i32);
        const i32 SSTR_MAX_LEN = SSTR_CAPACITY - 1;

        // https://en.wikipedia.org/wiki/Whitespace_character
        const u8 CHAR_TAB = 0x09;
        const u8 LINE_FEED = 0x0A;
        const u8 LINE_TAB = 0x0B;
        const u8 FORM_FEED = 0x0C;
        const u8 CARRIAGE_RETURN = 0x0D;
        const u8 SPACE = 0x20;
        const u8 NEXT_LINE = 0x85;
        const u8 NO_BREAK_SPACE = 0xA0;

        static constexpr i32 clamp_len(i32 x)
        {
            if (x > SSTR_MAX_LEN) return SSTR_MAX_LEN;
            if (x < 0) return 0;
            return x;
        }
        static constexpr bool whitespace(u8 c)
        {
            return  c == SPACE ||
                    c == LINE_FEED || c == CARRIAGE_RETURN ||
                    c == CHAR_TAB || c == LINE_TAB ||
                    c == NEXT_LINE || c == NO_BREAK_SPACE;
        }
        static constexpr bool whitespace(u8ptr c_ptr) { return  whitespace(*c_ptr); }

        class str;
        typedef const str* str_ptr;   typedef str* mut_str_ptr;
        typedef const str& str_ref;   typedef str& mut_str_ref;

        class str
        {
        private:
            mut_i32 len = 0;
            mut_u8 chars[SSTR_CAPACITY];

        public:
            str()
            {
                len = 0;
                chars[0] = 0;
                chars[SSTR_MAX_LEN] = 0;
            }

            str(cstr _str)
            {
                len = (i32)strnlen_s(_str, SSTR_MAX_LEN);
                chars[0] = 0;
                chars[SSTR_MAX_LEN] = 0;
                memcpy_s(chars, SSTR_MAX_LEN, _str, len);

#pragma warning( disable : 6386) // Erroneous Buffer overrun warning
                chars[len] = 0;
#pragma warning( default : 6386)
            }

            str(cstr _str, i32 startIndex, i32 char_ct)
            {
                i32 start = clamp_len(startIndex);
                len = (i32)strnlen_s(&_str[start], clamp_len(char_ct));
                chars[0] = 0;
                chars[SSTR_MAX_LEN] = 0;
                memcpy_s(chars, SSTR_MAX_LEN, _str, len);
            }

            str(str_ptr _str)
            {
                len = _str->len;
                memcpy_s(chars, len, _str->chars, len);
                chars[len] = 0;
                chars[SSTR_MAX_LEN] = 0;
            }

            str(str_ref _str)
            {
                len = _str.len;
                memcpy_s(chars, len, _str.chars, len);
                chars[len] = 0;
                chars[SSTR_MAX_LEN] = 0;
            }

            str(str_ptr _str, i32 startIndex, i32 char_ct)
            {
                i32 start = clamp_len(startIndex);

                len = (i32)clamp_len(char_ct);
                memcpy_s(chars, len, &_str->chars[start], len);
                chars[len] = 0;
                chars[SSTR_MAX_LEN] = 0;
            }

            str(str_ref _str, i32 startIndex, i32 char_ct)
            {
                i32 start = clamp_len(startIndex);

                len = (i32)clamp_len(char_ct);
                memcpy_s(chars, len, &_str.chars[start], len);
                chars[len] = 0;
                chars[SSTR_MAX_LEN] = 0;
            }

            operator cstr() const { return (cstr)chars; }

            INLINE i32 MaxLength() const { return SSTR_MAX_LEN; }
            INLINE i32 Length() const { return len; }
            INLINE utf8ptr Str() const { return &chars[0]; }
            INLINE bool Empty() const { return len == 0; }
            INLINE void Clear() { len = 0; chars[0] = 0; }
            INLINE void Fill(u8 c, i32 ct = SSTR_MAX_LEN)
            {
                len = ct;
                memset(chars, c, len);
            }
            INLINE u8& operator[](i32 index) { return chars[index]; }
            INLINE str_ref operator=(cstr rhs)
            {
                len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);
                chars[0] = 0;
                chars[SSTR_MAX_LEN] = 0;
                memcpy_s(chars, SSTR_MAX_LEN, rhs, len);

#pragma warning( disable : 6386) // Erroneous Buffer overrun warning
                chars[len] = 0;
#pragma warning( default : 6386)

                return *this;
            }
            INLINE str_ref operator=(str_ref rhs)
            {
                if (*this == rhs) { return *this; }

                len = rhs.len;
                memcpy_s(chars, len, rhs.chars, len);
                chars[len] = 0;
                chars[SSTR_MAX_LEN] = 0;

                return *this;
            }

            INLINE str_ref operator+=(cstr rhs)
            {
                i32 rhs_len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);
                i32 new_len = len + rhs_len;
                if (len == SSTR_MAX_LEN || new_len >= SSTR_MAX_LEN)
                {
                    return *this;
                }

                memcpy_s(chars + len, new_len, rhs, rhs_len);
                len = new_len;
                chars[len] = 0;

                return *this;
            }
            INLINE str_ref operator+=(str_ref rhs)
            {
                i32 new_len = len + rhs.len;
                if (len == SSTR_MAX_LEN || new_len >= SSTR_MAX_LEN)
                {
                    return *this;
                }

                memcpy_s(chars+len, new_len, rhs.chars, rhs.len);
                len = new_len;
                chars[len] = 0;

                return *this;
            }
            INLINE str_ref operator+=(u8 c)
            {
                if (len == SSTR_MAX_LEN) return *this;
                chars[len++] = c;
                chars[len] = 0;
                return *this;
            }

            INLINE bool operator>(str_ref rhs)
            {
                return memcmp(ptr(&len), ptr(&rhs.len), len) > 0;
            }
            INLINE bool operator>=(str_ref rhs)
            {
                return memcmp(ptr(&len), ptr(&rhs.len), len) >= 0;
            }
            INLINE bool operator<(str_ref rhs)
            {
                return memcmp(ptr(&len), ptr(&rhs.len), len) < 0;
            }
            INLINE bool operator<=(str_ref rhs)
            {
                return memcmp(ptr(&len), ptr(&rhs.len), len) <= 0;
            }
            INLINE bool operator==(str_ref rhs)
            {
                return memcmp(&len, rhs.ToRef(), len) == 0;
            }
            INLINE bool operator!=(str_ref rhs)
            {
                return memcmp(&len, rhs.ToRef(), len) != 0;
            }

            INLINE bool operator==(cstr rhs)
            {
                i32 rhs_len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);

#pragma warning( disable : 6385) // Erroneous invalid data read warning
                return rhs_len == len && memcmp(chars, rhs, rhs_len) == 0;
#pragma warning( default : 6385)
            }
            INLINE bool operator!=(cstr rhs)
            {
                i32 rhs_len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);

#pragma warning( disable : 6385) // Erroneous invalid data read warning
                return rhs_len == len && memcmp(chars, rhs, rhs_len) != 0;
#pragma warning( default : 6385)
            }

            INLINE logic::comp Compare(str_ref arg)
            {
                return logic::comp(memcmp(ptr(&len), ptr(&arg.len), len));
            }
            INLINE logic::comp Compare(cstr c_str)
            {
                return logic::comp(memcmp(ptr(&chars[0]), ptr(c_str), len));
            }

            INLINE cstr ToCstr() const { return (cstr)(&chars[0]); }
            INLINE ptr ToPtr() const { return (ptr)(&len); }
            INLINE u8ptr PtrToChars() const { return chars; }
            INLINE str_ref ToRef() const { return *this; }

            INLINE u8& First() const { return chars[0]; }
            INLINE u8& Last() const { return chars[len - 1]; }
            INLINE str_ref TrimStart()
            {
                if (len < 1) return *this;

                mut_u8ptr start = chars - 1;
                while (whitespace(++start));

                len -= (i32)(start - chars);
                memmove_s(chars, SSTR_MAX_LEN, start, len);
                chars[len] = 0;

                return *this;
            }
            INLINE str_ref TrimEnd()
            {
                if (len < 1) return *this;

                const u8ptr fxd_end = chars + len - 1;
                u8ptr end = fxd_end;
                while (whitespace(--end));

                len -= (i32)(fxd_end - end);
                memmove_s(chars, SSTR_MAX_LEN, chars, len);
                chars[len] = 0;

                return *this;
            }
            INLINE str_ref Trim()
            {
                if (len < 1) return *this;

                u8ptr start = chars - 1;
                u8ptr end = chars + len - 1;

                while (whitespace(++start));
                while (whitespace(--end));

                len = (i32)(end - start) + 1;
                memmove_s(chars, SSTR_MAX_LEN, start, len);
                chars[len] = 0;

                return *this;
            }
            INLINE i32 IndexOf(u8 target) const
            {
                for (int i = 0; i < len; ++i)
                {
                    if (chars[i] == target) return i;
                }

                return -1;
            }
            INLINE i32 IndexOf(u8 target, i32 startIndex) const
            {
                if (startIndex >= len) return -1;

                for (int i = startIndex; i < len; ++i)
                {
                    if (chars[i] == target) return i;
                }

                return -1;
            }
            INLINE bool Contains(u8 target) const
            {
                return IndexOf(target) > 0;
            }
            INLINE bool Contains(u8 target, i32 startIndex) const
            {
                return IndexOf(target, startIndex) > 0;
            }
            INLINE bool NullOrEmpty() const
            {
                return *this == nullptr || len < 1;
            }
            INLINE bool EmptyOrWhitespace() const
            {
                if (len < 1) return true;

                mut_i32 _len = len;
                while(--_len)
                {
                    if (!whitespace(chars[_len])) { return false; }
                }
                return true;
            }
        };

        INLINE static str operator +(str_ref lhs, str_ref rhs)
        {
            str res(lhs);
            res += rhs;
            return res;
        }
        INLINE static str operator +(cstr lhs, str_ref rhs)
        {
            str res(lhs);
            res += rhs;
            return res;
        }
    }

    namespace mth
    {
        using namespace rac::string;

        const f32 PI = 3.14159265358979323846f;
        const f32 SIXTH_PI = PI / 6.0f;
        const f32 FOURTH_PI = PI / 4.0f;
        const f32 THIRD_PI = PI / 3.0f;
        const f32 HALF_PI = PI / 2.0f;
        const f32 TAU = 2.0f * PI;

        const f32 INV_PI = 1.0f / PI;
        const f32 DEG2RAD = PI / 180.0f;
        const f32 RAD2DEG = 180.0f / PI;

        f32 F32_EPSILON = 0.0001f;
        f32 SIGNED_F32_EPSILON = -F32_EPSILON;
        f32 F32_ONE_EPSILON = 1.0f - F32_EPSILON;
        f32 SIGNED_F32_ONE_EPSILON = -F32_ONE_EPSILON;

        // Single precision floating point number has a range of eight
        // digits. Add a decimal point, a negative sign, and a space
        // for the null terminator and you have the length of a float
        // string.
        #define RAC_F32_UNSIGNED_RANGE 8
        #define RAC_F32_SIGNED_RANGE (RAC_F32_UNSIGNED_RANGE + 1)
        i32 F32_STR_MAX = RAC_F32_SIGNED_RANGE + 3;
        i32 F32_STR_LEN = F32_STR_MAX - 1;

        #define RAC_F32_APPROX(a, b) (fabsf(a - b) <= F32_EPSILON)
        #define RAC_F32_APPROX_ZERO(a) (fabsf(a) <= F32_EPSILON)
        #define RAC_F32_APPROX_ONE(a) (RAC_F32_APPROX(a, F32_ONE_EPSILON))

        #define RAC_F32_APPROX_MORE(a, b) (fabsf(a - b) > F32_EPSILON)
        #define RAC_F32_APPROX_LESS(a, b) (fabsf(a - b) < SIGNED_F32_EPSILON)
        #define RAC_F32_APPROX_MORE_OR_EQUAL(a, b) (fabsf(a - b) >= F32_EPSILON)
        #define RAC_F32_APPROX_LESS_OR_EQUAL(a, b) (fabsf(a - b) <= SIGNED_F32_EPSILON)
        #define RAC_F32_APPROX_POS(a) (a >= F32_EPSILON)
        #define RAC_F32_APPROX_NEG(a) (a <= F32_EPSILON)

        static f32 Clamp(f32 x, f32 min, f32 max)
        {
            if (fabsf(x - max) > F32_EPSILON) return max;
            if (fabsf(x - min) < F32_EPSILON) return min;
            return x;
        }
        static f32 Clamp01(f32 x)
        {
            if (fabsf(x - F32_ONE_EPSILON) > F32_EPSILON) return 1.0f;
            if (fabsf(x) < F32_EPSILON) return 0.0f;
            return x;
        }

        class v2;
        typedef const v2* v2_ptr;   typedef const v2& v2_ref;
        typedef v2* mut_v2_ptr; typedef v2& mut_v2_ref;

        i32 COMMA_SPACE_LEN = 2;
        i32 PARENTHESES_LEN = 2;
        i32 V2_STRING_MAX = (F32_STR_LEN * 2) + COMMA_SPACE_LEN + PARENTHESES_LEN + 1;
        i32 V2_STRING_LEN = V2_STRING_MAX - 1;
        class v2
        {
        public:
            mut_f32 x = 0.0f;
            mut_f32 y = 0.0f;

            v2(i8 _x, i8 _y) { x = (mut_f32)_x; y = (mut_f32)_y; }
            v2(i16 _x, i16 _y) { x = (mut_f32)_x; y = (mut_f32)_y; }
            v2(i32 _x, i32 _y) { x = (mut_f32)_x; y = (mut_f32)_y; }
            v2(i8 a) { x = (mut_f32)a; y = (mut_f32)a; }
            v2(i16 a) { x = (mut_f32)a; y = (mut_f32)a; }
            v2(i32 a) { x = (mut_f32)a; y = (mut_f32)a; }

            v2(f32 _x, f32 _y) { x = _x; y = _y; }
            v2(f32 a) { x = a; y = a; }
            v2() { }

            INLINE v2_ref operator=(v2_ref rhs)
            {
                x = rhs.x;
                y = rhs.y;
            }
            INLINE v2 operator -() const { return v2(-x, -y); }
            INLINE v2 operator *(f32 v) { return v2(x * v, y * v); }
            INLINE v2 operator /(f32 v)
            {
                f32 inv_v = 1.0f / v;
                return v2(x * inv_v, y * inv_v);
            }
            INLINE v2_ref operator +=(v2_ref rhs)
            {
                x += rhs.x;
                y += rhs.y;
            }
            INLINE v2_ref operator -=(v2_ref rhs)
            {
                x -= rhs.x;
                y -= rhs.y;
            }
            INLINE v2_ref operator *=(v2_ref rhs)
            {
                x *= rhs.x;
                y *= rhs.y;
            }
            INLINE v2_ref operator *=(f32 a)
            {
                x *= a;
                y *= a;
            }
            INLINE v2_ref operator /=(v2_ref rhs)
            {
                x /= rhs.x;
                y /= rhs.y;
            }
            INLINE v2_ref operator /=(f32 a)
            {
                f32 inv = 1.0f / a;
                x *= inv;
                y *= inv;
            }

            INLINE ptr Ptr() const { return (ptr)(&x); }
            INLINE v2_ref Ref() const { return *this; }

            INLINE str_ref ToStr(mut_str_ref str) const
            {
                char buff[V2_STRING_MAX] = { 0 };
                sprintf_s(buff, V2_STRING_LEN, "(%0.3f, %0.3f)", x, y);
                return str = buff;
            }

            INLINE v2 Max(v2 v)
            {
                return v2(fmaxf(x, v.x), fmaxf(y, v.y));
            }
            INLINE v2 Min(v2 v)
            {
                return v2(fminf(x, v.x), fminf(y, v.y));
            }
            INLINE v2 Clamp(f32 min, f32 max)
            {
                return v2(  x > max ? max : (x < min ? min : x),
                            y > max ? max : (y < min ? min : y));
            }
            INLINE v2 Clamp(f32 x_min, f32 x_max, f32 y_min, f32 y_max)
            {
                return v2(  x > x_max ? x_max : (x < x_min ? x_min : x),
                            y > y_max ? y_max : (y < y_min ? y_min : y));
            }
            INLINE v2 Clamp(v2 min, v2 max)
            {
                return v2(  x > max.x ? max.x : (x < min.x ? min.x : x),
                            y > max.y ? max.y : (y < min.y ? min.y : y));
            }

            INLINE v2 ClampMag(f32 min, f32 max)
            {
                v2 res = *this;
                mut_f32 len = res.x * res.x + res.y + res.y;
                if (len > F32_EPSILON)
                {
                    len = sqrtf(len);
                    mut_f32 scale = 1.0f;
                    if (len < min) { scale *= min / len; }
                    else if (len > max) { scale *= max / len; }

                    res.x *= scale;
                    res.y *= scale;
                }

                return res;
            }

            INLINE v2 Inv() const { return v2(1.0f / x, 1.0f / y); }
            INLINE f32 Mag() const { return sqrtf(x * x + y * y); }
            INLINE f32 InvMag() const { return 1.0f / sqrtf(x * x + y * y); }
            INLINE f32 SqrMag() const { return x * x + y * y; }
            INLINE f32 Dot() const { return x * x + y * y; }
            INLINE f32 Dot(v2_ref v2) const { return x * v2.x + y * v2.y; }
            INLINE f32 Dist(v2_ref from) const
            {
                return sqrtf((x - from.x)* (x - from.x) + (y * from.y)*(y * from.y));
            }
            INLINE f32 SqrDist(v2_ref from) const
            {
                return (x - from.x) * (x - from.x) + (y * from.y) * (y * from.y);
            }
            INLINE f32 Angle(v2_ref from) const
            {
                f32 dot = x * from.x + y * from.y;
                f32 det = x * from.y - y * from.x;
                return atan2f(det, dot);
            }
            INLINE v2 Norm() const
            {
                f32 inv_mag = InvMag();
                if (inv_mag > F32_EPSILON)
                {
                    return v2(x * inv_mag, y * inv_mag);
                }
                else return *this;
            }
            INLINE v2 Rotate(f32 radians) const
            {
                f32 cosRes = cosf(radians);
                f32 sinRes = sinf(radians);
                return v2(x*cosRes - y * sinRes, x*sinRes + y * cosRes);
            }
            INLINE v2 RotateDeg(f32 degrees) const
            {
                return Rotate(degrees * RAD2DEG);
            }
            INLINE v2 Perpendicular() const { return v2(y, -x); }
        };

        INLINE static bool operator >(v2_ref lhs, v2_ref rhs)
        {
            return  lhs.x > rhs.x && lhs.y > rhs.y;
        }
        INLINE static bool operator <(v2_ref lhs, v2_ref rhs)
        {
            return lhs.x < rhs.x && lhs.y < rhs.y;
        }
        INLINE static bool operator >=(v2_ref lhs, v2_ref rhs)
        {
            return lhs.x >= rhs.x && lhs.y >= rhs.y;
        }
        INLINE static bool operator <=(v2_ref lhs, v2_ref rhs)
        {
            return lhs.x <= rhs.x && lhs.y <= rhs.y;
        }
        INLINE static bool operator ==(v2_ref lhs, v2_ref rhs)
        {
            return  lhs.x == rhs.x && lhs.y == rhs.y;
        }
        INLINE static bool operator !=(v2_ref lhs, v2_ref rhs)
        {
            return !(lhs == rhs);
        }

        INLINE static v2 operator +(v2_ref lhs, v2_ref rhs)
        {
            return v2(lhs.x + rhs.x, lhs.y + rhs.y);
        }
        INLINE static v2 operator -(v2_ref lhs, v2_ref rhs)
        {
            return v2(lhs.x - rhs.x, lhs.y - rhs.y);
        }
        INLINE static v2 operator *(v2_ref lhs, v2_ref rhs)
        {
            return v2(lhs.x * rhs.x, lhs.y * rhs.y);
        }
        INLINE static v2 operator /(v2_ref lhs, v2_ref rhs)
        {
            return v2(lhs.x / rhs.x, lhs.y / rhs.y);
        }

        const v2 V2_ZERO = v2(0.0f, 0.0f);
        const v2 V2_ONE = v2(1.0f, 1.0f);
        const v2 V2_SIGNED_ONE = -V2_ONE;

        const v2 V2_RIGHT = v2(1.0f, 0.0f);
        const v2 V2_LEFT = -V2_RIGHT;
        const v2 V2_UP = v2(0.0f, 1.0f);
        const v2 V2_DOWN = -V2_UP;

        // Check if two vectors are approximately equal
        static MAY_INLINE bool Approx(v2_ref a, v2_ref b)
        {
            return RAC_F32_APPROX(a.x, b.x) && RAC_F32_APPROX(a.y, b.y);
        }
        static MAY_INLINE bool NotApprox(v2_ref a, v2_ref b)
        {
            return !Approx(a, b);
        }

        static MAY_INLINE v2 MoveToward(v2_ref from, v2_ref to, f32 maxDelta)
        {
            v2 dv = to - from;
            f32 dot = dv.Dot();

            if (dot <= F32_EPSILON || RAC_F32_APPROX_POS(maxDelta) &&
                (dot <= maxDelta * maxDelta))
            {
                return to;
            }

            f32 inv_scaled_dist = 1.0f / (sqrtf(dot) * maxDelta);
            return from + (dv * inv_scaled_dist);
        }

        static MAY_INLINE v2 Refract(v2_ref incoming, v2_ref normal, f32 refractRatio)
        {
            v2 res = V2_ZERO;
            f32 dot = incoming.Dot(normal);
            mut_f32 d = 1.0f - refractRatio * refractRatio * (1.0f - dot*dot);
            if (d >= F32_EPSILON)
            {
                d = sqrtf(d);
                res.x = refractRatio * incoming.x - (refractRatio * dot + d) * normal.x;
                res.y = refractRatio * incoming.y - (refractRatio * dot + d) * normal.y;
            }

            return res;
        }

        class v3;
        typedef const v3* v3_ptr;   typedef const v3& v3_ref;
        typedef v3* mut_v3_ptr; typedef v3& mut_v3_ref;

        i32 V3_STRING_MAX = (F32_STR_LEN * 3) + (COMMA_SPACE_LEN * 2) + PARENTHESES_LEN + 1;
        i32 V3_STRING_LEN = V3_STRING_MAX - 1;
        class v3
        {
        public:
            mut_f32 x = 0.0f;
            mut_f32 y = 0.0f;
            mut_f32 z = 0.0f;

            v3() { }

            v3(i8 _x, i8 _y, i8 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }
            v3(i16 _x, i16 _y, i16 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }
            v3(i32 _x, i32 _y, i32 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }
            v3(u8 _x, u8 _y, u8 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }
            v3(u16 _x, u16 _y, u16 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }
            v3(u32 _x, u32 _y, u32 _z)
            {
                x = (mut_f32)_x;
                y = (mut_f32)_y;
                z = (mut_f32)_z;
            }

            v3(f32 _a)
            {
                x = _a;
                y = _a;
                z = _a;
            }
            v3(f32 _x, f32 _y)
            {
                x = _x;
                y = _y;
                z = 0.0f;
            }
            v3(f32 _x, f32 _y, f32 _z)
            {
                x = _x;
                y = _y;
                z = _z;
            }
            v3(v2 v, f32 _z)
            {
                x = v.x;
                y = v.y;
                z = _z;
            }
            v3(v2 v)
            {
                x = v.x;
                y = v.y;
                z = 0.0f;
            }

            INLINE v3_ref operator=(v3_ref rhs)
            {
                x = rhs.x;
                y = rhs.y;
                z = rhs.z;
            }
            INLINE v3 operator -() const { return v3(-x, -y, -z); }
            INLINE v3 operator *(f32 v) { return v3(x * v, y * v, z * v); }
            INLINE v3 operator /(f32 v)
            {
                f32 inv = 1.0f / v;
                return v3(x * inv, y * inv, z * inv);
            }
            INLINE v3_ref operator +=(v3_ref rhs)
            {
                x += rhs.x;
                y += rhs.y;
                z += rhs.z;
            }
            INLINE v3_ref operator -=(v3_ref rhs)
            {
                x -= rhs.x;
                y -= rhs.y;
                z -= rhs.z;
            }
            INLINE v3_ref operator *=(v3_ref rhs)
            {
                x *= rhs.x;
                y *= rhs.y;
                z *= rhs.z;
            }
            INLINE v3_ref operator *=(f32 a)
            {
                x *= a;
                y *= a;
                z *= a;
            }
            INLINE v3_ref operator /=(v3_ref rhs)
            {
                x /= rhs.x;
                y /= rhs.y;
                z /= rhs.z;
            }
            INLINE v3_ref operator /=(f32 a)
            {
                f32 inv = 1.0f / a;
                x *= inv;
                y *= inv;
                z *= inv;
            }

            INLINE ptr Ptr() const { return (ptr)(&x); }
            INLINE v3_ref Ref() const { return *this; }

            INLINE str_ref ToStr(mut_str_ref str) const
            {
                char buff[V3_STRING_MAX] = { 0 };
                sprintf_s(buff, V3_STRING_LEN, "(%0.3f, %0.3f, %0.3f)", x, y, z);
                return str = buff;
            }

            INLINE v3 Max(v3 v)
            {
                return v3(fmaxf(x, v.x), fmaxf(y, v.y), fmaxf(z, v.z));
            }
            INLINE v3 Min(v3 v)
            {
                return v3(fminf(x, v.x), fminf(y, v.y), fminf(z, v.z));
            }
            INLINE v3 Clamp(f32 min, f32 max)
            {
                return v3(  x > max ? max : (x < min ? min : x),
                            y > max ? max : (y < min ? min : y),
                            z > max ? max : (z < min ? min : z));
            }
            INLINE v3 Clamp(f32 x_min, f32 x_max, f32 y_min, f32 y_max, f32 z_min, f32 z_max)
            {
                return v3(  x > x_max ? x_max : (x < x_min ? x_min : x),
                            y > y_max ? y_max : (y < y_min ? y_min : y),
                            z > z_max ? z_max : (z < z_min ? z_min : z));
            }
            INLINE v3 Clamp(v3 min, v3 max)
            {
                return v3(  x > max.x ? max.x : (x < min.x ? min.x : x),
                            y > max.y ? max.y : (y < min.y ? min.y : y),
                            z > max.z ? max.z : (z < min.z ? min.z : z));
            }
            INLINE v3 Inv() const
            {
                return v3(1.0f / x, 1.0f / y, 1.0f / z);
            }
            INLINE f32 Mag() const
            {
                return sqrtf(x * x + y * y + z * z);
            }
            INLINE f32 InvMag() const
            {
                return 1.0f / sqrtf(x * x + y * y + z * z);
            }
            INLINE f32 SqrMag() const
            {
                return x * x + y * y + z * z;
            }
            INLINE f32 Dot(v3_ref v) const
            {
                return x * v.x + y * v.y + z * v.z;
            }
            INLINE f32 Dist(v3_ref from) const
            {
                return sqrtf((x - from.x) * (x - from.x) +
                            (y * from.y) * (y * from.y) +
                            (z * from.z) * (z * from.z));
            }
            INLINE f32 SqrDist(v3_ref from) const
            {
                return  (x - from.x) * (x - from.x) +
                        (y * from.y) * (y * from.y) +
                        (z * from.z) * (z * from.z);
            }
            INLINE v3 Norm() const
            {
                f32 inv_mag = InvMag();
                if (inv_mag > F32_EPSILON)
                {
                    return v3(x * inv_mag, y * inv_mag, z * inv_mag);
                }
                else return *this;
            }

            // Cross product with unit X Vector3
            // Unit X == v3(1.0f, 0.0f, 0.0f)
            INLINE v3 CrossX() { return v3(0.0f, z, -y); }

            // Cross product with unit Y Vector3
            // Unit Y == v3(0.0f, 1.0f, 0.0f)
            INLINE v3 CrossY() { return v3(-z, 0.0f, x); }

            // Cross product with unit X Vector3
            // Unit Z == v3(0.0f, 0.0f, 1.0f)
            INLINE v3 CrossZ() { return v3(y, -x, 0.0f); }
        };

        INLINE static bool operator >(v3_ref lhs, v3_ref rhs)
        {
            return  lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z;
        }
        INLINE static bool operator <(v3_ref lhs, v3_ref rhs)
        {
            return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
        }
        INLINE static bool operator >=(v3_ref lhs, v3_ref rhs)
        {
            return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z;
        }
        INLINE static bool operator <=(v3_ref lhs, v3_ref rhs)
        {
            return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z;
        }
        INLINE static bool operator ==(v3_ref lhs, v3_ref rhs)
        {
            return  lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }
        INLINE static bool operator !=(v3_ref lhs, v3_ref rhs)
        {
            return !(lhs == rhs);
        }

        INLINE static v3 operator +(v3_ref lhs, v3_ref rhs)
        {
            return v3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }
        INLINE static v3 operator -(v3_ref lhs, v3_ref rhs)
        {
            return v3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }
        INLINE static v3 operator *(v3_ref lhs, v3_ref rhs)
        {
            return v3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }
        INLINE static v3 operator /(v3_ref lhs, v3_ref rhs)
        {
            return v3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
        }

        // Check if two vectors are approximately equal
        static MAY_INLINE bool Approx(v3_ref a, v3_ref b)
        {
            return  RAC_F32_APPROX(a.x, b.x) &&
                    RAC_F32_APPROX(a.y, b.y) &&
                    RAC_F32_APPROX(a.z, b.z);
        }
        static MAY_INLINE bool NotApprox(v3_ref a, v3_ref b)
        {
            return !Approx(a, b);
        }

        static MAY_INLINE v3 Cross(v3 v1, v3 v2)
        {
            return v3(  v1.y * v2.z - v1.z * v2.y,
                        v1.z * v2.x - v1.x * v2.z,
                        v1.x * v2.y - v1.y * v2.x);
        }

        const v3 V3_ZERO = v3(0.0f);
        const v3 V3_ONE = v3(1.0f);
        const v3 V3_SIGNED_ONE = -V3_ONE;

        const v3 V3_RIGHT = v3(1.0f, 0.0f, 0.0f);
        const v3 V3_LEFT = -V3_RIGHT;

        const v3 V3_UP = v3(0.0f, 1.0f, 0.0f);
        const v3 V3_DOWN = -V3_UP;

        const v3 V3_FORWARD = v3(0.0f, 0.0f, 1.0f);
        const v3 V3_BACKWARD = -V3_FORWARD;
    }

    namespace collections
    {
        namespace lists
        {
            class singleLink;

            typedef const singleLink* singleLink_ptr;
            typedef singleLink* mut_singleLink_ptr;

            typedef const singleLink& singleLink_ref;
            typedef singleLink& mut_singleLink_ref;

            const u64 NULL_LIST_ID = (u64)NULL;

            class singleLink
            {
            private:
                mut_u64 id = NULL_LIST_ID;

            public:
                mut_ptr data = nullptr;
                singleLink_ptr prev = nullptr;
                singleLink_ptr next = nullptr;

                singleLink(mut_ptr datum)
                {
                    id = rnd::xor64::GetU64();
                    data = datum;
                    prev = nullptr;
                    next = nullptr;
                }

                singleLink(mut_ptr datum, singleLink prevNode, singleLink nextNode)
                {
                    id = rnd::xor64::GetU64();
                    data = datum;
                    prev = &prevNode;
                    next = &nextNode;
                }

                INLINE u64 GetId() const { return id; }

                INLINE bool Null() const
                {
                    return  (id == NULL_LIST_ID || data == nullptr) &&
                            prev == nullptr &&
                            next == nullptr;
                }
                INLINE bool NotNull() const { return !Null(); }

                INLINE bool Head() const
                {
                    return NotNull() && (prev == nullptr && next != nullptr);
                }

                INLINE bool End() const
                {
                    return NotNull() && (prev != nullptr && next == nullptr);
                }

                INLINE bool Inside() const
                {
                    NotNull() && (prev != nullptr && next != nullptr);
                }

                INLINE ptr ToRef() const { return &id; }

                INLINE bool operator==(singleLink_ref rhs)
                {
                    return memcmp(&id, rhs.ToRef(), 4) == 0;
                }
                INLINE bool operator!=(singleLink_ref rhs)
                {
                    return memcmp(&id, rhs.ToRef(), 4) != 0;
                }

                INLINE singleLink_ptr GetHead()
                {
                    if (Head()) { return this; }

                    singleLink_ptr currNode = prev;
                    while (currNode->prev != nullptr) { currNode = prev; }
                    return currNode;
                }

                INLINE singleLink_ptr GetEnd()
                {
                    if (End()) { return this; }

                    singleLink_ptr currNode = this->prev;
                    while (currNode->next != nullptr) { currNode = this; }
                    return currNode;
                }

                INLINE singleLink_ptr FindInPlace(singleLink_ptr start, u64 targetId)
                {
                    singleLink_ptr currNode = start;
                    while ( currNode->next != nullptr &&
                            currNode->id != targetId)
                    {
                        currNode = currNode->next;
                    }

                    return currNode->NotNull() ? currNode : nullptr;
                }
                INLINE singleLink_ptr Find(u64 targetId)
                {
                    singleLink_ptr currNode = GetHead();
                    return FindInPlace(currNode, targetId);
                }
                INLINE singleLink_ptr Find(singleLink_ptr target)
                {
                    singleLink_ptr node = Head() ? this : GetHead();
                    return FindInPlace(node, target->id);
                }
            };
        }
    }

    namespace io
    {
        using namespace rac::string;

        static INLINE int Print(str_ref s)
        {
            u8ptr head = s.PtrToChars();
            mut_i32 len = s.Length();
            while (--len) { _fputchar(*head++); }
            return s.Length();
        }
        static INLINE int Println(str_ref s)
        {
            u8ptr head = s.PtrToChars();
            mut_i32 len = s.Length();
            while (--len) { _fputchar(*head++); }
            _fputchar('\r'); _fputchar('\n');
            return s.Length() + 2;
        }

        static INLINE int Print(char c) { return _fputchar(c); }
        static INLINE int Println(char c)
        {
            _fputchar(c); _fputchar('\r'); _fputchar('\n'); return 3;
        }
    }
}

#pragma warning(pop)

#endif

int main()
{
    using namespace rac::string;
    using namespace rac::mth;
    using namespace rac::io;

}
