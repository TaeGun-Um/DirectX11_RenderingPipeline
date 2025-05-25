#pragma once

// 입력된 레이아웃 데이터 정보
class InputLayoutData
{
public:
	// IAData 입력
	void AddInputLayoutDesc
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

// 입력 레이아웃을 생성 후 저장하기 위한 클래스(InputLayoutData 활용)
class Ext_DirectXVertexData
{
public:
	float4 POSITION;
	float4 COLOR;
	float4 TEXCOORD;
	float4 NORMAL;

	// Getter
	static InputLayoutData& GetInputLayoutData() { return ILData; }

protected:
	
private:
	static InputLayoutData ILData; // 입력된 레이아웃 데이터 관리
		
};
// [D3D11_INPUT_ELEMENT_DESC]
// DirectX11에서 입력 레이아웃(Input Layout)을 정의할 때 사용하는 구조체
// 구조체는 정점 데이터가 어떤 구조로 되어있는지를 GPU에 알려주기 위한 것
// 정점 버퍼 안의 데이터가 어떤 의미를 갖는지 설명하는 설명서 역할 수행
