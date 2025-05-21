#pragma once

// �Էµ� ���̾ƿ� ������ ����
class InputLayoutData
{
public:
	// IAData �Է�
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
	static UINT FormatSize(DXGI_FORMAT _Format); // ���� ũ�� Ȯ�ο�

	std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDescs;
	int Offset = 0;

};

// �Է� ���̾ƿ��� �����ϱ� ���� Ŭ����, 
class Ext_DirectXInputLayout
{
public:
	float4 POSITION;
	float4 COLOR;
	// float4 TEXCOORD;
	// float4 NORMAL;

	// Getter
	static InputLayoutData& GetInputLayoutData() { return ILData; }

protected:
	
private:
	static InputLayoutData ILData; // �Էµ� ���̾ƿ� ������ ����
	
};
// [D3D11_INPUT_ELEMENT_DESC]
// DirectX11���� �Է� ���̾ƿ�(Input Layout)�� ������ �� ����ϴ� ����ü
// ����ü�� ���� �����Ͱ� � ������ �Ǿ��ִ����� GPU�� �˷��ֱ� ���� ��
// ���� ���� ���� �����Ͱ� � �ǹ̸� ������ �����ϴ� ���� ���� ����
