#include "PrecompileHeader.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXDevice.h"

Ext_DirectXVertexShader::Ext_DirectXVertexShader()
{
	Type = ShaderType::Vertex;
}

void Ext_DirectXVertexShader::CreateVertexShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh, UINT _VersionLow)
{
	CreateVersion("vs", _VersionHigh, _VersionLow);
	EntryPoint = _EntryPoint;
	FilePath   = _Path;   // Hot Reload 시 재사용할 파일 경로 저장

	unsigned int Flag = 0;

#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG;
#endif

	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	COMPTR<ID3DBlob> Error;
	std::wstring UniCodePath = Base_String::AnsiToUniCode(_Path);
	if (S_OK != D3DCompileFromFile(UniCodePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), Version.c_str(), Flag, 0, BinaryCode.GetAddressOf(), Error.GetAddressOf()))
	{
		// 에러를 텍스트로 출력
		std::string ErrorString = reinterpret_cast<char*>(Error->GetBufferPointer());
		MsgAssert(ErrorString);
		return;
	}
	// <<설명>>
	/*1. 파일 경로 입력*/
	/*2. 기본 include 처리 방식 설정*/
	/*3. 함수명*/
	/*4. 셰이더 모델, 비주얼 스튜디오 2022에서 기본 생성하면 Vertex Shader Model 5.0이기 떄문에 vs_5_0*/
	/*5. 컴파일 플레그 설정*/
	/*6. 컴파일 플레그 설정*/
	/*7. 출력 바이트코드*/
	/*8. 출력 에러*/

	// 컴파일된 바이트코드로 GPU용 Vertex Shader 생성
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateVertexShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, VertexShader.GetAddressOf()))
	{
		MsgAssert("버텍스 쉐이더 핸들 생성에 실패");
		return;
	}

	// 상수버퍼 세팅, 리소스 세팅
	ShaderResourceSetting();
}

// Hot Reload — 저장된 FilePath 에서 HLSL 을 다시 읽어 재컴파일.
// - 실패 시 기존 셰이더 유지 (화면이 까맣게 되지 않음)
// - 성공 시 BinaryCode / VertexShader / BufferSetter 가 새 내용으로 갱신됨
// - 주의: VS input signature 가 바뀌면 InputLayout 이 스테일해져 드로우가 깨질 수 있음.
//         그런 경우엔 재시작 권장.
bool Ext_DirectXVertexShader::Reload()
{
	if (FilePath.empty())
	{
		OutputDebugStringA(("[HotReload] Skip VS: no FilePath - " + EntryPoint + "\n").c_str());
		return false;
	}

	unsigned int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flag |= D3D10_SHADER_DEBUG;
#endif

	// [1] 임시 Blob 에 먼저 컴파일 — 실패해도 기존 상태 보존
	COMPTR<ID3DBlob> NewBlob;
	COMPTR<ID3DBlob> Error;
	std::wstring UniCodePath = Base_String::AnsiToUniCode(FilePath);

	HRESULT Hr = D3DCompileFromFile(UniCodePath.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), Version.c_str(),
		Flag, 0, NewBlob.GetAddressOf(), Error.GetAddressOf());

	if (FAILED(Hr))
	{
		std::string Msg = "[HotReload] VS compile failed: " + EntryPoint + "\n";
		if (Error)
		{
			Msg += reinterpret_cast<const char*>(Error->GetBufferPointer());
			Msg += "\n";
		}
		OutputDebugStringA(Msg.c_str());
		return false; // 기존 상태 보존
	}

	// [2] 새 D3D 셰이더 객체 생성 — 역시 실패 시 기존 보존
	COMPTR<ID3D11VertexShader> NewVS;
	if (FAILED(Ext_DirectXDevice::GetDevice()->CreateVertexShader(
		NewBlob->GetBufferPointer(), NewBlob->GetBufferSize(),
		nullptr, NewVS.GetAddressOf())))
	{
		OutputDebugStringA(("[HotReload] VS object create failed: " + EntryPoint + "\n").c_str());
		return false;
	}

	// [3] 성공: 내부 상태 교체
	BinaryCode   = NewBlob;
	VertexShader = NewVS;

	// [4] 리플렉션 재수집 — BufferSetter 내부 맵을 비우고 다시 채움
	//     BufferSetter 는 copy/move 가 delete 되어 있어 재할당이 불가 → Clear() 로 초기화
	GetBufferSetter().Clear();
	ShaderResourceSetting();

	OutputDebugStringA(("[HotReload] VS reloaded: " + EntryPoint + "\n").c_str());
	return true;
}

// VSSetShader 호출로 정점 셰이더 세팅
void Ext_DirectXVertexShader::VertexShaderSetting()
{
	if (nullptr == VertexShader.Get())
	{
		MsgAssert("정점 셰이더가 존재하지 않아 세팅에 실패");
	}

	Ext_DirectXDevice::GetContext()->VSSetShader(VertexShader.Get(), nullptr, 0);
	// 1. 바인디할 셰이더 객체
	// 2. 셰이더에 사용할 클래스 배열 전달(보통 nullptr)
	// 3. 클래스 개수(보통 0)
}