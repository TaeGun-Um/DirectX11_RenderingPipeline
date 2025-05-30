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
#include "Ext_MeshLoader.h"

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
	Dir.MakePath("../Resource/Texture");
	std::vector<std::string> Paths = Dir.GetAllFile({ "png", "tga", "dss" });
	for (const std::string& FilePath : Paths)
	{
		Dir.SetPath(FilePath.c_str());
		std::string ExtensionName = Dir.GetExtension();
		std::string FileName = Dir.GetFileName();
		Ext_DirectXTexture::LoadTexture(FilePath.c_str(), FileName.c_str(), ExtensionName.c_str());
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

	// 메시 로드(테스트용)
	{
		// 파일 찾아서 메시로드 한 다음, 안에 std::vector<Ext_DirectXVertexData> Vertices;랑 std::vector<UINT> Index; 전달한거 받아서 Create 3종 실시

		//Base_Directory Dir;
		//Dir.MakePath("../Resource/Texture");
		//std::vector<std::string> Paths = Dir.GetAllFile({ "png", "tga", "dss" });
		//for (const std::string& FilePath : Paths)
		//{
		//	Dir.SetPath(FilePath.c_str());
		//	std::string ExtensionName = Dir.GetExtension();
		//	std::string FileName = Dir.GetFileName();
		//	Ext_DirectXTexture::LoadTexture(FilePath.c_str(), FileName.c_str(), ExtensionName.c_str());
		//}

		//std::vector<Ext_DirectXVertexData> Vertices;
		//std::vector<UINT> Indices;

		//Base_Directory Dir;
		//Dir.MakePath("../Resource/Mesh/SciCharacter/source/Belorian Soldier LP.obj");
		//Ext_MeshLoader::LoadMeshFromFile(Dir.GetPath(), Vertices, Indices);

		//Ext_DirectXVertexBuffer::CreateVertexBuffer("Belorian Soldier LP", Vertices);
		//Ext_DirectXIndexBuffer::CreateIndexBuffer("Belorian Soldier LP", Indices);
		//Ext_DirectXMesh::CreateMesh("Belorian Soldier LP");

		//Base_Directory Dir2;
		//Dir2.MakePath("../Resource/Mesh/SciCharacter/textures");
		//std::vector<std::string> Paths = Dir2.GetAllFile({ "png", "tga", "dss" });
		//for (const std::string& FilePath : Paths)
		//{
		//	Dir2.SetPath(FilePath.c_str());
		//	std::string ExtensionName = Dir2.GetExtension();
		//	std::string FileName = Dir2.GetFileName();
		//	Ext_DirectXTexture::LoadTexture(FilePath.c_str(), FileName.c_str(), ExtensionName.c_str());
		//}

		std::vector<Ext_DirectXVertexData> Vertices;
		std::vector<UINT> Indices;
		std::unordered_map<std::string, int> BoneMapping;
		std::vector<float4x4> BoneOffsets;
		aiNode* RootNode = nullptr;
		std::unique_ptr<Assimp::Importer> Importer;
		float4x4 Global;

		Base_Directory Dir;
		Dir.MakePath("../Resource/Mesh/Character/Mesh/Girl.fbx");
		Ext_MeshLoader::LoadMeshFromFile(Dir.GetPath(), Vertices, Indices);

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Girl", Vertices);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Girl", Indices);
		Ext_DirectXMesh::CreateMesh("Girl");

		/////////////////////////////////////////////////////////////////////////

		Base_Directory Dir2;
		Dir2.MakePath("../Resource/Mesh/Character/Texture");
		std::vector<std::string> Paths = Dir2.GetAllFile({ "png", "tga", "dss" });
		for (const std::string& FilePath : Paths)
		{
			Dir2.SetPath(FilePath.c_str());
			std::string ExtensionName = Dir2.GetExtension();
			std::string FileName = Dir2.GetFileName();
			Ext_DirectXTexture::LoadTexture(FilePath.c_str(), FileName.c_str(), ExtensionName.c_str());
		}

		int a = 0;
	}

	// 삼각형
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

	// 기본 메쉬 생성, Rect
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

	// 정육면체
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
	// 샘플러
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

		Ext_DirectXSampler::CreateSampler("BaseSampler", SamperInfo);
	}
	{
		//D3D11_SAMPLER_DESC SamperInfo = {};

		//SamperInfo.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		//SamperInfo.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		//SamperInfo.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		//SamperInfo.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		//SamperInfo.MipLODBias = 0.0f;
		//SamperInfo.MaxAnisotropy = 1;
		//SamperInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//SamperInfo.MinLOD = -FLT_MAX;
		//SamperInfo.MaxLOD = FLT_MAX;

		//Ext_DirectXSampler::CreateSampler("POINTSAMPLER", SamperInfo);
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

	{
		//D3D11_BLEND_DESC Desc = { 0, };

		//Desc.AlphaToCoverageEnable = false;
		//Desc.IndependentBlendEnable = false;

		//Desc.RenderTarget[0].BlendEnable = true;
		//Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		//Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		//Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		//Ext_DirectXBlend::CreateBlend("MergeBlend", Desc);
	}

	{
		//D3D11_BLEND_DESC Desc = { 0, };

		//Desc.AlphaToCoverageEnable = false;
		//Desc.IndependentBlendEnable = false;

		//Desc.RenderTarget[0].BlendEnable = true;
		//Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
		//Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		//Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		//Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		//Ext_DirectXBlend::CreateBlend("MaxMergeBlend", Desc);
	}

	{
		//D3D11_BLEND_DESC Desc = { 0, };

		//Desc.AlphaToCoverageEnable = false;
		//Desc.IndependentBlendEnable = false;

		//Desc.RenderTarget[0].BlendEnable = true;
		//Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		//Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		//Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		//Ext_DirectXBlend::CreateBlend("LightBlend", Desc);
	}

	{
		//D3D11_BLEND_DESC Desc = { 0, };

		//Desc.AlphaToCoverageEnable = false;
		//Desc.IndependentBlendEnable = false;

		//Desc.RenderTarget[0].BlendEnable = true;
		//Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		//Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		//Ext_DirectXBlend::CreateBlend("OneBlend", Desc);
	}

	{
		//D3D11_BLEND_DESC Desc = { 0, };

		//Desc.AlphaToCoverageEnable = false;
		//Desc.IndependentBlendEnable = false;

		//Desc.RenderTarget[0].BlendEnable = true;
		//Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
		//Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		//Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		//Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		//Ext_DirectXBlend::CreateBlend("MinBlend", Desc);
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

	{
		//D3D11_DEPTH_STENCIL_DESC Desc = { 0, };

		//Desc.DepthEnable = true;
		//Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		//Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		//Desc.StencilEnable = false;

		//Ext_DirectXDepth::CreateDepthStencilState("AlwayDepth", Desc);
	}
	{
		//D3D11_DEPTH_STENCIL_DESC Desc = { 0, };

		//Desc.DepthEnable = true;
		//Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		//Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		//Desc.StencilEnable = false;

		//Ext_DirectXDepth::CreateDepthStencilState("AlphaDepth", Desc);
	}
}

// DirectX11 Rasterizer 생성
void Ext_DirectXResourceLoader::MakeRasterizer()
{
	D3D11_RASTERIZER_DESC Desc = {};

	Desc.CullMode = D3D11_CULL_BACK; // 뒷면 제거(백페이스 컬링 활성화)
	Desc.FrontCounterClockwise = TRUE; // 반시계방향이 앞면(앞면 기준을 CCW로 지정)
	Desc.FillMode = D3D11_FILL_SOLID;
	// Desc.DepthClipEnable = FALSE;          // Z-Clipping 안함

	Ext_DirectXRasterizer::CreateRasterizer("EngineRasterizer", Desc);
}

// 렌더링 파이프라인 생성
void Ext_DirectXResourceLoader::MakeMaterial()
{
	// 일반(단일 메시)
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Basic");
		NewRenderingPipeline->SetVertexShader("Basic_VS");
		NewRenderingPipeline->SetPixelShader("Basic_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("EngineRasterizer");
	}

	// 다양한 텍스쳐가 있는 메시
	{
		std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("PBR");
		NewRenderingPipeline->SetVertexShader("Basic_VS");
		NewRenderingPipeline->SetPixelShader("PBR_PS");
		NewRenderingPipeline->SetBlendState("BaseBlend");
		NewRenderingPipeline->SetDepthState("EngineDepth");
		NewRenderingPipeline->SetRasterizer("EngineRasterizer");
	}
}
