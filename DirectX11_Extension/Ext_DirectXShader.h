#pragma once

// 임시
struct Setter
{
	std::string Name;
	// class GameEngineShader* ParentShader;
	int BindPoint = -1; // b0 t0 같은 몇번째 슬롯에 세팅되어야 하는지에 대한 정보.
	std::shared_ptr<class Ext_DirectXConstantBuffer> Res;
	const void* CPUData;
	UINT CPUDataSize;

	void Setting();
};

// DirectX의 셰이더 컴파일 담당
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


	static std::multimap<std::string, Setter> ConstantBufferSetters; // 임시
	COMPTR<ID3DBlob>& GetBinaryCode() { return BinaryCode; } // 임시

protected:
	void CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/);
	void ShaderResourceSetting();

	COMPTR<ID3DBlob> BinaryCode = nullptr; // ID3DBlob 정보 저장
	std::string Version = ""; // 셰이더 버전 저장
	std::string EntryPoint = ""; // 셰이더 EntryPoint 이름 저장(리소스 매니저에도 동일한 이름으로 저장)
	ShaderType Type = ShaderType::Unknown; // 셰이더 타입 저장

private:
	static ShaderType FindShaderType(std::string_view _EntryPoint);

};
// [ID3DBlob]
// 바이너리 데이터(메모리 버퍼, 셰이더 컴파일 결과 등)를 저장하고, GPU에 전달하기 위해 사용하는 인터페이스
// 특히 셰이더 컴파일 결과나 디버그 메시지 같은 바이너리 데이터를 담을 때 활용
//