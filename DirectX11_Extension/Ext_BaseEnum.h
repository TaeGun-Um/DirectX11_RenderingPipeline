#pragma once

#include <wrl/client.h>

template<typename T>
struct Comptr
{
    Microsoft::WRL::ComPtr<T> Ptr;

    // Ptr 직접 접근용 변환 연산자
    operator Microsoft::WRL::ComPtr<T>& () { return Ptr; }
    operator T* () const { return Ptr.Get(); }

    // 스마트 포인터처럼 동작하도록 연산자 오버로드
    T* operator->() const { return Ptr.Get(); }
    T** operator&() { return Ptr.GetAddressOf(); }
    T* Get() const { return Ptr.Get(); }
    T** GetAddressOf() { return Ptr.GetAddressOf(); }

    void Reset() { Ptr.Reset(); }
};

enum class CameraType
{
    None,
    Perspective,
    Orthogonal,
};