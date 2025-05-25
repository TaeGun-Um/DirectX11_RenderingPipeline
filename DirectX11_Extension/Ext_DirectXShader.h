#pragma once
#include "Ext_DirectXBufferSetter.h"

// DirectX의 셰이더 컴파일 및 정보 저장을 위한 클래스(상속하여 사용)
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

	// this 가져오기
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	Ext_DirectXBufferSetter& GetBufferSetter() { return BufferSetter; }
	ShaderType GetType() { return Type; }

	COMPTR<ID3DBlob>& GetBinaryCode() { return BinaryCode; } // 임시

protected:
	void CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/);
	void ShaderResourceSetting(); // 상수 버퍼 세팅

	COMPTR<ID3DBlob> BinaryCode = nullptr; // ID3DBlob 정보 저장
	std::string Version = ""; // 셰이더 버전 저장
	std::string EntryPoint = ""; // 셰이더 EntryPoint 이름 저장(리소스 매니저에도 동일한 이름으로 저장)
	ShaderType Type = ShaderType::Unknown; // 셰이더 타입 저장

private:
	static ShaderType FindShaderType(std::string_view _EntryPoint);
	Ext_DirectXBufferSetter BufferSetter;

};
// [ID3DBlob]
// 바이너리 데이터(메모리 버퍼, 셰이더 컴파일 결과 등)를 저장하고, GPU에 전달하기 위해 사용하는 인터페이스
// 특히 셰이더 컴파일 결과나 디버그 메시지 같은 바이너리 데이터를 담을 때 활용
//