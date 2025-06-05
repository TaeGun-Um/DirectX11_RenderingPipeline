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

// 정점 정보 생성(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex()
{
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);
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
}

// DirectX 블렌드 생성
void Ext_DirectXResourceLoader::MakeBlend()
{
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
}

// DirectX11 DepthStencilState 생성
void Ext_DirectXResourceLoader::MakeDepth()
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

// DirectX11 Rasterizer 생성
void Ext_DirectXResourceLoader::MakeRasterizer()
{
	{
		D3D11_RASTERIZER_DESC Desc = {};

		Desc.CullMode = D3D11_CULL_BACK; // 뒷면 제거(백페이스 컬링 활성화)
		Desc.FrontCounterClockwise = TRUE; // 반시계방향이 앞면(앞면 기준을 CCW로 지정)
		Desc.FillMode = D3D11_FILL_SOLID;
		// Desc.DepthClipEnable = FALSE;          // Z-Clipping 안함

		Ext_DirectXRasterizer::CreateRasterizer("BasicRasterizer", Desc);
	}

	{
		D3D11_RASTERIZER_DESC Desc = {};

		Desc.CullMode = D3D11_CULL_NONE; // 뒷면 제거(백페이스 컬링 활성화)
		Desc.FrontCounterClockwise = TRUE; // 반시계방향이 앞면(앞면 기준을 CCW로 지정)
		Desc.FillMode = D3D11_FILL_SOLID;
		// Desc.DepthClipEnable = FALSE;          // Z-Clipping 안함

		std::shared_ptr<Ext_DirectXRasterizer> NewRast = Ext_DirectXRasterizer::CreateRasterizer("DebugRasterizer", Desc);
		NewRast->SetFILL_MODE(D3D11_FILL_WIREFRAME);
	}
}

// CurState = WireframeState;

// 렌더링 파이프라인 생성
void Ext_DirectXResourceLoader::MakeMaterial()
{
	// 일반 스태틱 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("StaticNonG");
		NewRenderingPipeline->SetVertexShader("Basic_VS");
		NewRenderingPipeline->SetPixelShader("Basic_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽스 스태틱 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Static");
		NewRenderingPipeline->SetVertexShader("Grapics_VS");
		NewRenderingPipeline->SetPixelShader("Grapics_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 일반 다이나믹 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("DynamicNonG");
		NewRenderingPipeline->SetVertexShader("Dynamic_VS");
		NewRenderingPipeline->SetPixelShader("Basic_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 그래픽스 다이나믹 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Dynamic");
		NewRenderingPipeline->SetVertexShader("Dynamic_VS");
		NewRenderingPipeline->SetPixelShader("Grapics_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 다양한 텍스쳐가 있는 메시
	{
		//std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("PBR");
		//NewRenderingPipeline->SetVertexShader("Basic_VS");
		//NewRenderingPipeline->SetPixelShader("PBR_PS");
		//NewRenderingPipeline->SetBlendState("BaseBlend");
		//NewRenderingPipeline->SetDepthState("EngineDepth");
		//NewRenderingPipeline->SetRasterizer("BasicRasterizer");
	}

	// 와이어프레임(디버깅용)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Debug");
		NewRenderingPipeline->SetVertexShader("Debug_VS");
		NewRenderingPipeline->SetPixelShader("Debug_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("DebugRasterizer");
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
	std::vector<Ext_DirectXVertexData> ArrVertex;
	ArrVertex.resize(4);

	ArrVertex[0] = { { 0.5f,  0.5f, }, {1, 0, 0, 1}, {1, 0}, {0, 0, -1} };
	ArrVertex[1] = { {-0.5f,  0.5f, }, {0, 1, 0, 1}, {0, 0}, {0, 0, -1} };
	ArrVertex[2] = { {-0.5f, -0.5f, }, {0, 0, 1, 1}, {0, 1}, {0, 0, -1} };
	ArrVertex[3] = { { 0.5f, -0.5f, }, {1, 1, 0, 1}, {1, 1}, {0, 0, -1} };

	std::vector<UINT> ArrIndex = { 0, 1, 2, 0, 2, 3, };

	Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", ArrVertex);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Rect", ArrIndex);
	Ext_DirectXMesh::CreateMesh("Rect");
}

void Ext_DirectXResourceLoader::CreateCube()
{
	std::vector<Ext_DirectXVertexData> Vertex =
	{
		// Front (+Z)
		{ {-0.5f,  0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {0, 0, 1} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {0, 0, 1} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {0, 0, 1} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {0, 0, 1} },

		// Back (-Z)
		{ { 0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {0, 0, -1} },
		{ {-0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {0, 0, -1} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {0, 0, -1} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {0, 0, -1} },

		// Left (-X)
		{ {-0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {-1, 0, 0} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {-1, 0, 0} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {-1, 0, 0} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {-1, 0, 0} },

		// Right (+X)
		{ { 0.5f,  0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {1, 0, 0} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {1, 0, 0} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {1, 0, 0} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {1, 0, 0} },

		// Top (+Y)
		{ {-0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {0, 1, 0} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {0, 1, 0} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {0, 1, 0} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {0, 1, 0} },

		// Bottom (-Y)
		{ {-0.5f, -0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {1, 0}, {0, -1, 0} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {0, 0}, {0, -1, 0} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {0, 1}, {0, -1, 0} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {1, 1}, {0, -1, 0} },
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	const int SliceCount = 20;      // 경도 분할 수
	const int StackCount = 20;      // 위도 분할 수
	const float Radius = 0.5f;  // 구 반지름 (Box의 절반 크기와 비슷하게 0.5f 설정)
	const float PI = 3.1415926535f;

	// 1) 정점 생성
	std::vector<Ext_DirectXVertexData> Vertices;
	Vertices.reserve((StackCount + 1) * (SliceCount + 1));

	for (int i = 0; i <= StackCount; ++i)
	{
		// 위도 각도 (0 ~ PI)
		float Phi = static_cast<float>(i) * PI / static_cast<float>(StackCount);

		for (int j = 0; j <= SliceCount; ++j)
		{
			// 경도 각도 (0 ~ 2PI)
			float Theta = static_cast<float>(j) * 2.0f * PI / static_cast<float>(SliceCount);

			float x = Radius * sinf(Phi) * cosf(Theta);
			float y = Radius * cosf(Phi);
			float z = Radius * sinf(Phi) * sinf(Theta);

			// 텍스처 좌표 (u, v)
			float u = Theta / (2.0f * PI);
			float v = Phi / PI;

			// 컬러를 흰색으로 통일
			float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

			// 노말은 반지름을 1로 정규화한 방향 벡터
			float nx = x / Radius;
			float ny = y / Radius;
			float nz = z / Radius;

			Vertices.push_back(
				{
					{ x, y, z, 1.0f },     // Position (float4)
					{ r, g, b, a },        // Color    (float4)
					{ u, v },              // TexCoord (float2)
					{ nx, ny, nz }         // Normal   (float3)
				});
		}
	}

	// 2) 인덱스 생성 (각 스택과 슬라이스마다 두 개의 삼각형)
	std::vector<UINT> Indices;
	for (int i = 0; i < StackCount; ++i)
	{
		for (int j = 0; j < SliceCount; ++j)
		{
			// 현재 스택-슬라이스 위치의 첫 번째 정점 인덱스
			int First = i * (SliceCount + 1) + j;
			int Second = (i + 1) * (SliceCount + 1) + j;

			// 삼각형1: First, Second, First+1
			Indices.push_back(static_cast<UINT>(First));
			Indices.push_back(static_cast<UINT>(Second));
			Indices.push_back(static_cast<UINT>(First + 1));

			// 삼각형2: Second, Second+1, First+1
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
	const int    SliceCount = 20;            // 원 둘레 분할 수
	const float  Radius = 0.5f;          // 반지름
	const float  Height = 1.0f;          // 높이
	const float  HalfHeight = Height * 0.5f; // 절반 높이
	const float  PI = 3.1415926535f;

	// 1) 정점 생성
	std::vector<Ext_DirectXVertexData> Vertices;

	// -- Top Center
	Vertices.push_back(
		{
			{ 0.0f,  HalfHeight,  0.0f, 1.0f },   // Position
			{ 1.0f, 1.0f, 1.0f, 1.0f },           // Color (흰색)
			{ 0.5f, 0.5f },                       // TexCoord (중앙 매핑)
			{ 0.0f, 1.0f, 0.0f }                  // Normal (위쪽)
		});

	// -- Top Circle Edge (SliceCount + 1개, 마지막은 첫 정점과 동일하여 링을 닫음)
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float u = (x / Radius + 1.0f) * 0.5f; // 텍스처 매핑: x/r ∈ [-1,1] → u ∈ [0,1]
		float v = (z / Radius + 1.0f) * 0.5f; // 텍스처 매핑: z/r ∈ [-1,1] → v ∈ [0,1]

		Vertices.push_back(
			{
				{ x,  HalfHeight, z, 1.0f },        // Position (Y = +HalfHeight)
				{ 1.0f, 1.0f, 1.0f, 1.0f },         // Color (흰색)
				{ u, v },                           // TexCoord (원형 매핑)
				{ 0.0f, 1.0f, 0.0f }                // Normal (위쪽)
			});
	}

	// -- Bottom Center
	Vertices.push_back(
		{
			{ 0.0f, -HalfHeight, 0.0f, 1.0f },    // Position
			{ 1.0f, 1.0f, 1.0f, 1.0f },           // Color (흰색)
			{ 0.5f, 0.5f },                       // TexCoord (중앙 매핑)
			{ 0.0f, -1.0f, 0.0f }                 // Normal (아래쪽)
		});

	// -- Bottom Circle Edge (SliceCount + 1개)
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float u = (x / Radius + 1.0f) * 0.5f;
		float v = (z / Radius + 1.0f) * 0.5f;

		Vertices.push_back(
			{
				{ x, -HalfHeight, z, 1.0f },        // Position (Y = -HalfHeight)
				{ 1.0f, 1.0f, 1.0f, 1.0f },         // Color (흰색)
				{ u, v },                           // TexCoord
				{ 0.0f, -1.0f, 0.0f }               // Normal (아래쪽)
			});
	}

	// -- Side: Bottom Ring (SliceCount + 1개)
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float nx = x / Radius;             // 노말(측면) X
		float nz = z / Radius;             // 노말(측면) Z
		float u = static_cast<float>(i) / static_cast<float>(SliceCount); // 텍스처 가로 위치
		float v = 1.0f;                   // 아래쪽 테두리

		Vertices.push_back(
			{
				{ x, -HalfHeight, z, 1.0f },      // Position (아래쪽 원주)
				{ 1.0f, 1.0f, 1.0f, 1.0f },       // Color (흰색)
				{ u, v },                         // TexCoord
				{ nx, 0.0f, nz }                  // Normal (측면)
			});
	}

	// -- Side: Top Ring (SliceCount + 1개)
	for (int i = 0; i <= SliceCount; ++i)
	{
		float Theta = (static_cast<float>(i) * 2.0f * PI) / static_cast<float>(SliceCount);
		float x = Radius * cosf(Theta);
		float z = Radius * sinf(Theta);
		float nx = x / Radius;
		float nz = z / Radius;
		float u = static_cast<float>(i) / static_cast<float>(SliceCount);
		float v = 0.0f;                   // 위쪽 테두리

		Vertices.push_back(
			{
				{ x,  HalfHeight, z, 1.0f },      // Position (위쪽 원주)
				{ 1.0f, 1.0f, 1.0f, 1.0f },       // Color (흰색)
				{ u, v },                         // TexCoord
				{ nx, 0.0f, nz }                  // Normal (측면)
			});
	}

	// 2) 인덱스 생성
	std::vector<UINT> Indices;

	const int OffsetTopCenter = 0;
	const int OffsetTopRingStart = OffsetTopCenter + 1;                    // 1
	const int OffsetBottomCenter = OffsetTopRingStart + (SliceCount + 1);  // 1 + (SliceCount+1)
	const int OffsetBottomRingStart = OffsetBottomCenter + 1;                 // ...
	const int OffsetSideBotStart = OffsetBottomRingStart + (SliceCount + 1);
	const int OffsetSideTopStart = OffsetSideBotStart + (SliceCount + 1);

	// -- Top Cap (OffsetTopCenter, OffsetTopRingStart ~ OffsetTopRingStart+SliceCount)
	//    (Center → Curr → Next) 순서로 CCW 윈딩을 맞춥니다.
	for (int i = 0; i < SliceCount; ++i)
	{
		Indices.push_back(static_cast<UINT>(OffsetTopCenter));               // 중심
		Indices.push_back(static_cast<UINT>(OffsetTopRingStart + i));        // 현재 링 정점
		Indices.push_back(static_cast<UINT>(OffsetTopRingStart + i + 1));    // 다음 링 정점
	}

	// -- Bottom Cap (OffsetBottomCenter, OffsetBottomRingStart ~ OffsetBottomRingStart+SliceCount)
	//    (Center → Next → Curr) 순서로 CCW 윈딩을 맞춥니다.
	for (int i = 0; i < SliceCount; ++i)
	{
		Indices.push_back(static_cast<UINT>(OffsetBottomCenter));            // 중심
		Indices.push_back(static_cast<UINT>(OffsetBottomRingStart + i + 1)); // 다음 링 정점
		Indices.push_back(static_cast<UINT>(OffsetBottomRingStart + i));     // 현재 링 정점
	}

	// -- Side Surface
	//    (CurrBot → NextBot → NextTop) 과 (CurrBot → NextTop → CurrTop) 순서가 CCW 윈딩입니다.
	for (int i = 0; i < SliceCount; ++i)
	{
		int CurrBot = OffsetSideBotStart + i;
		int NextBot = OffsetSideBotStart + i + 1;
		int CurrTop = OffsetSideTopStart + i;
		int NextTop = OffsetSideTopStart + i + 1;

		// 삼각형1: 아래현재 → 아래다음 → 위다음
		Indices.push_back(static_cast<UINT>(CurrBot));
		Indices.push_back(static_cast<UINT>(NextBot));
		Indices.push_back(static_cast<UINT>(NextTop));

		// 삼각형2: 아래현재 → 위다음 → 위현재
		Indices.push_back(static_cast<UINT>(CurrBot));
		Indices.push_back(static_cast<UINT>(NextTop));
		Indices.push_back(static_cast<UINT>(CurrTop));
	}

	// 3) 버퍼 생성 및 메시 등록
	Ext_DirectXVertexBuffer::CreateVertexBuffer("Cylinder", Vertices);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Cylinder", Indices);
	Ext_DirectXMesh::CreateMesh("Cylinder");
}