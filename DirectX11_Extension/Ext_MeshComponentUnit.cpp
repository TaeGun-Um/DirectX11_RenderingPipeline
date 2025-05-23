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
	InputLayout = std::make_shared<Ext_DirectXInputLayout>(); // ������ ������
}

Ext_MeshComponentUnit::~Ext_MeshComponentUnit()
{
}

// Mesh, Material Initialize
void Ext_MeshComponentUnit::MeshComponentUnitInitialize(std::string_view _MeshName, std::string_view _MaterialName)
{
	Mesh = Ext_DirectXMesh::Find(_MeshName); // �޽� ����
	Material = Ext_DirectXMaterial::Find(_MaterialName); // ��Ƽ���� ����
	
	if (nullptr == Mesh || nullptr == Material)
	{
		MsgAssert("�������� �ʴ� �޽ó� ��Ƽ������ �޽����ֿ� ���� ���� �����ϴ�.")
	}

	InputLayout->CreateInputLayout(Mesh->GetVertexBuffer(), Material->GetVertexShader()); // InputLayout ����

	// ������� ����
	// [1] ���ؽ� ���̴� ���� ��������
	const Ext_DirectXBufferSetter& VertexShaderBuffers = Material->GetVertexShader()->GetBufferSetter();
	BufferSetter.Copy(VertexShaderBuffers);

	// [2] �ȼ� ���̴� ���� ��������
	const Ext_DirectXBufferSetter& PixelShaderBuffers = Material->GetPixelShader()->GetBufferSetter();
	BufferSetter.Copy(PixelShaderBuffers);

	// [3] Ʈ������ ������� �����ϱ�
	const TransformData& Data = *(OwnerMeshComponent.lock()->GetTransform()->GetTransformData().get());
	BufferSetter.SetConstantBufferLink("TransformData", Data);

	// [4] ī�޶� �ֱ�
	GetOwnerMeshComponent().lock()->GetOwnerCamera().lock()->PushMeshComponentUnit(GetSharedFromThis<Ext_MeshComponentUnit>(), RenderPath::Unknown);
}

// Mesh, Material�� RenderingPipeline Setting
void Ext_MeshComponentUnit::RenderUnitSetting()
{
	if (nullptr == Mesh)
	{
		MsgAssert("�Ž��� �������� �ʴ� ����Ʈ �Դϴ�");
	}

	if (nullptr == Material)
	{
		MsgAssert("������������ �������� �ʴ� ����Ʈ �Դϴ�");
	}

	InputLayout->InputLayoutSetting(); // InputLayout
	Mesh->MeshSetting(); // IA1, IA2 
	Material->MaterialSetting(); // ���� ����
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