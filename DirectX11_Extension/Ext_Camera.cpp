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

// 현재 카메라 기준 뷰, 프로젝션, 뷰포트 세팅
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
		MsgAssert("카메라 투영이 설정되지 않았습니다.");
		break;
	}
	case CameraType::Perspective:
		ProjectionMatrix.PerspectiveFovLH(FOV, Width / Height, Near, Far);
		break;
	case CameraType::Orthogonal:
		// Projection.OrthographicLH(Width * ZoomRatio, Height * ZoomRatio, Near, Far);
		// 직교는 일단 패스
		break;
	default:
		break;
	}

	ViewPortMatrix.ViewPort(Width, Height, 0.0f, 0.0f);
}

// MeshComponents에 생성된 MeshComponent 넣기
void Ext_Camera::PushMeshComponent(std::shared_ptr<Ext_MeshComponent> _MeshComponent)
{
	if (nullptr == _MeshComponent)
	{
		MsgAssert("MeshComponent가 nullptr 입니다");
		return;
	}

	_MeshComponent->SetOwnerCamera(GetSharedFromThis<Ext_Camera>());
	MeshComponents[GetOrder()].push_back(_MeshComponent);
}

// 생성된 MeshComponentUnit을 카메라의 MeshComponentUnits에 넣기
void Ext_Camera::PushMeshComponentUnit(std::shared_ptr<Ext_MeshComponentUnit> _Unit, RenderPath _Path /*= RenderPath::None*/)
{
	int Order = _Unit->GetOwnerMeshComponent().lock()->GetOrder();
	// RenderPath Path = _Unit->GetMaterial()->GetPixelShader()->GetRenderPath();
	MeshComponentUnits[_Path][Order].push_back(_Unit);

	// 여기서 동적, 정적으로 나눌 수도 있다.
}

// 뷰, 프로젝션행렬을 MeshComponent의 Transform에 적용
void Ext_Camera::MeshComponentTransformUpdate(std::shared_ptr<Ext_Camera> _Camera)
{
	if (nullptr == _Camera)
	{
		MsgAssert("렌더링 하려는데 카메라에 문제있다.");
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

// 카메라의 MeshComponents들에 대한 업데이트 및 렌더링 파이프라인 리소스 정렬
void Ext_Camera::Rendering(float _Deltatime)
{
	Ext_DirectXDevice::GetMainRenderTarget()->RenderTargetClear(); // [1] 메인 렌더 타겟(백 버퍼) Clear 실시, 렌더타겟뷰와 뎁스스텐실뷰를 클리어한다.

	// MeshComponents 렌더링 업데이트 시작
	for (auto& [Key, MeshComponentList] : MeshComponents)
	{
		// [!] 필요하면 ZSort 실시(나중에)

		for (const std::shared_ptr<Ext_MeshComponent>& CurMeshComponent : MeshComponentList)
		{
			if (!CurMeshComponent->GetIsUpdate()) continue;
			else
			{
				MeshComponentTransformUpdate(GetSharedFromThis<Ext_Camera>()); // [2] 현재 MeshComponent에게 카메라의 View, Projection 곱해주기
				// [!] 필요하면 픽셀 셰이더에서 활용할 Value들 업데이트
			}
		}

	}

	// 컴포넌트 메시의 유닛들 순회
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
			Ext_DirectXDevice::GetMainRenderTarget()->RenderTargetSetting(); // [3] 메인 렌더 타겟 세팅(백 버퍼) Setting 실시
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
				Unit->Rendering(_Deltatime); // 예시
			}
		}

		// 6. MeshComponents의 Render(_Deltatime); 호출
		// <<하는것>>
		// 렌더 유닛에 있는 Ext_Device::GetContext()->IASetInputLayout(InputLayOut); 호출
		// 렌더 유닛의 GameEngineMesh에 있는 GameEngineDevice::GetContext()->IASetVertexBuffers(0, 1, &Buffer, &VertexSize, &Offset);, GameEngineDevice::GetContext()->IASetPrimitiveTopology(TOPOLOGY); 호출 [InputAssmbler1]
		// 렌더 유닛의 GameEngineMesh에 있는 GameEngineDevice::GetContext()->IASetIndexBuffer(Buffer, Format, Offset); 호출 [InputAssmbler2]
		// 렌더링 파이프라인의 각 단계 호출, << Ext_Material에서 진행 >>
		// 1. VertexShader :  GameEngineDevice::GetContext()->VSSetShader(ShaderPtr, nullptr, 0);
		// 2. GeometryShader : GameEngineDevice::GetContext()->GSSetShader(ShaderPtr, nullptr, 0); <<없어도 됨>>
		// 3. Resterizer : RasterizerPtr->SetFILL_MODE(FILL_MODE);, GameEngineDevice::GetContext()->RSSetState(CurState);
		// 4. PixelShader : GameEngineDevice::GetContext()->PSSetShader(ShaderPtr, nullptr, 0);
		// 5. OutputMerget : GameEngineDevice::GetContext()->OMSetBlendState(State, nullptr, Mask);, GameEngineDevice::GetContext()->OMSetDepthStencilState(State, 0);
		// 6. 상수버퍼 세팅, 쉐이더리소스헬퍼에서 진행 : GameEngineStructuredBufferSetter, GameEngineSamplerSetter, GameEngineTextureSetter, GameEngineConstantBufferSetter가 있음
		// <<GameEngineConstantBufferSetter>> :	GameEngineDevice::GetContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SettingResources);, memcpy_s(SettingResources.pData, BufferInfo.ByteWidth, _Data, BufferInfo.ByteWidth);, GameEngineDevice::GetContext()->Unmap(Buffer, 0); 호출
		// 그다음에 GameEngineDevice::GetContext()->VSSetConstantBuffers(_Slot, 1, &Buffer);, Res->PSSetting(BindPoint); 실시
		// 7. Draw
		// 렌더유닛에서 실시
		// 	UINT IndexCount = Mesh->IndexBufferPtr->GetIndexCount();
		// GameEngineDevice::GetContext()->DrawIndexed(IndexCount, 0, 0);
	}

	// 끝
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}










// Zsort 로직(저장)
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