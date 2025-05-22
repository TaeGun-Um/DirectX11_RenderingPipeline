#pragma once

#include <wrl/client.h>

template<typename T>
struct Comptr
{
    Microsoft::WRL::ComPtr<T> Ptr;

    // Ptr ���� ���ٿ� ��ȯ ������
    operator Microsoft::WRL::ComPtr<T>& () { return Ptr; }
    operator T* () const { return Ptr.Get(); }

    // ����Ʈ ������ó�� �����ϵ��� ������ �����ε�
    T* operator->() const { return Ptr.Get(); }
    T** operator&() { return Ptr.GetAddressOf(); }
    T* Get() const { return Ptr.Get(); }
    T** GetAddressOf() { return Ptr.GetAddressOf(); }

    void Reset() { Ptr.Reset(); }
};

struct CBTransform
{
    float4x4 WorldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
};

enum class CameraType
{
    None,
    Perspective,
    Orthogonal,
};
