#pragma once

#include <wrl/client.h>

// Microsoft::WRL::ComPtr<T> 래핑 구조체
template<typename T>
struct COMPTR
{
public:
    // 생성자
    COMPTR() {}
    COMPTR(nullptr_t) {}

    // 연산자 오버로딩
    operator Microsoft::WRL::ComPtr<T>& () { return Ptr; } // Ptr 직접 접근용 변환 연산자
    operator T* () const { return Ptr.Get(); } // Ptr 직접 접근용 변환 연산자
    T* operator->() const { return Ptr.Get(); } // 스마트 포인터처럼 동작하도록 연산자 오버로드
    T** operator&() { return Ptr.GetAddressOf(); } // 스마트 포인터처럼 동작하도록 연산자 오버로드
    operator bool() const { return Ptr != nullptr; } // 조건문에서 사용(null인지 아닌지 판별)

    // Getter
    T* Get() const { return Ptr.Get(); } // 원시 포인터 반환
    T** GetAddressOf() { return Ptr.GetAddressOf(); } // 레퍼런스 반환(함수 전달용)

    void Reset() { Ptr.Reset(); } // 현재 보유중인 COM 객체 해제(Release())를 호출하고 nullptr로 설정
    T* Detach() { return Ptr.Detach(); } // 소유권 이전

public:
    Microsoft::WRL::ComPtr<T> Ptr = nullptr;
};