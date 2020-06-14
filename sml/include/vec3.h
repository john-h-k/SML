#ifndef sml_vec3_h__
#define sml_vec3_h__

/* vec3.h -- vec3 implementation of the 'Simple Math Library'
  Copyright (C) 2020 Roderick Griffioen
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <string>
#include <type_traits>

#include "smltypes.h"

namespace sml
{
    template<typename T>
    class vec3view;

    template<typename T>
    class alignas(simdalign<T>::value) vec3
    {
        public:
            constexpr vec3() noexcept
            {
                zero();
                v[3] = 0;
            }

            constexpr vec3(T x, T y, T z) noexcept
            {
                set(x, y, z);
                v[3] = 0;
            }

            constexpr explicit vec3(T v) noexcept
            {
                set(v, v, v);
                v[3] = 0;
            }

            constexpr vec3(const vec3& other) noexcept
            { 
                set(const_cast<T*>(other.v));
                v[3] = 0;
            }

            constexpr vec3(vec3&& other) noexcept
            {
                v[0] = std::move(other.v[0]);
                v[1] = std::move(other.v[1]);
                v[2] = std::move(other.v[2]);
                v[3] = 0;
            }

            constexpr vec3& operator = (const vec3view<T>& other) noexcept;
            constexpr vec3& operator = (vec3view<T>&& other) noexcept;

            SML_NO_DISCARD constexpr void zero() noexcept
            {
                set(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
            }

            SML_NO_DISCARD constexpr void set(T x, T y, T z) noexcept
            {
                this->x = x;
                this->y = y;
                this->z = z;
            }

            SML_NO_DISCARD constexpr void set(T v[3]) noexcept
            {
                this->v[0] = v[0];
                this->v[1] = v[1];
                this->v[2] = v[2];
            }

            // Operators
            inline constexpr bool operator == (const vec3& other) const noexcept
            {
                if constexpr (std::is_same<T, f32>::value)
                {
                    union m128
                    {
                        __m128 f;
                        __m128i i;
                    };

                    __m128 me = _mm_load_ps(v);
                    __m128 ot = _mm_load_ps(other.v);

                    m128 cmp = { _mm_cmpeq_ps(me, ot) };
                    s32 result = _mm_testc_si128(cmp.i, cmp.i);

                    return result != 0;
                }

                if constexpr (std::is_same<T, f64>::value)
                {
                    union m128
                    {
                        __m128d d;
                        __m128i i;
                    };

                    __m128d me = _mm_load_pd(v);
                    __m128d ot = _mm_load_pd(other.v);

                    m128 cmp = { _mm_cmpeq_pd(me, ot) };
                    s32 result = _mm_testc_si128(cmp.i, cmp.i);

                    return result != 0;
                }

                return x == other.x && y == other.y && z == other.z;
            }

            inline constexpr bool operator != (const vec3& other) const noexcept
            {
                if constexpr (std::is_same<T, f32>::value)
                {
                    union m128
                    {
                        __m128 f;
                        __m128i i;
                    };

                    __m128 me = _mm_load_ps(v);
                    __m128 ot = _mm_load_ps(other.v);

                    m128 cmp = { _mm_cmpneq_ps(me, ot) };
                    s32 result = _mm_testc_si128(cmp.i, cmp.i);

                    return result != 0;
                }

                if constexpr (std::is_same<T, f64>::value)
                {
                    union m128
                    {
                        __m128d d;
                        __m128i i;
                    };

                    __m128d me = _mm_load_pd(v);
                    __m128d ot = _mm_load_pd(other.v);

                    m128 cmp = { _mm_cmpneq_pd(me, ot) };
                    s32 result = _mm_testc_si128(cmp.i, cmp.i);

                    return result != 0;
                }

                return x != other.x || y != other.y || z != other.z;
            }

            constexpr vec3& operator = (const vec3& other) noexcept
            {
                set(other.v);

                return *this;
            }

            constexpr vec3& operator = (vec3&& other) noexcept
            {
                v[0] = std::move(other.v[0]);
                v[1] = std::move(other.v[1]);
                v[2] = std::move(other.v[2]);

                return *this;
            }

            vec3& operator += (const vec3& other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_load_ps(other.v);
                    __m128 res = _mm_add_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_load_pd(other.v);
                    __m256d res = _mm256_add_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x += other.x;
                y += other.y;
                z += other.z;

                return *this;
            }

            vec3& operator -= (const vec3& other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_load_ps(other.v);
                    __m128 res = _mm_sub_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_load_pd(other.v);
                    __m256d res = _mm256_sub_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x -= other.x;
                y -= other.y;
                z -= other.z;

                return *this;
            }

            vec3& operator *= (const vec3& other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_load_ps(other.v);
                    __m128 res = _mm_mul_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_load_pd(other.v);
                    __m256d res = _mm256_mul_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x *= other.x;
                y *= other.y;
                z *= other.z;

                return *this;
            }

            vec3& operator *= (const T other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_broadcast_ss(&other);
                    __m128 res = _mm_mul_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_set1_pd(other);
                    __m256d res = _mm256_mul_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x *= other;
                y *= other;
                z *= other;

                return *this;
            }

            vec3& operator /= (const vec3& other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_load_ps(other.v);
                    __m128 res = _mm_div_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_load_pd(other.v);
                    __m256d res = _mm256_div_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x /= other.x;
                y /= other.y;
                z /= other.z;

                return *this;
            }

            vec3& operator /= (const T other) noexcept
            {
                if constexpr(std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 him = _mm_broadcast_ss(&other);
                    __m128 res = _mm_div_ps(me, him);

                    _mm_store_ps(v, res);

                    return *this;
                }

                if constexpr(std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d him = _mm256_set1_pd(other);
                    __m256d res = _mm256_div_pd(me, him);

                    _mm256_store_pd(v, res);

                    return *this;
                }

                x /= other;
                y /= other;
                z /= other;

                return *this;
            }

            // Operations
            SML_NO_DISCARD inline constexpr T dot(vec3 other) const noexcept
            {
                if constexpr (std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(v);
                    __m128 ot = _mm_load_ps(other.v);
                    __m128 product = _mm_mul_ps(me, ot);
                    __m128 dp = _mm_hadd_ps(product, product);

                    s32 res = _mm_extract_epi32(_mm_hadd_ps(dp, dp), 0);

                    return *reinterpret_cast<f32*>(&(res));
                }

                if constexpr (std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(v);
                    __m256d ot = _mm256_load_pd(other.v);
                    __m256d product = _mm256_mul_pd(me, ot);
                    __m256d dp = _mm256_hadd_pd(product, product);

                    s32 res = _mm256_extract_epi32(_mm256_hadd_pd(dp, dp), 0);

                    return *reinterpret_cast<f64*>(&(res));
                }

                return (x * other.x) + (y * other.y) + (z * other.z);
            }

            SML_NO_DISCARD inline constexpr T length() const noexcept
            {
                return sml::sqrt((x * x) + (y * y) + (z * z));
            }

            SML_NO_DISCARD inline constexpr T lengthsquared() const noexcept
            {
                return (x * x) + (y * y) + (z * z);
            }

            SML_NO_DISCARD inline constexpr vec3& normalize() noexcept
            {
                float mag = length();

                if(mag > constants::epsilon)
                    *this /= length();
                else
                    set(0, 0, 0);

                return *this;
            }

            SML_NO_DISCARD inline constexpr vec3 normalized() const noexcept
            {
                vec3 copy(x, y, z);
                return copy.normalize();
            }

            SML_NO_DISCARD inline constexpr bool any() const noexcept
            {
                return x || y || z;
            }

            SML_NO_DISCARD inline constexpr bool all() const noexcept
            {
                return x && y && z;
            } 

            SML_NO_DISCARD inline constexpr bool none() const noexcept
            {
                return !x && !y && !z;
            }

            SML_NO_DISCARD inline constexpr std::string toString() const noexcept
            {
                return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
            }

            // Statics
            SML_NO_DISCARD static inline constexpr T distance(const vec3& a, const vec3& b) noexcept
            {
                vec3 delta = b - a;
                return delta.length();
            }

            SML_NO_DISCARD static inline constexpr vec3 min(const vec3& a, const vec3& b) noexcept
            {
                vec3 result;

                if constexpr (std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(a.v);
                    __m128 ot = _mm_load_ps(b.v);

                    __m128 maxres = _mm_min_ps(me, ot);

                    _mm_store_ps(result.v, maxres);

                    return result;
                }

                if constexpr (std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(a.v);
                    __m256d ot = _mm256_load_pd(b.v);

                    __m256d maxres = _mm256_min_pd(me, ot);

                    _mm256_store_pd(result.v, maxres);

                    return result;
                }

                return 
                {
                    sml::min(a.x, b.x), 
                    sml::min(a.y, b.y),
                    sml::min(a.z, b.z)
                };
            }

            SML_NO_DISCARD static inline constexpr vec3 max(const vec3& a, const vec3& b) noexcept
            {
                vec3 result;

                if constexpr (std::is_same<T, f32>::value)
                {
                    __m128 me = _mm_load_ps(a.v);
                    __m128 ot = _mm_load_ps(b.v);

                    __m128 maxres = _mm_max_ps(me, ot);

                    _mm_store_ps(result.v, maxres);

                    return result;
                }

                if constexpr (std::is_same<T, f64>::value)
                {
                    __m256d me = _mm256_load_pd(a.v);
                    __m256d ot = _mm256_load_pd(b.v);

                    __m256d maxres = _mm256_max_pd(me, ot);

                    _mm256_store_pd(result.v, maxres);

                    return result;
                }

                return 
                {
                    sml::max(a.x, b.y), 
                    sml::max(a.y, b.y),
                    sml::max(a.z, b.z)
                };
            }

            SML_NO_DISCARD static inline constexpr vec3 clamp(const vec3& v, const vec3& a, const vec3& b) noexcept
            {
                return max(a, min(v, b));
            }

            SML_NO_DISCARD static inline constexpr vec3 lerp(const vec3& a, const vec3& b, T t) noexcept
            {
                T retX = sml::lerp(a.x, b.x, t);
                T retY = sml::lerp(a.y, b.y, t);
                T retZ = sml::lerp(a.z, b.z, t);

                return vec3(retX, retY, retZ);
            }

            SML_NO_DISCARD static inline constexpr vec3 lerpclamped(const vec3& a, const vec3& b, T t) noexcept
            {
                T retX = sml::lerpclamped(a.x, b.x, t);
                T retY = sml::lerpclamped(a.y, b.y, t);
                T retZ = sml::lerpclamped(a.z, b.z, t);

                return vec3(retX, retY, retZ);
            }

            SML_NO_DISCARD static inline constexpr vec3 cross(const vec3& left, const vec3& right) noexcept
            {
                return
                {
                    left.y * right.z - left.z * right.y,
                    left.z * right.x - left.x * right.z,
                    left.x * right.y - left.y * right.x
                };
            }

            SML_NO_DISCARD static inline constexpr vec3 project(const vec3& a, const vec3& b) noexcept
            {
                return b * (dot(a, b) / dot(b, b));
            }

            // Data
            union
            {
                struct
                {
                    T x, y, z;
                };

                T v[4];
            };
    };

    // Operators
    template<typename T>
    constexpr vec3<T> operator + (vec3<T> left, vec3<T> right) noexcept
    {
        left += right;
        return left;
    }

    template<typename T>
    constexpr vec3<T> operator - (vec3<T> left, vec3<T> right) noexcept
    {
        left -= right;
        return left;
    }

    template<typename T>
    constexpr vec3<T> operator * (vec3<T> left, vec3<T> right) noexcept
    {
        left *= right;
        return left;
    }

    template<typename T>
    constexpr vec3<T> operator * (vec3<T> left, T right) noexcept
    {
        left += right;
        return left;
    }

    template<typename T>
    constexpr vec3<T> operator / (vec3<T> left, vec3<T> right) noexcept
    {
        left /= right;
        return left;
    }

    template<typename T>
    constexpr vec3<T> operator / (vec3<T> left, T right) noexcept
    {
        left += right;
        return left;
    }

    // Predefined types
    typedef vec3<bool> bvec3;
    typedef vec3<u32> uvec3;
    typedef vec3<s32> ivec3;
    typedef vec3<f32> fvec3;
    typedef vec3<f64> dvec3;

    template<typename T>
    class vec3view
    {
    public:
        constexpr vec3view() = default;

        constexpr vec3view(const vec3<T>& other) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }

        constexpr vec3view(vec3<T>&& other) noexcept
        {
            x = std::move(other.x);
            y = std::move(other.y);
            z = std::move(other.z);
        }

        constexpr vec3view& operator = (const vec3<T>& other) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;

            return *this;
        }

        constexpr vec3view& operator = (vec3<T>&& other) noexcept
        {
            x = std::move(other.x);
            y = std::move(other.y);
            z = std::move(other.z);

            return *this;
        }

        T x = static_cast<T>(0), y = static_cast<T>(0), z = static_cast<T>(0);
    };

    template<typename T>
    constexpr vec3<T>& vec3<T>::operator = (const vec3view<T>& other) noexcept
    {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }

    template<typename T>
    constexpr vec3<T>& vec3<T>::operator = (vec3view<T>&& other) noexcept
    {
        x = std::move(other.x);
        y = std::move(other.y);
        z = std::move(other.z);

        return *this;
    }
} // namespace sml

#endif // sml_vec3_h__