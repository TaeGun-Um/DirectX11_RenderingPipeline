#include "PrecompileHeader.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_MeshComponent.h"
#include "Ext_Camera.h"
#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXMaterial.h"
#include "Ext_DirectXBufferSetter.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXDevice.h"
#include "Ext_Transform.h"

#include "Ext_DirectXVertexData.h"

Ext_MeshComponentUnit::Ext_MeshComponentUnit()
{
	InputLayout = std::make_shared<Ext_DirectXInputLayout>(); // 만들어야 동작함
}

Ext_MeshComponentUnit::~Ext_MeshComponentUnit()
{
}

// Mesh, Material Initialize
void Ext_MeshComponentUnit::MeshComponentUnitInitialize(std::string_view _MeshName, std::string_view _MaterialName)
{
	Mesh = Ext_DirectXMesh::Find(_MeshName); // 메시 설정
	Material = Ext_DirectXMaterial::Find(_MaterialName); // 머티리얼 설정
	
	if (nullptr == Mesh || nullptr == Material)
	{
		MsgAssert("존재하지 않는 메시나 머티리얼을 메시유닛에 넣을 수는 없습니다.")
	}

	InputLayout->CreateInputLayout(Mesh->GetVertexBuffer(), Material->GetVertexShader()); // InputLayout 설정

	// 상수버퍼 세팅
	// [1] 버텍스 셰이더 정보 가져오기
	const Ext_DirectXBufferSetter& VertexShaderBuffers = Material->GetVertexShader()->GetBufferSetter();
	BufferSetter.Copy(VertexShaderBuffers);

	// [2] 픽셀 셰이더 정보 가져오기
	const Ext_DirectXBufferSetter& PixelShaderBuffers = Material->GetPixelShader()->GetBufferSetter();
	BufferSetter.Copy(PixelShaderBuffers);

	// [3] 트랜스폼 상수버퍼 세팅하기
	const TransformData& Data = *(OwnerMeshComponent.lock()->GetTransform()->GetTransformData().get());
	BufferSetter.SetConstantBufferLink("TransformData", Data);

	// [4] 카메라에 넣기
	GetOwnerMeshComponent().lock()->GetOwnerCamera().lock()->PushMeshComponentUnit(GetSharedFromThis<Ext_MeshComponentUnit>(), RenderPath::Unknown);
}

// Mesh, Material의 RenderingPipeline Setting
void Ext_MeshComponentUnit::RenderUnitSetting()
{
	if (nullptr == Mesh)
	{
		MsgAssert("매쉬가 존재하지 않는 유니트 입니다");
	}

	if (nullptr == Material)
	{
		MsgAssert("파이프라인이 존재하지 않는 유니트 입니다");
	}

	InputLayout->InputLayoutSetting(); // InputLayout
	Mesh->MeshSetting(); // IA1, IA2 
	Material->MaterialSetting(); // 이후 렌파
	BufferSetter.BufferSetting();
}

// 
void Ext_MeshComponentUnit::RenderUnitDraw()
{
	UINT IndexCount = Mesh->GetIndexBuffer()->GetVertexCount();
	Ext_DirectXDevice::GetContext()->DrawIndexed(IndexCount, 0, 0);
}

void Ext_MeshComponentUnit::Rendering(float _Deltatime)
{
	RenderUnitSetting();
	RenderUnitDraw();
}