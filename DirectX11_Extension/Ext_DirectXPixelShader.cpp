#include "PrecompileHeader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXDevice.h"

Ext_DirectXPixelShader::Ext_DirectXPixelShader()
{
}

Ext_DirectXPixelShader::~Ext_DirectXPixelShader()
{
}

void Ext_DirectXPixelShader::CreatePixelShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh, UINT _VersionLow)
{
	CreateVersion("ps", _VersionHigh, _VersionLow);
	EntryPoint = _EntryPoint;

	unsigned int Flag = 0;

#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG;

#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	COMPTR<ID3DBlob> Error;
	std::wstring UniCodePath = Base_String::StringToWString(_Path.data());
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

	// 컴파일된 바이트코드로 GPU용 Pixel Shader 생성
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreatePixelShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, PixelShader.GetAddressOf()))
	{
		MsgAssert("픽셀 쉐이더 핸들 생성에 실패");
		return;
	}

	// 상수버퍼 세팅, 리소스 세팅
	ShaderResourceSetting();
}

// PSSetShader 호출로 픽셀 셰이더 세팅
void Ext_DirectXPixelShader::PixelShaderSetting()
{
	if (nullptr == PixelShader.Get())
	{
		MsgAssert("픽셀 셰이더가 존재하지 않아 세팅에 실패");
	}

	Ext_DirectXDevice::GetContext()->PSSetShader(PixelShader.Get(), nullptr, 0);
}