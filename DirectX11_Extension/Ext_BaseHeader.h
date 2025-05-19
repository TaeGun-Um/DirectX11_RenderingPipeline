#pragma once

#include <wrl/client.h>

// Microsoft::WRL::ComPtr<T> ���� ����ü
template<typename T>
struct COMPTR
{
public:
    // ������
    COMPTR() {}
    COMPTR(nullptr_t) {}

    // ������ �����ε�
    operator Microsoft::WRL::ComPtr<T>& () { return Ptr; } // Ptr ���� ���ٿ� ��ȯ ������
    operator T* () const { return Ptr.Get(); } // Ptr ���� ���ٿ� ��ȯ ������
    T* operator->() const { return Ptr.Get(); } // ����Ʈ ������ó�� �����ϵ��� ������ �����ε�
    T** operator&() { return Ptr.GetAddressOf(); } // ����Ʈ ������ó�� �����ϵ��� ������ �����ε�
    operator bool() const { return Ptr != nullptr; } // ���ǹ����� ���(null���� �ƴ��� �Ǻ�)

    // Getter
    T* Get() const { return Ptr.Get(); } // ���� ������ ��ȯ
    T** GetAddressOf() { return Ptr.GetAddressOf(); } // ���۷��� ��ȯ(�Լ� ���޿�)

    void Reset() { Ptr.Reset(); } // ���� �������� COM ��ü ����(Release())�� ȣ���ϰ� nullptr�� ����
    T* Detach() { return Ptr.Detach(); } // ������ ����

public:
    Microsoft::WRL::ComPtr<T> Ptr = nullptr;
};