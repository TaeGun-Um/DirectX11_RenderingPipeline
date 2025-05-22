#pragma once

// �ӽ�
struct Setter
{
	std::string Name;
	// class GameEngineShader* ParentShader;
	int BindPoint = -1; // b0 t0 ���� ���° ���Կ� ���õǾ�� �ϴ����� ���� ����.
	std::shared_ptr<class Ext_DirectXConstantBuffer> Res;
	const void* CPUData;
	UINT CPUDataSize;

	void Setting();
};

// DirectX�� ���̴� ������ ���
class Ext_DirectXShader
{
public:
	// constrcuter destructer
	Ext_DirectXShader() {};
	~Ext_DirectXShader() {};

	// delete Function
	Ext_DirectXShader(const Ext_DirectXShader& _Other) = delete;
	Ext_DirectXShader(Ext_DirectXShader&& _Other) noexcept = delete;
	Ext_DirectXShader& operator=(const Ext_DirectXShader& _Other) = delete;
	Ext_DirectXShader& operator=(Ext_DirectXShader&& _Other) noexcept = delete;

	static void ShaderAutoCompile(std::string_view _Path, std::string_view _EntryPoint);


	static std::multimap<std::string, Setter> ConstantBufferSetters; // �ӽ�
	COMPTR<ID3DBlob>& GetBinaryCode() { return BinaryCode; } // �ӽ�

protected:
	void CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/);
	void ShaderResourceSetting();

	COMPTR<ID3DBlob> BinaryCode = nullptr; // ID3DBlob ���� ����
	std::string Version = ""; // ���̴� ���� ����
	std::string EntryPoint = ""; // ���̴� EntryPoint �̸� ����(���ҽ� �Ŵ������� ������ �̸����� ����)
	ShaderType Type = ShaderType::Unknown; // ���̴� Ÿ�� ����

private:
	static ShaderType FindShaderType(std::string_view _EntryPoint);

};
// [ID3DBlob]
// ���̳ʸ� ������(�޸� ����, ���̴� ������ ��� ��)�� �����ϰ�, GPU�� �����ϱ� ���� ����ϴ� �������̽�
// Ư�� ���̴� ������ ����� ����� �޽��� ���� ���̳ʸ� �����͸� ���� �� Ȱ��
//