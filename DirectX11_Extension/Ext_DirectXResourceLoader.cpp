#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXTexture.h"
#include "Ext_DirectXVertexData.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXMaterial.h"
#include "Ext_DirectXSampler.h"
#include "Ext_DirectXBlend.h"
#include "Ext_DirectXRasterizer.h"
#include "Ext_DirectXDepth.h"
#include "Ext_DirectXRenderTarget.h"

// DirectX에 필요한 리소스를 로드
void Ext_DirectXResourceLoader::Initialize()
{
	LoadTexture();
	MakeVertex(); // Ext_DirectXVertexData 클래스의 InputLayoutElement에 SemanticName, Format 결정
	MakeSampler();
	MakeBlend();
	MakeDepth();
	MakeRasterizer();
	ShaderCompile(); // 셰이더 컴파일 후 상수버퍼 생성
	MakeMaterial(); // 머티리얼(파이프라인) 생성
}

void Ext_DirectXResourceLoader::LoadTexture()
{
	// 기본 텍스쳐들 로드
	{
		Base_Directory Dir;
		Dir.MakePath("../Resource/Basic");
		std::vector<std::string> Paths = Dir.GetAllFile({ "png", "tga", "dss" });
		for (const std::string& FilePath : Paths)
		{
			Dir.SetPath(FilePath.c_str());
			std::string ExtensionName = Dir.GetExtension();
			std::string FileName = Dir.GetFileName();
			Ext_DirectXTexture::LoadTexture(FilePath.c_str());
		}
	}
}

// 정점 정보 생성(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex()
{
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("TANGENT", DXGI_FORMAT_R32G32B32A32_FLOAT); // Normal Mapping용
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("BINORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT); // Normal Mapping용
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("BONEID", DXGI_FORMAT_R32G32B32A32_SINT); // FBX Animation용
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("WEIGHT", DXGI_FORMAT_R32G32B32A32_FLOAT); // FBX Animation용

	// 너무 길어져서 밑으로 뺌
	CreateTriangle(); // 삼각형 메시 만들기
	CreateRect(); // 사각형 메시 만들기
	CreateCube(); // 정육면체 만들기
	CreateSphere(); // 구체 만들기
	CreateCylinder(); // 원기둥 만들기
}

// 셰이더 정보 생성(Shader 오토컴파일, ConstantBuffer)
void Ext_DirectXResourceLoader::ShaderCompile()
{
	// 셰이더 생성 규칙 : [1], [2]를 모두 만족해야 정상 컴파일 진행
	// [1] 이름 + "_" + "Type" == Basic_PS
	// [2] 내부 main(EntryPoint) 이름도 동일하게 설정
	Base_Directory Dir;
	Dir.MakePath("../Shader");
	std::vector<std::string> Paths = Dir.GetAllFile({ "hlsl" });
	for (const std::string& ShaderPath : Paths)
	{
		std::string EntryPoint = Dir.FindEntryPoint(ShaderPath);
		Ext_DirectXShader::ShaderAutoCompile(ShaderPath, EntryPoint.c_str());
	}
}

// 샘플러 정보 생성
void Ext_DirectXResourceLoader::MakeSampler()
{
	// 리니어 클램프 샘플러, 
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;
		// <<설명>>
		/*1. Filter : MIN_MAG_MIP_LINEAR(축소, 확대, 밉맵) 모두 선형 보간으로 부드럽게 처리한다는 뜻의 D3D11_FILTER_MIN_MAG_MIP_LINEAR 전달*/
		/*2~4. AddressUVW : D3D11_TEXTURE_ADDRESS_CLAMP는 UV가 0~1 범위를 넘을 경우, 가장자리 색상을 계속 사용한다는 뜻*/
		/*5. MipLODBias : 밉맵 LOD를 얼마나 조정할지 정하기 위함, 0.0f로 전달하면 기본 수준 사용 */
		/*6. MaxAnisotropy : 애니소트로픽 필터링 여부, 1은 비활성화이며 4 ~ 16을 보통 사용한다.*/
		/*7. ComparisonFunc : 그림자 맵 등에서 비교 연산 시 항상 통과한다는 설정, 일반 텍스쳐는 보통 ALWAYS를 설정한다.*/
		/*8~9. MinLOD, MaxLOD : 밉맵 레벨 범위 제한, -FLT_MAX ~ FLT_MAX면 전체를 허용하는 것*/

		Ext_DirectXSampler::CreateSampler("LinearClampSampler", SamperInfo);
	}

	// 리니어 미러 샘플러
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("LinearMirrorSampler", SamperInfo);
	}

	// 리니어 랩 샘플러
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("LinearWrapSampler", SamperInfo);
	}

	// 포인트 클램프 샘플러
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("PointClampSampler", SamperInfo);
	}

	// 포인트 미러 샘플러
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("PointMirrorSampler", SamperInfo);
	}

	// 포인트 랩 샘플러
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("PointWrapSampler", SamperInfo);
	}

	// 이퀄
	{
		D3D11_SAMPLER_DESC SamperInfo = {};

		SamperInfo.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamperInfo.MipLODBias = 0.0f;
		SamperInfo.MaxAnisotropy = 1;
		SamperInfo.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		SamperInfo.MinLOD = -FLT_MAX;
		SamperInfo.MaxLOD = FLT_MAX;

		Ext_DirectXSampler::CreateSampler("ShadowCmpSampler", SamperInfo);
	}
}

// DirectX 블렌드 생성
void Ext_DirectXResourceLoader::MakeBlend()
{
	// Alpha 블렌드(가장 기본 블렌드)
	{
		D3D11_BLEND_DESC BlendInfo = { 0, };

		// Desc.AlphaToCoverageEnable = false; == 자동으로 알파부분을 제거하여 출력해준다. 너무 느려서 사용 안함.
		BlendInfo.AlphaToCoverageEnable = false;
		BlendInfo.IndependentBlendEnable = false;
		BlendInfo.RenderTarget[0].BlendEnable = true;

		BlendInfo.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 자주 쓰는 조합 1
		BlendInfo.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendInfo.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		BlendInfo.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		BlendInfo.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		// <<설명>>
		/*1. BlendEnable : 블렌드 활성화 여부, true는 활성화*/
		/*2. SrcBlend : 소스 색상에 곱할 계수, D3D11_BLEND_SRC_ALPHA면 RGBA에서 A*/
		/*3. DestBlend : 대상 색상에 곱할 계수*/
		/*4. BlendOp : 두 색상의 혼합 방식, ADD, SUBTRACT 등 있음*/
		/*5. SrcBlendAlpha : 알파 채널에 대한 Scr 계수*/
		/*6. DestBlendAlpha : 알파 채널에 대한 Scr 계수*/
		/*7. BlendOpAlpha : 알파 채널의 혼합 연산*/
		/*8. RenderTargetWriteMask : RGBA 쓰기 허용 여부 결정*/

		// 자주쓰는 조합들
		// 1. 알파블랜딩 : 일반적인 반투명 처리에 활용된다.
		// SrcBlend = D3D11_BLEND_SRC_ALPHA, DestBlend = D3D11_BLEND_INV_SRC_ALPHA, BlendOp = D3D11_BLEND_OP_ADD
		// 공식 : Final = SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)이다. 
		// 2. 가산 합성 : 광원, 폭발, 글로우 등에 사용된다.
		// SrcBlend = D3D11_BLEND_ONE, DestBlend = D3D11_BLEND_ONE, BlendOp = D3D11_BLEND_OP_ADD
		// 공식 : Final = SrcColor + DestColor
		// 3. 서브트랙티브 : 어둡게 만들거나 특정 색을 빼는 연출에 사용된다.
		// SrcBlend = D3D11_BLEND_ONE, DestBlend = D3D11_BLEND_ONE, BlendOp = D3D11_BLEND_OP_SUBTRACT
		// 공식 : Final = SrcColor - DestColor

		Ext_DirectXBlend::CreateBlend("BaseBlend", BlendInfo);
	}

	// Min 블렌드(그림자에 쓰는)
	{
		D3D11_BLEND_DESC BlendInfo = { 0, };

		BlendInfo.AlphaToCoverageEnable = false;
		BlendInfo.IndependentBlendEnable = false;

		BlendInfo.RenderTarget[0].BlendEnable = true;
		BlendInfo.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

		BlendInfo.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;

		BlendInfo.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		Ext_DirectXBlend::CreateBlend("MinBlend", BlendInfo);
	}

	// Merge용 블렌드
	{
		D3D11_BLEND_DESC BlendInfo = { 0, };

		BlendInfo.AlphaToCoverageEnable = false;
		BlendInfo.IndependentBlendEnable = false;
		BlendInfo.RenderTarget[0].BlendEnable = true;

		BlendInfo.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendInfo.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		BlendInfo.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;

		BlendInfo.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		Ext_DirectXBlend::CreateBlend("MergeBlend", BlendInfo);
	}

	// One 블렌드, Deffered용
	{
		D3D11_BLEND_DESC BlendInfo = { 0, };

		BlendInfo.AlphaToCoverageEnable = false;
		BlendInfo.IndependentBlendEnable = false;
		BlendInfo.RenderTarget[0].BlendEnable = true;

		BlendInfo.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		BlendInfo.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		BlendInfo.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		BlendInfo.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		Ext_DirectXBlend::CreateBlend("OneBlend", BlendInfo);
	}
}

// DirectX11 DepthStencilState 생성
void Ext_DirectXResourceLoader::MakeDepth()
{
	// 깊이 테스트만 기본으로 진행
	{
		D3D11_DEPTH_STENCIL_DESC DepthStencilInfo = { 0, };

		DepthStencilInfo.DepthEnable = true;
		DepthStencilInfo.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilInfo.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		DepthStencilInfo.StencilEnable = false;
		/*1. 깊이 테스트 수행함(true)*/
		/*2. 깊이 값을 Z버퍼에 기록*/
		/*3. 새 픽셀이 더 가깝거나 같으면 통과*/
		/*4. 스텐실 안함*/

		Ext_DirectXDepth::CreateDepthStencilState("EngineDepth", DepthStencilInfo);
	}

	// 깊이 테스트 안하기
	{
		D3D11_DEPTH_STENCIL_DESC DepthStencilInfo = { 0, };

		DepthStencilInfo.DepthEnable = true;
		DepthStencilInfo.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilInfo.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		DepthStencilInfo.StencilEnable = false;

		Ext_DirectXDepth::CreateDepthStencilState("AlwayDepth", DepthStencilInfo);
	}

	// 스텐실 사용
	{
		D3D11_DEPTH_STENCIL_DESC DepthStencilInfo = { 0, };

		DepthStencilInfo.DepthEnable = true;
		DepthStencilInfo.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilInfo.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		DepthStencilInfo.StencilEnable = TRUE;
		DepthStencilInfo.StencilWriteMask = 0xFF;

		// 앞면/뒷면 다 스텐실 값 1 써 주기
		DepthStencilInfo.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		DepthStencilInfo.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		DepthStencilInfo.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilInfo.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilInfo.BackFace = DepthStencilInfo.FrontFace;

		Ext_DirectXDepth::CreateDepthStencilState("StencilDepth", DepthStencilInfo);
	}

	// 스텐실 저격
	{
		D3D11_DEPTH_STENCIL_DESC DepthStencilInfo = { 0, };

		DepthStencilInfo.DepthEnable = false;      // 깊이는 이미 다 찍혔으니 끔
		DepthStencilInfo.StencilEnable = TRUE;
		DepthStencilInfo.StencilReadMask = 0xFF;
		DepthStencilInfo.StencilWriteMask = 0x00;     // 포스트엔 쓰지 않음

		// ref 와 동일한 곳만 통과
		DepthStencilInfo.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		DepthStencilInfo.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilInfo.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilInfo.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		DepthStencilInfo.BackFace = DepthStencilInfo.FrontFace;

		Ext_DirectXDepth::CreateDepthStencilState("StencilTest", DepthStencilInfo);
	}
}

// DirectX11 Rasterizer 생성
void Ext_DirectXResourceLoader::MakeRasterizer()
{
	// 일반 백페이스 컬링용 레스터라이저
	{
		D3D11_RASTERIZER_DESC Desc = {};

		Desc.CullMode = D3D11_CULL_BACK; // 뒷면 제거(백페이스 컬링 활성화)
		Desc.FrontCounterClockwise = TRUE; // 반시계방향이 앞면(앞면 기준을 CCW로 지정)
		Desc.FillMode = D3D11_FILL_SOLID;
		// Desc.DepthClipEnable = FALSE;          // Z-Clipping 안함

		Ext_DirectXRasterizer::CreateRasterizer("BasicRasterizer", Desc);
	}

	// None컬링 레스터라이저
	{
		D3D11_RASTERIZER_DESC Desc = {};

		Desc.CullMode = D3D11_CULL_NONE;
		Desc.FrontCounterClockwise = FALSE;
		Desc.FillMode = D3D11_FILL_SOLID;

		std::shared_ptr<Ext_DirectXRasterizer> NewRast = Ext_DirectXRasterizer::CreateRasterizer("NonCullingRasterizer", Desc);
	}

	// 디버깅용 와이어프레임 레스터라이저
	{
		D3D11_RASTERIZER_DESC Desc = {};

		Desc.CullMode = D3D11_CULL_NONE;
		Desc.FrontCounterClockwise = FALSE;
		Desc.FillMode = D3D11_FILL_SOLID;

		std::shared_ptr<Ext_DirectXRasterizer> NewRast = Ext_DirectXRasterizer::CreateRasterizer("DebugRasterizer", Desc);
		NewRast->SetFILL_MODE(D3D11_FILL_WIREFRAME);
	}
}

// CurState = WireframeState;

// 렌더링 파이프라인 생성
void Ext_DirectXResourceLoader::MakeMaterial()
{
	// 그래픽스 스태틱 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Static");
		NewRenderingPipeline->SetVertexShader("Static_VS");
		NewRenderingPipeline->SetPixelShader("Graphics_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽스 다이나믹 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Dynamic");
		NewRenderingPipeline->SetVertexShader("Dynamic_VS");
		NewRenderingPipeline->SetPixelShader("Graphics_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽스 스태틱 메시 + 텍스쳐들 추가 버젼
	{
		//std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("PBRStatic");
		//NewRenderingPipeline->SetVertexShader("PBR_VS");
		//NewRenderingPipeline->SetPixelShader("PBR_PS");
		//NewRenderingPipeline->SetBlendState("BaseBlend");
		//NewRenderingPipeline->SetDepthState("EngineDepth");
		//NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽스 다이나믹 메시 + 텍스쳐들 추가 버젼
	{
		//std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("PBRDynamic");
		//NewRenderingPipeline->SetVertexShader("DynamicPBR_VS");
		//NewRenderingPipeline->SetPixelShader("PBR_PS");
		//NewRenderingPipeline->SetBlendState("BaseBlend");
		//NewRenderingPipeline->SetDepthState("EngineDepth");
		//NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// Deffered
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("DeferredLight");
		NewRenderingPipeline->SetVertexShader("DeferredLight_VS");
		NewRenderingPipeline->SetPixelShader("DeferredLight_PS");
		NewRenderingPipeline->SetBlendState("OneBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// PBR Deffered
	{
		//std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("PBRDeferredLight");
		//NewRenderingPipeline->SetVertexShader("PBRDeferredLight_VS");
		//NewRenderingPipeline->SetPixelShader("PBRDeferredLight_PS");
		//NewRenderingPipeline->SetBlendState("OneBlend");
		//NewRenderingPipeline->SetDepthState("AlwayDepth");
		//NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// Deffered된 결과물 Merge용
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("DeferredMerge");
		NewRenderingPipeline->SetVertexShader("DeferredMerge_VS");
		NewRenderingPipeline->SetPixelShader("DeferredMerge_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// 그림자
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Shadow");
		NewRenderingPipeline->SetVertexShader("Shadow_VS");
		NewRenderingPipeline->SetPixelShader("Shadow_PS");
		NewRenderingPipeline->SetBlendState("MinBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// 다이나믹 그림자
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("DynamicShadow");
		NewRenderingPipeline->SetVertexShader("DynamicShadow_VS");
		NewRenderingPipeline->SetPixelShader("Shadow_PS");
		NewRenderingPipeline->SetBlendState("MinBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// 렌더타겟 간 Merge를 위한 머티리얼
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("RenderTargetMerge");

		NewRenderingPipeline->SetVertexShader("RenderTargetMerge_VS");
		NewRenderingPipeline->SetPixelShader("RenderTargetMerge_PS");
		NewRenderingPipeline->SetBlendState("MergeBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// 와이어프레임(디버깅용)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Debug");
		NewRenderingPipeline->SetVertexShader("NonGStatic_VS");
		NewRenderingPipeline->SetPixelShader("Debug_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("DebugRasterizer");
	}

	// 그래픽 처리X 스태틱 메시(이제 안씀)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("NonGStatic");
		NewRenderingPipeline->SetVertexShader("NonGStatic_VS");
		NewRenderingPipeline->SetPixelShader("NonG_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽 처리X 다이나믹 메시(이제 안씀)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("NonGDynamic");
		NewRenderingPipeline->SetVertexShader("NonGDynamic_VS");
		NewRenderingPipeline->SetPixelShader("NonG_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// PostProcess - Blur
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Blur");

		NewRenderingPipeline->SetVertexShader("Blur_VS");
		NewRenderingPipeline->SetPixelShader("Blur_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// PostProcess - Distortion
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Distortion");

		NewRenderingPipeline->SetVertexShader("Distortion_VS");
		NewRenderingPipeline->SetPixelShader("Distortion_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// PostProcess - OldFilm
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("OldFilm");

		NewRenderingPipeline->SetVertexShader("OldFilm_VS");
		NewRenderingPipeline->SetPixelShader("OldFilm_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("AlwayDepth");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
	}

	// 스텐실(테스트용)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("StaticStencil");

		NewRenderingPipeline->SetVertexShader("Static_VS");
		NewRenderingPipeline->SetPixelShader("Graphics_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("StencilDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
		NewRenderingPipeline->SetStencilTest();
	}

	// 스텐실 디스토션
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("DistortionStencil");

		NewRenderingPipeline->SetVertexShader("Distortion_VS");
		NewRenderingPipeline->SetPixelShader("Distortion_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("StencilTest");
		NewRenderingPipeline->SetRasterizer("NonCullingRasterizer");
		NewRenderingPipeline->SetStencilTest();
	}

	// RenderTarget Merge를 위해 MergeUnit에 값 넣어주기 위해 호출
	{
		Ext_DirectXRenderTarget::RenderTargetMergeUnitInitialize();
	}
}

void Ext_DirectXResourceLoader::CreateTriangle()
{
	std::vector<Ext_DirectXVertexData> Vertices;
	Vertices.resize(3);

	Vertices[0] = { { 0.0f, 0.5f, 0.0 }, { 1, 0, 0, 1 } };
	Vertices[1] = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
	Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };

	std::vector<UINT> ArrIndex = { 0, 1, 2 };

	Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", Vertices);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Triangle", ArrIndex);
	Ext_DirectXMesh::CreateMesh("Triangle");
}

void Ext_DirectXResourceLoader::CreateRect()
{
	// 일반 Rect
	{
		std::vector<Ext_DirectXVertexData> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { {  0.5f,  0.5f,  0.0f, 1.0f },  {1, 0, 0, 1},  {1, 0}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[1] = { { -0.5f,  0.5f,  0.0f, 1.0f },  {0, 1, 0, 1},  {0, 0}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[2] = { { -0.5f, -0.5f,  0.0f, 1.0f },  {0, 0, 1, 1},  {0, 1}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[3] = { {  0.5f, -0.5f,  0.0f, 1.0f },  {1, 1, 0, 1},  {1, 1}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };

		std::vector<UINT> ArrIndex = { 0, 1, 2, 0, 2, 3, };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Rect", ArrIndex);
		Ext_DirectXMesh::CreateMesh("Rect");
	}

	// 테스트용 미러렉트
	{
		std::vector<Ext_DirectXVertexData> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { {  0.5f,  0.5f,  0.0f, 1.0f }, {1, 0, 0, 1}, {4.f, -4.f}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[1] = { { -0.5f,  0.5f,  0.0f, 1.0f }, {0, 1, 0, 1}, {-4.f, -4.f}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[2] = { { -0.5f, -0.5f,  0.0f, 1.0f }, {0, 0, 1, 1}, {-4.f, -4.f}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };
		ArrVertex[3] = { {  0.5f, -0.5f,  0.0f, 1.0f }, {1, 1, 0, 1}, {4.f, 4.f}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0} };

		std::vector<UINT> ArrIndex = { 0, 1, 2, 0, 2, 3, };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("MirrorRect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("MirrorRect", ArrIndex);
		Ext_DirectXMesh::CreateMesh("MirrorRect");
	}

	// Merge용 FullRect
	{
		std::vector<Ext_DirectXVertexData> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { { 1.0f, 1.0f, 0.0f, 1.0f }, {1, 0, 0, 1},  {1, 0} };
		ArrVertex[1] = { { -1.0f, 1.0f, 0.0f, 1.0f }, {0, 1, 0, 1}, {0, 0} };
		ArrVertex[2] = { { -1.0f, -1.0f, 0.0f, 1.0f }, {0, 0, 1, 1}, {0, 1} };
		ArrVertex[3] = { { 1.0f, -1.0f, 0.0f, 1.0f }, {1, 1, 0, 1},  {1, 1} };

		std::vector<UINT> ArrIndex = { 0, 1, 2, 0, 2, 3 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("FullRect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("FullRect", ArrIndex);
		Ext_DirectXMesh::CreateMesh("FullRect");
	}
}

void Ext_DirectXResourceLoader::CreateCube()
{
	std::vector<Ext_DirectXVertexData> Vertex =
	{
		// Front (+Z) 면: Normal = (0, 0, 1), Tex U→ +X, Tex V→ −Y
		{ {-0.5f,  0.5f,  0.5f, 1.0f},  {1,0,0,1}, {1,0}, {0,0,1},  {1,0,0},  {0,-1,0} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},  {0,1,0,1}, {0,0}, {0,0,1},  {1,0,0},  {0,-1,0} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},  {0,0,1,1}, {0,1}, {0,0,1},  {1,0,0},  {0,-1,0} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},  {1,1,0,1}, {1,1}, {0,0,1},  {1,0,0},  {0,-1,0} },

		// Back (−Z) 면: Normal = (0, 0, −1), Tex U→ −X, Tex V→ −Y
		{ { 0.5f,  0.5f, -0.5f, 1.0f},  {1,0,0,1}, {1,0}, {0,0,-1}, {-1,0,0}, {0,-1,0} },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},  {0,1,0,1}, {0,0}, {0,0,-1}, {-1,0,0}, {0,-1,0} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},  {0,0,1,1}, {0,1}, {0,0,-1}, {-1,0,0}, {0,-1,0} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},  {1,1,0,1}, {1,1}, {0,0,-1}, {-1,0,0}, {0,-1,0} },

		// Left (−X) 면: Normal = (−1, 0, 0), Tex U→ −Z, Tex V→ −Y
		{ {-0.5f,  0.5f, -0.5f, 1.0f},  {1,0,0,1}, {1,0}, {-1,0,0}, {0,0,-1}, {0,-1,0} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},  {0,1,0,1}, {0,0}, {-1,0,0}, {0,0,-1}, {0,-1,0} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},  {0,0,1,1}, {0,1}, {-1,0,0}, {0,0,-1}, {0,-1,0} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},  {1,1,0,1}, {1,1}, {-1,0,0}, {0,0,-1}, {0,-1,0} },

		// Right (+X) 면: Normal = (1, 0, 0), Tex U→ +Z, Tex V→ −Y
		{ { 0.5f,  0.5f,  0.5f, 1.0f},  {1,0,0,1}, {1,0}, {1,0,0},  {0,0,1},  {0,-1,0} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},  {0,1,0,1}, {0,0}, {1,0,0},  {0,0,1},  {0,-1,0} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},  {0,0,1,1}, {0,1}, {1,0,0},  {0,0,1},  {0,-1,0} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},  {1,1,0,1}, {1,1}, {1,0,0},  {0,0,1},  {0,-1,0} },

		// Top (+Y) 면: Normal = (0, 1, 0), Tex U→ +X, Tex V→ +Z
		{ {-0.5f,  0.5f, -0.5f, 1.0f},  {1,0,0,1}, {1,0}, {0,1,0},  {1,0,0},  {0,0,1} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},  {0,1,0,1}, {0,0}, {0,1,0},  {1,0,0},  {0,0,1} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},  {0,0,1,1}, {0,1}, {0,1,0},  {1,0,0},  {0,0,1} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},  {1,1,0,1}, {1,1}, {0,1,0},  {1,0,0},  {0,0,1} },

		// Bottom (−Y) 면: Normal = (0, −1, 0), Tex U→ +X, Tex V→ −Z
		{ {-0.5f, -0.5f,  0.5f, 1.0f},  {1,0,0,1}, {1,0}, {0,-1,0}, {1,0,0},  {0,0,-1} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},  {0,1,0,1}, {0,0}, {0,-1,0}, {1,0,0},  {0,0,-1} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},  {0,0,1,1}, {0,1}, {0,-1,0}, {1,0,0},  {0,0,-1} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},  {1,1,0,1}, {1,1}, {0,-1,0}, {1,0,0},  {0,0,-1} },
	};

	std::vector<UINT> Index =
	{
		// Front (+Z)
		0, 1, 2,    0, 2, 3,

		// Back (-Z)
		4, 5, 6,    4, 6, 7,

		// Left (-X)
		8, 9, 10,   8, 10, 11,

		// Right (+X)
		12, 13, 14, 12, 14, 15,

		// Top (+Y)
		16, 17, 18, 16, 18, 19,

		// Bottom (-Y)
		20, 21, 22, 20, 22, 23
	};

	Ext_DirectXVertexBuffer::CreateVertexBuffer("Box", Vertex);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Box", Index);
	std::shared_ptr<Ext_DirectXMesh> Mesh = Ext_DirectXMesh::CreateMesh("Box");
}

void Ext_DirectXResourceLoader::CreateSphere()
{
	const int SliceCount = 20;
	const int StackCount = 20;
	const float Radius = 0.5f;
	const float PI = 3.1415926535f;

	// 정점 생성
	std::vector<Ext_DirectXVertexData> Vertices;
	Vertices.reserve((StackCount + 1) * (SliceCount + 1));

	for (int i = 0; i <= StackCount; ++i)
	{
		float Phi = static_cast<float>(i) * PI / static_cast<float>(StackCount); // 위도
		float cosPhi = cosf(Phi);
		float sinPhi = sinf(Phi);

		for (int j = 0; j <= SliceCount; ++j)
		{
			float Theta = static_cast<float>(j) * 2.0f * PI / static_cast<float>(SliceCount); // 경도
			float cosTheta = cosf(Theta);
			float sinTheta = sinf(Theta);

			// 위치
			float x = Radius * sinPhi * cosTheta;
			float y = Radius * cosPhi;
			float z = Radius * sinPhi * sinTheta;

			// 텍스처 좌표
			float u = Theta / (2.0f * PI);
			float v = Phi / PI;

			// 법선 (θ, φ에서 이미 단위 벡터)
			float nx = sinPhi * cosTheta;
			float ny = cosPhi;
			float nz = sinPhi * sinTheta;

			// 접선: ∂P/∂θ 방향 (경도 방향)
			float tx = -sinPhi * sinTheta;  // ∂x/∂θ = -R sinφ sinθ, normalize 하면 R이 없어짐
			float ty = 0.0f;               // ∂y/∂θ = 0
			float tz = sinPhi * cosTheta;  // ∂z/∂θ =  R sinφ cosθ
			// 정규화
			float lenT = sqrtf(tx * tx + ty * ty + tz * tz);
			if (lenT > 0.0001f) { tx /= lenT; ty /= lenT; tz /= lenT; }
			else { tx = 1; ty = 0; tz = 0; } // 위·아래 꼭짓점 근처 예외 처리

			// 이접선: Normal × Tangent
			float bx = ny * tz - nz * ty;
			float by = nz * tx - nx * tz;
			float bz = nx * ty - ny * tx;
			// 정규화
			float lenB = sqrtf(bx * bx + by * by + bz * bz);
			if (lenB > 0.0001f) { bx /= lenB; by /= lenB; bz /= lenB; }
			else { bx = 0; by = 1; bz = 0; }

			// 컬러는 임의로 흰색
			float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

			Vertices.push_back(
				{
					{ x, y, z, 1.0f },      // POSITION
					{ r, g, b, a },         // COLOR
					{ u, v, 0.0f, 0.0f },    // TEXCOORD (float4)
					{ nx, ny, nz, 0.0f },   // NORMAL (float4, w=0)
					{ tx, ty, tz, 0.0f },   // TANGENT (float4, w=0)
					{ bx, by, bz, 0.0f }    // BINORMAL (float4, w=0)
				});
		}
	}

	// 인덱스 생성
	std::vector<UINT> Indices;
	for (int i = 0; i < StackCount; ++i)
	{
		for (int j = 0; j < SliceCount; ++j)
		{
			int First = i * (SliceCount + 1) + j;
			int Second = (i + 1) * (SliceCount + 1) + j;

			// 삼각형 1
			Indices.push_back(static_cast<UINT>(First));
			Indices.push_back(static_cast<UINT>(Second));
			Indices.push_back(static_cast<UINT>(First + 1));
			// 삼각형 2
			Indices.push_back(static_cast<UINT>(Second));
			Indices.push_back(static_cast<UINT>(Second + 1));
			Indices.push_back(static_cast<UINT>(First + 1));
		}
	}

	// 3) 버퍼 생성 및 메시 등록
	Ext_DirectXVertexBuffer::CreateVertexBuffer("Sphere", Vertices);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Sphere", Indices);
	Ext_DirectXMesh::CreateMesh("Sphere");
}

void Ext_DirectXResourceLoader::CreateCylinder()
{
	const int SliceCount = 20;
	const float Radius = 0.5f;
	const float Height = 1.0f;
	const float HalfHeight = Height * 0.5f;
	const float PI = 3.1415926535f;

	std::vector<Ext_DirectXVertexData> Vertices;

	// -- Top Center (정점 0)
	Vertices.push_back(
		{
			{  0.0f,  HalfHeight,  0.0f, 1.0f },  // POSITION
			{  1.0f, 1.0f,  1.0f, 1.0f },         // COLOR
			{  0.5f,  0.5f, 0.0f, 0.0f },         // TEXCOORD
			{  0.0f,  1.0f, 0.0f, 0.0f },         // NORMAL (위쪽)
			{  1.0f,  0.0f, 0.0f, 0.0f },         // TANGENT (U축: +X 방향)
			{  0.0f,  0.0f, 1.0f, 0.0f }          // BINORMAL (V축: +Z 방향)
		});

	// -- Top Circle Edge (정점 1 ~ SliceCount+1)
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float u = (x / Radius + 1.0f) * 0.5f;
		float v = (z / Radius + 1.0f) * 0.5f;

		// 법선 (위쪽) = (0, +1, 0)
		// 접선: U축이 +X 방향, 이접선: V축이 +Z 방향 (둘 다 월드 축과 일치)
		Vertices.push_back(
			{
				{ x,  HalfHeight, z, 1.0f },       // POSITION
				{ 1.0f, 1.0f, 1.0f, 1.0f },         // COLOR
				{ u, v, 0.0f, 0.0f },               // TEXCOORD
				{ 0.0f, 1.0f, 0.0f, 0.0f },         // NORMAL
				{ 1.0f, 0.0f, 0.0f, 0.0f },         // TANGENT
				{ 0.0f, 0.0f, 1.0f, 0.0f }          // BINORMAL
			});
	}

	// -- Bottom Center
	Vertices.push_back(
		{
			{  0.0f, -HalfHeight, 0.0f, 1.0f },  // POSITION
			{  1.0f, 1.0f,  1.0f, 1.0f },         // COLOR
			{  0.5f,  0.5f, 0.0f, 0.0f },         // TEXCOORD
			{  0.0f, -1.0f, 0.0f, 0.0f },         // NORMAL (아래쪽)
			{  1.0f,  0.0f, 0.0f, 0.0f },         // TANGENT (+X)
			{  0.0f,  0.0f, -1.0f,0.0f }          // BINORMAL (−Z)
		});

	// -- Bottom Circle Edge
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float u = (x / Radius + 1.0f) * 0.5f;
		float v = (z / Radius + 1.0f) * 0.5f;

		Vertices.push_back(
			{
				{ x, -HalfHeight, z, 1.0f },       // POSITION
				{ 1.0f, 1.0f,  1.0f, 1.0f },        // COLOR
				{ u, v, 0.0f, 0.0f },               // TEXCOORD
				{ 0.0f, -1.0f, 0.0f, 0.0f },        // NORMAL
				{ 1.0f,  0.0f, 0.0f, 0.0f },        // TANGENT
				{ 0.0f,  0.0f, -1.0f, 0.0f }        // BINORMAL
			});
	}

	// -- Side: Bottom Ring (정점: OffsetSideBotStart ... OffsetSideBotStart+SliceCount)
	int OffsetSideBotStart = static_cast<int>(Vertices.size());
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float nx = x / Radius;
		float nz = z / Radius;
		float u = static_cast<float>(i) / static_cast<float>(SliceCount);
		float v = 1.0f; // 아래쪽 테두리

		// 법선: (nx, 0, nz)
		// 접선: ∂P/∂θ 방향 = (−sinθ, 0, cosθ)
		float tx = -sinf(Theta);
		float ty = 0.0f;
		float tz = cosf(Theta);
		// 이접선: Normal × Tangent = (nx,0,nz) × (tx,0,tz) = (0, - (nx*tz - nz*tx), 0)
		float bx = 0.0f;
		float by = -(nx * tz - nz * tx);
		float bz = 0.0f;
		// 여기서는 by가 음수(= -1)로 고정되므로, 단위 길이 (0, -1, 0)
		// 사실 (nx,0,nz)×(tx,0,tz) = (0, −(nx*tz − nz*tx), 0) = (0, −(nx*cosθ − nz*(−sinθ)), 0)
		// = (0, −(nx*cosθ + nz*sinθ), 0) = (0, −( (x/R)*cosθ + (z/R)*sinθ ), 0) = (0, −1, 0).
		bx = 0.0f; by = -1.0f; bz = 0.0f;

		Vertices.push_back(
			{
				{ x, -HalfHeight, z, 1.0f },    // POSITION
				{ 1.0f, 1.0f, 1.0f, 1.0f },      // COLOR
				{ u, v, 0.0f, 0.0f },            // TEXCOORD
				{ nx, 0.0f, nz, 0.0f },          // NORMAL
				{ tx, ty, tz, 0.0f },            // TANGENT
				{ bx, by, bz, 0.0f }             // BINORMAL
			});
	}

	// -- Side: Top Ring (정점: OffsetSideTopStart ... OffsetSideTopStart+SliceCount)
	int OffsetSideTopStart = static_cast<int>(Vertices.size());
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float nx = x / Radius;
		float nz = z / Radius;
		float u = static_cast<float>(i) / static_cast<float>(SliceCount);
		float v = 0.0f; // 위쪽 테두리

		// 법선: (nx, 0, nz)
		// 접선: (−sinθ, 0, cosθ)
		float tx = -sinf(Theta);
		float ty = 0.0f;
		float tz = cosf(Theta);
		// 이접선: (0, +1, 0)
		float bx = 0.0f, by = 1.0f, bz = 0.0f;

		Vertices.push_back(
			{
				{ x,  HalfHeight, z, 1.0f },    // POSITION
				{ 1.0f, 1.0f, 1.0f, 1.0f },      // COLOR
				{ u, v, 0.0f, 0.0f },            // TEXCOORD
				{ nx, 0.0f, nz, 0.0f },          // NORMAL
				{ tx, ty, tz, 0.0f },            // TANGENT
				{ bx, by, bz, 0.0f }             // BINORMAL
			});
	}

	// 인덱스 생성
	std::vector<UINT> Indices;

	const int OffsetTopCenter = 0;
	const int OffsetTopRingStart = OffsetTopCenter + 1;
	const int OffsetBottomCenter = OffsetTopRingStart + (SliceCount + 1);
	const int OffsetBottomRingStart = OffsetBottomCenter + 1;
	// 사이드 오프셋은 위에서 계산된 값 사용
	// Side Bottom: OffsetSideBotStart ... 
	// Side Top:    OffsetSideTopStart ...

	// -- Top Cap
	for (int i = 0; i < SliceCount; ++i)
	{
		Indices.push_back(static_cast<UINT>(OffsetTopCenter));
		Indices.push_back(static_cast<UINT>(OffsetTopRingStart + i));
		Indices.push_back(static_cast<UINT>(OffsetTopRingStart + i + 1));
	}
	// -- Bottom Cap
	for (int i = 0; i < SliceCount; ++i)
	{
		Indices.push_back(static_cast<UINT>(OffsetBottomCenter));
		Indices.push_back(static_cast<UINT>(OffsetBottomRingStart + i + 1));
		Indices.push_back(static_cast<UINT>(OffsetBottomRingStart + i));
	}
	// -- Side Surface
	for (int i = 0; i < SliceCount; ++i)
	{
		int CurrBot = OffsetSideBotStart + i;
		int NextBot = OffsetSideBotStart + i + 1;
		int CurrTop = OffsetSideTopStart + i;
		int NextTop = OffsetSideTopStart + i + 1;

		// 삼각형 1
		Indices.push_back(static_cast<UINT>(CurrBot));
		Indices.push_back(static_cast<UINT>(NextBot));
		Indices.push_back(static_cast<UINT>(NextTop));
		// 삼각형 2
		Indices.push_back(static_cast<UINT>(CurrBot));
		Indices.push_back(static_cast<UINT>(NextTop));
		Indices.push_back(static_cast<UINT>(CurrTop));
	}

	Ext_DirectXVertexBuffer::CreateVertexBuffer("Cylinder", Vertices);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Cylinder", Indices);
	Ext_DirectXMesh::CreateMesh("Cylinder");
}