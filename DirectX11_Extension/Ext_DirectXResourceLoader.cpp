#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXVertexData.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXMaterial.h"
#include "Ext_DirectXRasterizer.h"
#include "Ext_DirectXDepth.h"

#include "Ext_DirectXDevice.h"// 임시

// DirectX에 필요한 리소스를 로드
void Ext_DirectXResourceLoader::Initialize()
{
	MakeVertex(); // Ext_DirectXVertexData 클래스의 InputLayoutElement에 SemanticName, Format 결정
	MakeSampler();
	MakeBlend();
	MakeDepth();
	MakeRasterizer();
	ShaderCompile(); // 셰이더 컴파일 후 상수버퍼 생성
	MakeMaterial(); // 머티리얼(파이프라인) 생성
}

// 정점 정보 생성(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

	// 삼각형
	{
		std::vector<Ext_DirectXVertexData> Vertices;
		Vertices.resize(3);

		Vertices[0] = { { 0.0f, 0.5f, 0.0 }, { 1, 0, 0, 1 } };
		Vertices[1] = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
		Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };

		std::vector<UINT> ArrIndex = { 0, 2, 1 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", Vertices);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Triangle", ArrIndex);
		Ext_DirectXMesh::CreateMesh("Triangle");
	}

	// 기본 메쉬 생성, Rect
	{
		std::vector<Ext_DirectXVertexData> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { 1, 0, 0, 1 }, /*{ 0.0f, 0.0f }*/ };
		ArrVertex[1] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { 0, 1, 0, 1 }, /*{ 1.0f, 0.0f }*/ };
		ArrVertex[2] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0, 0, 1, 1 }, /*{ 1.0f, 1.0f }*/ };
		ArrVertex[3] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { 1, 1, 0, 1 }, /*{ 0.0f, 1.0f }*/ };

		std::vector<UINT> ArrIndex = { 0, 2, 1, 2, 3, 1 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Rect", ArrIndex);
		Ext_DirectXMesh::CreateMesh("Rect");
	}

	// 정육면체
	{
		std::vector<Ext_DirectXVertexData> Vertex =
		{
			// Front (+Z)
			{ {-0.5f,  0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {0, 0, 1} },
			{ { 0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {0, 0, 1} },
			{ { 0.5f, -0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {0, 0, 1} },
			{ {-0.5f, -0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {0, 0, 1} },

			// Back (-Z)
			{ { 0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {0, 0, -1} },
			{ {-0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {0, 0, -1} },
			{ {-0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {0, 0, -1} },
			{ { 0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {0, 0, -1} },

			// Left (-X)
			{ {-0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {-1, 0, 0} },
			{ {-0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {-1, 0, 0} },
			{ {-0.5f, -0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {-1, 0, 0} },
			{ {-0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {-1, 0, 0} },

			// Right (+X)
			{ { 0.5f,  0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {1, 0, 0} },
			{ { 0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {1, 0, 0} },
			{ { 0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {1, 0, 0} },
			{ { 0.5f, -0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {1, 0, 0} },

			// Top (+Y)
			{ {-0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {0, 1, 0} },
			{ { 0.5f,  0.5f, -0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {0, 1, 0} },
			{ { 0.5f,  0.5f,  0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {0, 1, 0} },
			{ {-0.5f,  0.5f,  0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {0, 1, 0} },

			// Bottom (-Y)
			{ {-0.5f, -0.5f,  0.5f, 1.0f}, {1, 0, 0, 1}, {0, 0}, {0, -1, 0} },
			{ { 0.5f, -0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {1, 0}, {0, -1, 0} },
			{ { 0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, 1, 1}, {1, 1}, {0, -1, 0} },
			{ {-0.5f, -0.5f, -0.5f, 1.0f}, {1, 1, 0, 1}, {0, 1}, {0, -1, 0} },
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

	// 테스트용
	{
		float AngleDeg = 45.0f;
		float AngleRad = AngleDeg * (3.14159265f / 180.0f);

		float Cos = cosf(AngleRad); // ? 0.7071
		float Sin = sinf(AngleRad); // ? 0.7071

		auto RotateY = [&](float x, float y, float z)
			{
				return float4(
					x * Cos - z * Sin,
					y,
					x * Sin + z * Cos,
					1.0f
				);
			};

		std::vector<Ext_DirectXVertexData> Vertex =
		{
			{ RotateY(-0.5f,  0.5f, 0.0f), {1, 0, 0, 1}, {0, 0}, {0, 0, 1} }, // LT
			{ RotateY(0.5f,  0.5f, 0.0f), {0, 1, 0, 1}, {1, 0}, {0, 0, 1} }, // RT
			{ RotateY(0.5f, -0.5f, 0.0f), {0, 0, 1, 1}, {1, 1}, {0, 0, 1} }, // RB
			{ RotateY(-0.5f, -0.5f, 0.0f), {1, 1, 0, 1}, {0, 1}, {0, 0, 1} }, // LB
		};

		std::vector<UINT> Index =
		{
			0, 1, 2,
			2, 3, 0
		};

		Ext_DirectXVertexBuffer::CreateVertexBuffer("RotateFace", Vertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("RotateFace", Index);
		std::shared_ptr<Ext_DirectXMesh> Mesh = Ext_DirectXMesh::CreateMesh("RotateFace");
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

void Ext_DirectXResourceLoader::MakeSampler() 
{

}

void Ext_DirectXResourceLoader::MakeBlend() 
{

}

// DirectX11 DepthStencilState 생성
void Ext_DirectXResourceLoader::MakeDepth() 
{
	D3D11_DEPTH_STENCIL_DESC Desc = { 0, };

	Desc.DepthEnable = true;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	Desc.StencilEnable = false;
	/*1. 깊이 테스트 수행함(true)*/
	/*2. 깊이 값을 Z버퍼에 기록*/
	/*3. 새 픽셀이 더 가깝거나 같으면 통과*/
	/*4. 스텐실 안함*/

	Ext_DirectXDepth::CreateDepthStencilState("EngineDepth", Desc);
}

// DirectX11 Rasterizer 생성
void Ext_DirectXResourceLoader::MakeRasterizer() 
{
	D3D11_RASTERIZER_DESC Desc = {};

	Desc.CullMode = D3D11_CULL_BACK; // 뒷면 제거(백페이스 컬링 활성화)
	Desc.FrontCounterClockwise = TRUE; // 반시계방향이 앞면(앞면 기준을 CCW로 지정)
	// Desc.DepthClipEnable = FALSE;          // Z-Clipping 안함

	Ext_DirectXRasterizer::CreateRasterizer("EngineRasterizer", Desc);
}

// 렌더링 파이프라인 생성
void Ext_DirectXResourceLoader::MakeMaterial()
{
	std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Basic");
	NewRenderingPipeline->SetVertexShader("Basic_VS");
	NewRenderingPipeline->SetPixelShader("Basic_PS");
	// NewRenderingPipeline->SetBlendState("BaseBlend");
	NewRenderingPipeline->SetDepthState("EngineDepth");
	NewRenderingPipeline->SetRasterizer("EngineRasterizer");
}
