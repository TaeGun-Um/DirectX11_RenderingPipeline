#include "PrecompileHeader.h"
#include "Ext_Camera.h"

#include <DirectX11_Base/Base_Windows.h>

#include "Ext_DirectXDevice.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_DirectXMaterial.h"

Ext_Camera::Ext_Camera()
{
}

Ext_Camera::~Ext_Camera()
{
}

// ���� ī�޶� ���� ��, ��������, ����Ʈ ����
void Ext_Camera::CameraTransformUpdate()
{
	float4 EyeDir = GetTransform()->GetLocalForwardVector();
	float4 EyeUp = GetTransform()->GetLocalUpVector();
	float4 EyePos = GetTransform()->GetWorldPosition();
	ViewMatrix.LookToLH(EyePos, EyeDir, EyeUp);

	switch (ProjectionType)
	{
	case CameraType::None:
	{
		MsgAssert("ī�޶� ������ �������� �ʾҽ��ϴ�.");
		break;
	}
	case CameraType::Perspective:
		ProjectionMatrix.PerspectiveFovLH(FOV, Width / Height, Near, Far);
		break;
	case CameraType::Orthogonal:
		// Projection.OrthographicLH(Width * ZoomRatio, Height * ZoomRatio, Near, Far);
		// ������ �ϴ� �н�
		break;
	default:
		break;
	}

	ViewPortMatrix.ViewPort(Width, Height, 0.0f, 0.0f);
}

// MeshComponents�� ������ MeshComponent �ֱ�
void Ext_Camera::PushMeshComponent(std::shared_ptr<Ext_MeshComponent> _MeshComponent)
{
	if (nullptr == _MeshComponent)
	{
		MsgAssert("MeshComponent�� nullptr �Դϴ�");
		return;
	}

	_MeshComponent->SetOwnerCamera(GetSharedFromThis<Ext_Camera>());
	MeshComponents[GetOrder()].push_back(_MeshComponent);
}

// ������ MeshComponentUnit�� ī�޶��� MeshComponentUnits�� �ֱ�
void Ext_Camera::PushMeshComponentUnit(std::shared_ptr<Ext_MeshComponentUnit> _Unit, RenderPath _Path /*= RenderPath::None*/)
{
	int Order = _Unit->GetOwnerMeshComponent().lock()->GetOrder();
	// RenderPath Path = _Unit->GetMaterial()->GetPixelShader()->GetRenderPath();
	MeshComponentUnits[_Path][Order].push_back(_Unit);

	// ���⼭ ����, �������� ���� ���� �ִ�.
}

// ��, ������������� MeshComponent�� Transform�� ����
void Ext_Camera::MeshComponentTransformUpdate(std::shared_ptr<Ext_Camera> _Camera)
{
	if (nullptr == _Camera)
	{
		MsgAssert("������ �Ϸ��µ� ī�޶� �����ִ�.");
		return;
	}

	GetTransform()->SetCameraMatrix(_Camera->GetViewMatrix(), _Camera->GetProjectionMatrix());
}

void Ext_Camera::ViewPortSetting()
{
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, &ViewPortData);
}

void Ext_Camera::Start()
{
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = Base_Windows::GetScreenSize().x;
	ViewPortData.Height = Base_Windows::GetScreenSize().y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	Width = ViewPortData.Width;
	Height = ViewPortData.Height;
}

// ī�޶��� MeshComponents�鿡 ���� ������Ʈ �� ������ ���������� ���ҽ� ����
void Ext_Camera::Rendering(float _Deltatime)
{
	Ext_DirectXDevice::GetMainRenderTarget()->RenderTargetClear(); // [1] ���� ���� Ÿ��(�� ����) Clear �ǽ�, ����Ÿ�ٺ�� �������ٽǺ並 Ŭ�����Ѵ�.

	// MeshComponents ������ ������Ʈ ����
	for (auto& [Key, MeshComponentList] : MeshComponents)
	{
		// [!] �ʿ��ϸ� ZSort �ǽ�(���߿�)

		for (const std::shared_ptr<Ext_MeshComponent>& CurMeshComponent : MeshComponentList)
		{
			if (!CurMeshComponent->GetIsUpdate()) continue;
			else
			{
				MeshComponentTransformUpdate(GetSharedFromThis<Ext_Camera>()); // [2] ���� MeshComponent���� ī�޶��� View, Projection �����ֱ�
				// [!] �ʿ��ϸ� �ȼ� ���̴����� Ȱ���� Value�� ������Ʈ
			}
		}

	}

	// ������Ʈ �޽��� ���ֵ� ��ȸ
	for (auto& [RenderPathKey, UnitMap] : MeshComponentUnits)
	{
		switch (RenderPathKey)
		{
		case RenderPath::Forward:
		{

			break;
		}
		case RenderPath::Deferred:
		{

			break;
		}
		case RenderPath::Alpha:
		{

			break;
		}
		case RenderPath::Unknown:
		{
			Ext_DirectXDevice::GetMainRenderTarget()->RenderTargetSetting(); // [3] ���� ���� Ÿ�� ����(�� ����) Setting �ǽ�
			break;
		}
		default:
		{
			MsgAssert("what?");
			break;
		}
		}

		for (auto& [IndexKey, UnitList] : UnitMap)
		{
			for (auto& Unit : UnitList)
			{
				Unit->Rendering(_Deltatime); // ����
			}
		}

		// 6. MeshComponents�� Render(_Deltatime); ȣ��
		// <<�ϴ°�>>
		// ���� ���ֿ� �ִ� Ext_Device::GetContext()->IASetInputLayout(InputLayOut); ȣ��
		// ���� ������ GameEngineMesh�� �ִ� GameEngineDevice::GetContext()->IASetVertexBuffers(0, 1, &Buffer, &VertexSize, &Offset);, GameEngineDevice::GetContext()->IASetPrimitiveTopology(TOPOLOGY); ȣ�� [InputAssmbler1]
		// ���� ������ GameEngineMesh�� �ִ� GameEngineDevice::GetContext()->IASetIndexBuffer(Buffer, Format, Offset); ȣ�� [InputAssmbler2]
		// ������ ������������ �� �ܰ� ȣ��, << Ext_Material���� ���� >>
		// 1. VertexShader :  GameEngineDevice::GetContext()->VSSetShader(ShaderPtr, nullptr, 0);
		// 2. GeometryShader : GameEngineDevice::GetContext()->GSSetShader(ShaderPtr, nullptr, 0); <<��� ��>>
		// 3. Resterizer : RasterizerPtr->SetFILL_MODE(FILL_MODE);, GameEngineDevice::GetContext()->RSSetState(CurState);
		// 4. PixelShader : GameEngineDevice::GetContext()->PSSetShader(ShaderPtr, nullptr, 0);
		// 5. OutputMerget : GameEngineDevice::GetContext()->OMSetBlendState(State, nullptr, Mask);, GameEngineDevice::GetContext()->OMSetDepthStencilState(State, 0);
		// 6. ������� ����, ���̴����ҽ����ۿ��� ���� : GameEngineStructuredBufferSetter, GameEngineSamplerSetter, GameEngineTextureSetter, GameEngineConstantBufferSetter�� ����
		// <<GameEngineConstantBufferSetter>> :	GameEngineDevice::GetContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SettingResources);, memcpy_s(SettingResources.pData, BufferInfo.ByteWidth, _Data, BufferInfo.ByteWidth);, GameEngineDevice::GetContext()->Unmap(Buffer, 0); ȣ��
		// �״����� GameEngineDevice::GetContext()->VSSetConstantBuffers(_Slot, 1, &Buffer);, Res->PSSetting(BindPoint); �ǽ�
		// 7. Draw
		// �������ֿ��� �ǽ�
		// 	UINT IndexCount = Mesh->IndexBufferPtr->GetIndexCount();
		// GameEngineDevice::GetContext()->DrawIndexed(IndexCount, 0, 0);
	}

	// ��
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}










// Zsort ����(����)
///////////////////////////////////////////////////////////////////////////////////////////////////////////int Order = RenderGroupStartIter->first;
///////////////////////////////////////////////////////////////////////////////////////////////////////////std::map<int, SortType>::iterator SortIter = SortValues.find(Order);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////if (SortIter != SortValues.end() && SortIter->second != SortType::None)
///////////////////////////////////////////////////////////////////////////////////////////////////////////{
///////////////////////////////////////////////////////////////////////////////////////////////////////////	if (SortIter->second == SortType::ZSort)
///////////////////////////////////////////////////////////////////////////////////////////////////////////	{
///////////////////////////////////////////////////////////////////////////////////////////////////////////		for (std::shared_ptr<GameEngineRenderer>& Render : RenderGroup)
///////////////////////////////////////////////////////////////////////////////////////////////////////////		{
///////////////////////////////////////////////////////////////////////////////////////////////////////////			Render->CalSortZ(this);
///////////////////////////////////////////////////////////////////////////////////////////////////////////		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////		RenderGroup.sort([](std::shared_ptr<GameEngineRenderer>& _Left, std::shared_ptr<GameEngineRenderer>& _Right)
///////////////////////////////////////////////////////////////////////////////////////////////////////////			{
///////////////////////////////////////////////////////////////////////////////////////////////////////////				return _Left->CalZ > _Right->CalZ;
///////////////////////////////////////////////////////////////////////////////////////////////////////////			});
///////////////////////////////////////////////////////////////////////////////////////////////////////////	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////}