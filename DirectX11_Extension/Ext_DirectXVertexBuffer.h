#pragma once
#include "Ext_ResourceManager.h"

// 버텍스 버퍼(Vertex Buffer) 생성을 위한 클래스
class Ext_DirectXVertexBuffer : public Ext_ResourceManager<Ext_DirectXVertexBuffer>
{
	friend class Ext_DirectXMesh;

public:
	// constrcuter destructer
	Ext_DirectXVertexBuffer() {}
	~Ext_DirectXVertexBuffer() {}

	// delete Function
	Ext_DirectXVertexBuffer(const Ext_DirectXVertexBuffer& _Other) = delete;
	Ext_DirectXVertexBuffer(Ext_DirectXVertexBuffer&& _Other) noexcept = delete;
	Ext_DirectXVertexBuffer& operator=(const Ext_DirectXVertexBuffer& _Other) = delete;
	Ext_DirectXVertexBuffer& operator=(Ext_DirectXVertexBuffer&& _Other) noexcept = delete;

	// Vertex Buffer 생성 및 저장
	template<typename VertexLayout>
	static std::shared_ptr<Ext_DirectXVertexBuffer> CreateVertexBuffer(std::string_view _Name, const std::vector<VertexLayout>& _Vertexs)
	{
		std::shared_ptr<Ext_DirectXVertexBuffer> NewVertexBuffer = Ext_ResourceManager::CreateNameResource(_Name);
		NewVertexBuffer->InputLayout = std::shared_ptr<InputLayoutData>(&VertexLayout::GetInputLayoutData(), [](InputLayoutData*) {});
		NewVertexBuffer->CreateVertexBuffer(&_Vertexs[0], sizeof(VertexLayout), static_cast<UINT>(_Vertexs.size()));

		return NewVertexBuffer;
	}

	// Getter
	std::shared_ptr<class InputLayoutData> GetInputLayout() { return InputLayout; }
	COMPTR<ID3D11Buffer>& GetVertexBuffer() { return VertexBuffer; }
	UINT GetVertexSize() { return VertexSize; }
	UINT GetVertexCount() { return VertexCount; }
	UINT GetBufferSize() { return VertexBufferInfo.ByteWidth; }

protected:
	
private:
	void CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount); // Vertex Buffer 생성 및 저장
	void VertexBufferSetting(); // IASetVertexBuffers() 호출

	std::shared_ptr<class InputLayoutData> InputLayout = nullptr;			     // 생성된 입력 레이아웃 정보 저장용
	D3D11_BUFFER_DESC VertexBufferInfo = { 0, };  // 버텍스 버퍼 DESC 저장용
	COMPTR<ID3D11Buffer> VertexBuffer = nullptr;   // 버텍스 버퍼 인터페이스 저장용
	UINT VertexSize = 0;											 // 버텍스 사이즈
	UINT VertexCount = 0;										 // 버텍스 갯수
	
	UINT Offset = 0;
};
// [ID3D11Buffer]
// 버퍼 리소스를 나타내는 인터페이스
// 일반적으로 GPU에 데이터를 전달할 때 사용되는 가장 기본적인 객체로, 3가지 용도로 사용됨
// 1. Vertex Buffer : 정점 데이터를 저장하고 GPU에 전달
// 2. Index Buffer : 정점 배열의 인덱스 정보 저장
// 3. Constant Buffer : 셰이더에 전달할 상수 데이터 저장(전역 변수처럼 사용)

// [D3D11_BUFFER_DESC]
// ID3D11Buffer 생성 시, 버퍼의 용도와 특성을 정의하기 위한 구조체
// GPU 메모리에서 버퍼가 어떻게 사용될지, 접근될지 지정하는 정보