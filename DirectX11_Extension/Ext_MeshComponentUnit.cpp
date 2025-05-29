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

void Ext_MeshComponentUnit::Release()
{
	// [2] 렌더링 자원 해제
	InputLayout = nullptr;
	Mesh = nullptr;
	Material = nullptr;

	// [3] 부모 연결 해제
	OwnerMeshComponent.reset();
}

// 메시 컴포넌트 유닛 생성 시 호출, Mesh, Material, ConstantBuffer 세팅
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

// 텍스쳐 변경하기
void Ext_MeshComponentUnit::SetTexture(std::string_view _TextureName, const std::string& _SettingTexture /*= "DefaultTex"*/)
{
	BufferSetter.SetTexture(_SettingTexture, _TextureName);
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

	InputLayout->InputLayoutSetting(); // InputLayout으로 IASetInputLayout 호출
	Mesh->MeshSetting();					 // InputAssembler 1단계, InputAssembler 2단계 호출 : IASetVertexBuffers, IASetPrimitiveTopology, IASetIndexBuffer
	Material->MaterialSetting();			 // VertexShader, PixelShader 호출(렌더링 파이프라인 단계 실행) : VSSetShader, PSSetShader
	BufferSetter.BufferSetting();			 // Map -> memcpy -> UnMap, VSSetConstantBuffers, PSSetConstantBuffers 실시
}

// 정점 정보들과 셰이더를 통해 메시 Draw 실시(DrawIndexed Call)
void Ext_MeshComponentUnit::RenderUnitDraw()
{
	UINT IndexCount = Mesh->GetIndexBuffer()->GetVertexCount();
	Ext_DirectXDevice::GetContext()->DrawIndexed(IndexCount, 0, 0); // 파이프라인 설정 완료 후, GPU에게 그리라고 명령
	// 1. 그릴 인덱스 갯수(Mesh가 가진 전체 인덱스 갯수)
	// 2. 인덱스 버퍼 내에서 시작할 위치(보통 0부터 시작)
	// 3. 정점 버퍼 내 정점 인덱스의 시작값 오프셋(주로 0)
}

void Ext_MeshComponentUnit::Rendering(float _Deltatime)
{
	RenderUnitSetting();
	RenderUnitDraw();
}