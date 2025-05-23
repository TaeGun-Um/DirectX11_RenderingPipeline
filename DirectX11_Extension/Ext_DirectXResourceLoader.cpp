#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXVertexData.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXMaterial.h"

#include "Ext_DirectXDevice.h"// 임시

COMPTR<ID3D11RasterizerState> Ext_DirectXResourceLoader::RasterState; // 임시

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

		std::vector<UINT> Index;
		for (int i = 0; i < 6; ++i)
		{
			int Base = i * 4;

			Index.push_back(Base + 0);
			Index.push_back(Base + 1);
			Index.push_back(Base + 2);

			Index.push_back(Base + 2);
			Index.push_back(Base + 3);
			Index.push_back(Base + 0);
		}

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

void Ext_DirectXResourceLoader::MakeDepth() 
{

}

void Ext_DirectXResourceLoader::MakeRasterizer() 
{
	D3D11_RASTERIZER_DESC RasterDesc = {};
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_NONE;  // 뒷면 제거
	RasterDesc.FrontCounterClockwise = TRUE; // CCW를 앞면으로 인식
	Ext_DirectXDevice::GetDevice()->CreateRasterizerState(&RasterDesc, RasterState.GetAddressOf());
}

// 렌더링 파이프라인 생성
void Ext_DirectXResourceLoader::MakeMaterial()
{
	std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Basic");
	NewRenderingPipeline->SetVertexShader("Basic_VS");
	NewRenderingPipeline->SetPixelShader("Basic_PS");
	// NewRenderingPipeline->SetBlendState("BaseBlend");
	// NewRenderingPipeline->SetDepthState("EngineDepth");
	// NewRenderingPipeline->SetRasterizer("Engine2DBase");
}
