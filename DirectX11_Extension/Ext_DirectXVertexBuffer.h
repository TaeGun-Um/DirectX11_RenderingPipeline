#pragma once
#include "Ext_ResourceManager.h"

// ���ؽ� ����(Vertex Buffer) ������ ���� Ŭ����
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

	// Vertex Buffer ���� �� ����
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
	void CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount); // Vertex Buffer ���� �� ����
	void VertexBufferSetting(); // IASetVertexBuffers() ȣ��

	std::shared_ptr<class InputLayoutData> InputLayout = nullptr;			     // ������ �Է� ���̾ƿ� ���� �����
	D3D11_BUFFER_DESC VertexBufferInfo = { 0, };  // ���ؽ� ���� DESC �����
	COMPTR<ID3D11Buffer> VertexBuffer = nullptr;   // ���ؽ� ���� �������̽� �����
	UINT VertexSize = 0;											 // ���ؽ� ������
	UINT VertexCount = 0;										 // ���ؽ� ����
	
	UINT Offset = 0;
};
// [ID3D11Buffer]
// ���� ���ҽ��� ��Ÿ���� �������̽�
// �Ϲ������� GPU�� �����͸� ������ �� ���Ǵ� ���� �⺻���� ��ü��, 3���� �뵵�� ����
// 1. Vertex Buffer : ���� �����͸� �����ϰ� GPU�� ����
// 2. Index Buffer : ���� �迭�� �ε��� ���� ����
// 3. Constant Buffer : ���̴��� ������ ��� ������ ����(���� ����ó�� ���)

// [D3D11_BUFFER_DESC]
// ID3D11Buffer ���� ��, ������ �뵵�� Ư���� �����ϱ� ���� ����ü
// GPU �޸𸮿��� ���۰� ��� ������, ���ٵ��� �����ϴ� ����