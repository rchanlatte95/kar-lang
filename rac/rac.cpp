#include <iostream>
#include <limits>
#include <vector>
#include <math.h>
#include <float.h>
#include <stdio.h>

#ifndef RAC
#define RAC

#define MAY_INLINE __inline
#define INLINE __forceinline
#define NO_INLINE __declspec(noinline)

typedef const char* cstr;   typedef char* mut_cstr;

typedef const int8_t i8;    typedef const uint8_t u8;
typedef int8_t mut_i8;      typedef uint8_t mut_u8;

typedef const int16_t i16;  typedef const uint16_t u16;
typedef int16_t mut_i16;    typedef uint16_t mut_u16;

typedef const int32_t i32;  typedef const uint32_t u32;
typedef int32_t mut_i32;    typedef uint32_t mut_u32;

typedef const int64_t i64;  typedef const uint64_t u64;
typedef int64_t mut_i64;    typedef uint64_t mut_u64;

typedef const float f32;    typedef const float& f32_ref;   typedef float mut_f32;
typedef const double f64;   typedef const double& f64_ref;        typedef double mut_f64;

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

namespace rac
{
    const u64 BYTES_IN_KB = 1024;
    const u64 BYTES_IN_WIN_PAGE = BYTES_IN_KB * 4;
    const u64 BYTES_IN_MB = BYTES_IN_KB * 1024;

    const u64 X86_STACK_BYTE_SIZE = BYTES_IN_MB;
    const u64 X64_STACK_BYTE_SIZE = BYTES_IN_MB * 4;

    namespace logic
    {
        class Bool
        {
        private:
            bool truth_value = false;

        public:

            Bool() { truth_value = false; }
            Bool(bool b) { truth_value = b; }
            Bool(i8 i) { truth_value = i != 0; } Bool(u8 u) { truth_value = u != 0; }
            Bool(i16 i) { truth_value = i != 0; } Bool(u16 u) { truth_value = u != 0; }
            Bool(i32 i) { truth_value = i != 0; } Bool(u32 u) { truth_value = u != 0; }
            Bool(i64 i) { truth_value = i != 0; } Bool(u64 u) { truth_value = u != 0; }

            cstr Cstr() const
            {
                return truth_value ? "true" : "false";
            }
        };

        enum Comparison : int_fast8_t
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
    }

    namespace stack_utf8_string
    {
        const i32 SSTR_BASE_BYTE_SIZE = BYTES_IN_KB / 2;
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

        class alignas(8) str;
        typedef const str* str_ptr;   typedef str* mut_str_ptr;
        typedef const str& str_ref;   typedef str& mut_str_ref;

        class alignas(8) str
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
                chars[len] = 0;
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

            INLINE i32 MaxLength() const { return SSTR_MAX_LEN; }
            INLINE i32 Length() const { return len; }
            INLINE utf8ptr Str() { return &chars[0]; }
            INLINE bool Empty() const { return len == 0; }
            INLINE void Clear() { len = 0; chars[0] = 0; }
            INLINE u8& operator[](i32 index) { return chars[index]; }

            INLINE str_ref operator=(cstr rhs)
            {
                len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);
                chars[0] = 0;
                chars[SSTR_MAX_LEN] = 0;
                memcpy_s(chars, SSTR_MAX_LEN, rhs, len);
                chars[len] = 0;

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
                return memcmp(ptr(&len), ptr(&rhs.len), len) == 0;
            }
            INLINE bool operator!=(str_ref rhs)
            {
                return memcmp(ptr(&len), ptr(&rhs.len), len) != 0;
            }

            INLINE bool operator==(cstr rhs)
            {
                const i32 rhs_len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);
                return rhs_len == this->len && memcmp(this->chars, rhs, rhs_len) == 0;
            }
            INLINE bool operator!=(cstr rhs)
            {
                const i32 rhs_len = (i32)strnlen_s(rhs, SSTR_MAX_LEN);
                return rhs_len != this->len || memcmp(this->chars, rhs, rhs_len) != 0;
            }

            INLINE logic::comp Compare(str_ref arg)
            {
                return logic::comp(memcmp(ptr(&this->len), ptr(&arg.len), this->len));
            }
            INLINE logic::comp Compare(cstr c_str)
            {
                return logic::comp(memcmp(ptr(&this->chars[0]), ptr(c_str), this->len));
            }

            INLINE cstr Cstr() const { return (cstr)(&chars[0]); }
            INLINE std::string StdStr() const { return std::string((cstr)chars, len); }
            INLINE ptr Ptr() const { return (ptr)(&len); }
            INLINE str_ref Ref() const { return *this; }

            INLINE u8& First() { return chars[0]; }
            INLINE u8& Last() { return chars[len - 1]; }
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

                const u8ptr fxd_end = chars + len - 1;
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
    }

    namespace mth
    {
        using namespace rac::stack_utf8_string;

        f32 FLOAT_EPSILON = 0.0001f;
        f32 SIGNED_FLOAT_EPSILON = -FLOAT_EPSILON;
        i32 FLT_STR_MAX = FLT_DECIMAL_DIG * 2 + 4;
        i32 FLT_STR_LEN = FLT_STR_MAX - 1;
        #define RAC_F32_APPROX(a, b) (fabsf(a - b) <= FLOAT_EPSILON)
        #define RAC_F32_APPROX_MORE(a, b) (fabsf(a - b) > FLOAT_EPSILON)
        #define RAC_F32_APPROX_LESS(a, b) (fabsf(a - b) < SIGNED_FLOAT_EPSILON)

        class v2;
        typedef const v2* v2_ptr;   typedef v2* mut_v2_ptr;
        typedef const v2& v2_ref;   typedef v2& mut_v2_ref;

        class v2
        {
        public:
            mut_f32 x = 0.0f;
            mut_f32 y = 0.0f;

            v2(i8 _x, i8 _y) { x = _x; y = _y; }
            v2(i16 _x, i16 _y) { x = _x; y = _y; }
            v2(i32 _x, i32 _y) { x = _x; y = _y; }
            v2(i64 _x, i64 _y) { x = _x; y = _y; }
            v2(i8 a) { x = a; y = a; }
            v2(i16 a) { x = a; y = a; }
            v2(i32 a) { x = a; y = a; }
            v2(i64 a) { x = a; y = a; }

            v2(f32 _x, f32 _y) { x = _x; y = _y; }
            v2(f32 a) { x = a; y = a; }
            v2() { }

            INLINE v2_ref operator=(v2_ref rhs)
            {
                x = rhs.x;
                y = rhs.y;
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
                x /= a;
                y /= a;
            }

            INLINE ptr Ptr() const { return (ptr)(&x); }
            INLINE v2_ref Ref() const { return *this; }

            INLINE str_ref Str(mut_str_ref str) const
            {
                char buff[FLT_STR_MAX] = { 0 };
                sprintf_s(buff, "(%0.4f, %0.4f)", x, y);
                str = buff;
                return str;
            }

            INLINE v2_ref Clamp(f32 x_min, f32 x_max, f32 y_min, f32 y_max)
            {
                x = x > x_max ? x_max : (x < x_min ? x_min : x);
                y = y > y_max ? y_max : (y < y_min ? y_min : y);
            }
            INLINE v2_ref Clamp(v2 min, v2 max)
            {
                x = x > max.x ? max.x : (x < min.x ? min.x : x);
                y = y > max.y ? max.y : (y < min.y ? min.y : y);
            }
        };

        INLINE static bool operator >(v2_ref lhs, v2_ref rhs)
        {
            return  RAC_F32_APPROX_MORE(lhs.x, rhs.x) &&
                    RAC_F32_APPROX_MORE(lhs.y, rhs.y);
        }
        INLINE static bool operator <(v2_ref lhs, v2_ref rhs)
        {
            return lhs > rhs;
        }
        INLINE static bool operator <=(v2_ref lhs, v2_ref rhs)
        {
            return !(lhs > rhs);
        }
        INLINE static bool operator >=(v2_ref lhs, v2_ref rhs)
        {
            return !(lhs < rhs);
        }
        INLINE static bool operator ==(v2_ref lhs, v2_ref rhs)
        {
            return RAC_F32_APPROX(lhs.x, rhs.x) && RAC_F32_APPROX(lhs.y, rhs.y);
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
    }

    namespace collections
    {
        namespace lists
        {
            class singleLink;
            typedef const singleLink* singleLink_ptr;
            typedef const singleLink& singleLink_ref;

            const u64 NULL_LIST_ID = 0;

            class singleLink
            {
                u64 id = NULL_LIST_ID;
                ptr data = nullptr;
                singleLink_ptr prev = nullptr;
                singleLink_ptr next = nullptr;

                INLINE bool Invalid() const {
                    return    id == NULL_LIST_ID &&
                        data == nullptr &&
                        prev == nullptr &&
                        next == nullptr;
                }

                INLINE bool Head() const { return prev == nullptr && next != nullptr; }
                INLINE bool End() const { return prev != nullptr && next == nullptr; }
            };
        }
    }
}

#endif

static void test_str()
{
    using namespace std;
    using namespace rac::stack_utf8_string;
    using namespace rac::logic;

    str stack_str_0 = str();

    std::cout << "sizeof(sstr) = " << sizeof(str) << " bytes\r\n" << std::endl;

    stack_str_0 = "Hello World!";
    str stack_str_1 = str(stack_str_0, 0, 5);
    stack_str_1 += " World!";
    std::cout << "stack_str_0 := " << stack_str_0.StdStr() << std::endl;
    std::cout << "stack_str_1 := " << stack_str_1.Cstr() << std::endl;

    std::cout << "\r\nstack_str_0 == stack_str_1 => " << Bool(stack_str_0 == stack_str_1).Cstr() << std::endl;
    std::cout << "\r\nstack_str_0 == \"Hello World!\" => " << Bool(stack_str_0 == "Hello World!").Cstr() << std::endl;

    comp comparisonResult = stack_str_0.Compare(stack_str_1);
    std::cout << "stack_str_0.Compare(stack_str_1) => " << comparisonResult.Cstr() << std::endl;

    comparisonResult = stack_str_0.Compare("testing");
    std::cout << "stack_str_0.Compare(\"testing\") => " << comparisonResult.Cstr() << std::endl;
    std::cout << "\r\n\r\n-----\r\n" << std::endl;

    str stack_str_3 = str("     \r\n\t     test     \r\n\t");
    std::cout << "Before trim(): => |" << stack_str_3.Cstr() << "|" << std::endl;
    std::cout << "\r\nAfter trim(): => |" << stack_str_3.TrimStart().Cstr() << "|" << std::endl;
}

int main()
{
    using namespace std;
    using namespace rac::stack_utf8_string;
    using namespace rac::mth;

    str testStr;
    v2 test(1.0f, 2.0f);
    cout << test.Str(testStr).Cstr() << endl;
}
