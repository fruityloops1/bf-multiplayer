#pragma once

#include "math/seadMathCalcCommon.h"
#include <sead/math/seadMathNumbers.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

namespace pe {

template <typename T>
struct Constants {
    Constants() = delete;

    constexpr static T degreesToRadians = sead::numbers::pi_v<T> / T(180);
    constexpr static T radiansToDegrees = T(180) / sead::numbers::pi_v<T>;
};

sead::Vector3f quatToRotate(const sead::Quatf& quat);

template <typename T>
sead::Vector3<T> slerp(const sead::Vector3<T>& v1, const sead::Vector3<T>& v2, T t)
{
    T dot = v1.dot(v2);
    T theta = sead::MathCalcCommon<T>::acos(dot);

    if (sead::MathCalcCommon<T>::abs(theta) < T(0.001))
        return v1 + (v2 - v1) * t;

    T sinTheta = sead::MathCalcCommon<T>::sin(theta);

    T s1 = sead::MathCalcCommon<T>::sin((T(1) - t) * theta) / sinTheta;
    T s2 = sead::MathCalcCommon<T>::sin(t * theta) / sinTheta;

    return v1 * s1 + v2 * s2;
}

template <typename T>
sead::Vector2<T> abs(const sead::Vector2<T>& vec)
{
    sead::Vector2<T> newVec = vec;
    newVec.x = std::abs(newVec.x);
    newVec.y = std::abs(newVec.y);
    return newVec;
}

template <typename T>
sead::Vector3<T> abs(const sead::Vector3<T>& vec)
{
    sead::Vector3<T> newVec = vec;
    newVec.x = std::abs(newVec.x);
    newVec.y = std::abs(newVec.y);
    newVec.z = std::abs(newVec.z);
    return newVec;
}

template <typename T>
T dif(T value1, T value2)
{
    return sead::MathCalcCommon<T>::abs(value1 - value2);
}

template <typename T>
T easeInSine(T t)
{
    return 1.0 - sead::MathCalcCommon<T>::cos((t * sead::MathCalcCommon<T>::pi()) / 2.0);
}

template <typename T>
T easeInQuad(T t) { return t * t; }

template <typename T>
T easeOutQuad(T t) { return 1 - (1 - t) * (1 - t); }

template <typename T>
T easeInOutSine(T x)
{
    return -0.5f * (sead::MathCalcCommon<T>::cos(sead::MathCalcCommon<T>::pi() * x) - 1.0f);
}

template <typename T>
T jumpQuad(T t)
{
    return 1 - (1 - t * 2) * (1 - t * 2);
}

} // namespace pe