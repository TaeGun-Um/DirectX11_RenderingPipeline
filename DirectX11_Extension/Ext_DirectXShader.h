#pragma once
#include "Ext_DirectXBufferSetter.h"

// DirectX�� ���̴� ������ ���
class Ext_DirectXShader : public std::enable_shared_from_this<Ext_DirectXShader>
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

	// this ��������
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	Ext_DirectXBufferSetter& GetBufferSetter() { return BufferSetter; }
	ShaderType GetType() { return Type; }

	COMPTR<ID3DBlob>& GetBinaryCode() { return BinaryCode; } // �ӽ�

protected:
	void CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/);
	void ShaderResourceSetting(); // ��� ���� ����

	COMPTR<ID3DBlob> BinaryCode = nullptr; // ID3DBlob ���� ����
	std::string Version = ""; // ���̴� ���� ����
	std::string EntryPoint = ""; // ���̴� EntryPoint �̸� ����(���ҽ� �Ŵ������� ������ �̸����� ����)
	ShaderType Type = ShaderType::Unknown; // ���̴� Ÿ�� ����

private:
	static ShaderType FindShaderType(std::string_view _EntryPoint);
	Ext_DirectXBufferSetter BufferSetter;

};
// [ID3DBlob]
// ���̳ʸ� ������(�޸� ����, ���̴� ������ ��� ��)�� �����ϰ�, GPU�� �����ϱ� ���� ����ϴ� �������̽�
// Ư�� ���̴� ������ ����� ����� �޽��� ���� ���̳ʸ� �����͸� ���� �� Ȱ��
//