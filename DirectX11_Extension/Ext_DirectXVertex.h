#pragma once

struct VertexData
{
	float4 POSITION;
	float4 TEXCOORD;
	float4 COLOR;
	float4 NORMAL;
};

// �Էµ� ���̾ƿ� ������ ����
class InputLayoutElement
{
public:
	// IAData �Է�
	void AddInputLayoutElement
	(
		LPCSTR _SemanticName,
		DXGI_FORMAT _Format,
		D3D11_INPUT_CLASSIFICATION _InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		UINT _InstanceDataStepRate = 0,
		UINT _AlignedByteOffset = -1,
		UINT _InputSlot = 0,
		UINT _SemanticIndex = 0
	);

	// Getter
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetElementDescsDescs() { return LayOutDescs;  }

private:
	static UINT FormatSize(DXGI_FORMAT _Format);

	std::vector<D3D11_INPUT_ELEMENT_DESC> LayOutDescs;
	int Offset = 0;

};

// ����(Vertex)�� �����ϱ� ���� Ŭ����
class Ext_DirectXVertex
{
public:
	// constrcuter destructer
	Ext_DirectXVertex() {};
	~Ext_DirectXVertex() {};

	// delete Function
	Ext_DirectXVertex(const Ext_DirectXVertex& _Other) = delete;
	Ext_DirectXVertex(Ext_DirectXVertex&& _Other) noexcept = delete;
	Ext_DirectXVertex& operator=(const Ext_DirectXVertex& _Other) = delete;
	Ext_DirectXVertex& operator=(Ext_DirectXVertex&& _Other) noexcept = delete;

	// Getter
	static InputLayoutElement& GetInputLayoutElement() { return IAElement; }
	VertexData& GetVertexData() { return VData; }

protected:
	
private:
	static InputLayoutElement IAElement; // �Էµ� ���̾ƿ� ������ ����
	VertexData VData;
	
};

