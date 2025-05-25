#include "PrecompileHeader.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXDevice.h"

std::map<int, std::map<std::string, std::shared_ptr<Ext_DirectXConstantBuffer>>> Ext_DirectXConstantBuffer::ConstantBuffers;
std::string Ext_DirectXConstantBuffer::Name = "";

// CreateBuffer()를 통해 상수 버퍼를 생성
void Ext_DirectXConstantBuffer::CreateConstantBuffer(const D3D11_SHADER_BUFFER_DESC& _BufferDesc)
{
	ConstantBufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 셰이더에 바인딩할 상수 버퍼임을 명시
	ConstantBufferInfo.ByteWidth = _BufferDesc.Size; // 바이트 크기를 HLSL 셰이더에서 얻은 값으로 설정(144, 256 등)
	ConstantBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE; // CPU가 이 버퍼를 쓰기 전용으로 접근함을 명시(Map() 호출을 위해 필요)
	if (0 == ConstantBufferInfo.CPUAccessFlags)
	{
		ConstantBufferInfo.Usage = D3D11_USAGE_DEFAULT; // CPU에서 직접 수정하지 않고, GPU 측에서 자주 사용하는 버퍼
	}
	else 
	{
		ConstantBufferInfo.Usage = D3D11_USAGE_DYNAMIC; // CPU에서 자주 수정할 수 있는 버퍼, 주로 MAP() 방식과 함께 사용
	}


	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&ConstantBufferInfo, nullptr, ConstantBuffer.GetAddressOf()))
	{
		MsgAssert("상수 버퍼 생성에 실패했습니다.");
		return;
	}
	// <<설명>>
	/*1. D3D11_BUFFER_DESC 구조체 주소*/
	/*2. 초기값 (nullptr이면 비워진 상태로 생성)*/
	/*3. 생성된 ID3D11Buffer를 받을 smart pointer 주소*/
}

// Map(), Unmap() 실시
void Ext_DirectXConstantBuffer::ChangeData(const void* _Data, UINT _Size)
{
	// 머티리얼들은 상수버퍼나 이런걸 통일해 놓은 것이다.
	if (nullptr == _Data)
	{
		std::string CurName = Name;
		MsgAssert(CurName + "에 nullptr인 데이터를 세팅하려고 했습니다.");
		return;
	}

	if (ConstantBufferInfo.ByteWidth != _Size)
	{
		std::string CurName = Name;
		MsgAssert(CurName + "크기가 다른 데이터가 들어왔습니다.");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE SettingResources = { 0, };

	// 매 프레임마다 카메라 위치, 뷰/프로젝션 행렬, 시간, 조명 데이터 등을 셰이더에 넘겨야 할 때(버퍼의 내용을 바꾸는 경우)를 위해 D3D11_CPU_ACCESS_WRITE 설정, 이러면 반드시 D3D11_USAGE_DYNAMIC 이어야함
	// 설정이 없으면 Map이 불가능 → UpdateSubresource()를 사용해야 함 (비효율적), GPU에서만 읽고 CPU가 직접 쓰지 못해 프레임별 갱신이 불가능
	Ext_DirectXDevice::GetContext()->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SettingResources); // GPU 버퍼를 CPU에서 쓸 수 있도록 잠금
	// 1. 실제 GPU 상수 버퍼 객체
	// 2. 서브 리소스 index(항상 0)
	// 3. 기존 내용을 폐기하고 새로 쓰기(가장 빠름)
	// 4. ?
	// 5. 매핑된 데이터 포인터를 받을 구조체

	if (SettingResources.pData == nullptr)
	{
		std::string CurName = Name;
		MsgAssert(CurName + " 그래픽카드에게 메모리 접근을 허가받지 못했습니다.");
		return;
	}
	memcpy_s(SettingResources.pData, ConstantBufferInfo.ByteWidth, _Data, ConstantBufferInfo.ByteWidth); // CPU측 메모리 _Data의 내용을 GPU측 상수 버퍼로 복사
	Ext_DirectXDevice::GetContext()->Unmap(ConstantBuffer, 0); // 쓰기 완료(잠금 해제), GPU에게 알려줌, 이거 안하면 렌더링 중 GPU가 해당 버퍼를 읽으려할 때 충돌 발생 가능
}

// 정점 셰이더 바인딩
void Ext_DirectXConstantBuffer::VSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->VSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
	// 1. 셰이더 내 CBuffer가 바인딩된 슬롯 인덱스(cbuffer TransformData : register(b0) 한개만 있으니, 0 전달됨)
	// 2. 설정할 버퍼 개수(하나만 바인딩)
	// 3. ID3D11Buffer* 주소 전달
}

// PSSetConstantBuffers() 호출
void Ext_DirectXConstantBuffer::PSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->PSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
	// 아직 뭐 없음
}

// CSSetConstantBuffers() 호출
void Ext_DirectXConstantBuffer::CSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->CSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
}