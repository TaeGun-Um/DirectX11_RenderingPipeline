#pragma once

// 입력된 레이아웃 데이터 관리
class InputLayoutElement
{
public:
	// IAData 입력
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
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetInputLayoutDescs() { return InputLayoutDescs;  }

private:
	static UINT FormatSize(DXGI_FORMAT _Format); // 포멧 크기 확인용

	std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDescs;
	int Offset = 0;

};

// 정점(Vertex)을 생성하기 위한 클래스
class Ext_DirectXVertex
{
public:
	float4 POSITION;
	float4 COLOR;
	float4 TEXCOORD;
	float4 NORMAL;

	// Getter
	static InputLayoutElement& GetInputLayoutElement() { return IAElement; }

protected:
	
private:
	static InputLayoutElement IAElement; // 입력된 레이아웃 데이터 관리
	
};

