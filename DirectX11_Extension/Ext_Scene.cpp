#include "PrecompileHeader.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"

#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Camera.h"
#include "Ext_Transform.h"

#include "Ext_DirectXVertexShader.h" // 임시
#include "Ext_DirectXPixelShader.h" // 임시
#include "Ext_DirectXShader.h"
#include "Ext_DirectXConstantBuffer.h"

Ext_Scene::Ext_Scene()
{
	
}

Ext_Scene::~Ext_Scene()
{
}

// Actor 생성 시 자동 호출, 이름 설정, Owner설정
void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, std::string_view _Name, int _Order /*= 0*/)
{
	_Actor->SetName(_Name);
	_Actor->SetOwnerScene(_Level);
	_Actor->SetOrder(_Order);
	_Actor->Start();
}

// 카메라 찾아서 그 카메라에 MeshComponent 저장
void Ext_Scene::PushMeshToCamera(std::shared_ptr<Ext_MeshComponent> _MeshComponent, std::string_view _CameraName)
{
	std::shared_ptr<Ext_Camera> FindCam = FindCamera(_CameraName);

	if (nullptr == FindCam)
	{
		MsgAssert("존재하지 않는 카메라에 랜더러를 넣을수는 없습니다.");
		return;
	}

	FindCam->PushMeshComponent(_MeshComponent);
}

// 이름으로 카메라 찾기
std::shared_ptr<Ext_Camera> Ext_Scene::FindCamera(std::string_view _CameraName)
{
	std::map<std::string, std::shared_ptr<Ext_Camera>>::iterator FindIter = Cameras.find(_CameraName.data());

	if (FindIter == Cameras.end())
	{
		return nullptr;
	}

	std::shared_ptr<Ext_Camera> FindCam = FindIter->second;

	return FindCam;
}

void Ext_Scene::SceneChangeInitialize()
{

}

void Ext_Scene::SceneChangeEnd()
{

}

void Ext_Scene::Start()
{
	
}

void Ext_Scene::Update(float _DeltaTime)
{
	for (auto& [Key, ActorList] : Actors)
	{
		for (const std::shared_ptr<Ext_Actor>& CurActor : ActorList)
		{
			if (false == CurActor->GetIsDeath())
			{
				CurActor->Update(_DeltaTime);
			}
		}
	}
}

void Ext_Scene::Rendering()
{
	// Rendering 업데이트
	for (auto& Iter : Cameras)
	{
		std::shared_ptr<Ext_Camera> CurCamera = Iter.second;
		if (false == CurCamera->GetIsDeath())
		{
			CurCamera->CameraTransformUpdate();
		}
	}

	RenderTest();
}

void Ext_Scene::RenderTest()
{
	// 1. 메인 렌더 타겟 가져오기
	std::shared_ptr<Ext_DirectXRenderTarget> MainRenderTarget = Ext_DirectXDevice::GetMainRenderTarget();

	// 2. 렌더 타겟 및 뷰포트 바인딩
	COMPTR<ID3D11RenderTargetView> RTV = MainRenderTarget->GetTexture(0)->GetRTV();
	COMPTR<ID3D11DepthStencilView> DSV = MainRenderTarget->GetDepthTexture()->GetDSV();
	D3D11_VIEWPORT* ViewPort = MainRenderTarget->GetViewPort(0);

	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, ViewPort);

	// 3. 렌더 타겟 및 뎁스 클리어
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // 파란색
	Ext_DirectXDevice::GetContext()->ClearRenderTargetView(RTV.Get(), ClearColor);
	Ext_DirectXDevice::GetContext()->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 4. ==========렌더링==============
	std::shared_ptr<Ext_DirectXVertexBuffer> VB = Ext_DirectXVertexBuffer::Find("Rect");
	std::shared_ptr<Ext_DirectXIndexBuffer> IB = Ext_DirectXIndexBuffer::Find("Rect");
	COMPTR<ID3D11Buffer>& VertexBuffer = VB->GetVertexBuffer();
	UINT stride = VB->GetVertexSize();
	UINT Offset = 0;

	assert(VB != nullptr && "Rect VB가 없습니다.");
	assert(IB != nullptr && "Rect IB가 없습니다.");

	Setter CBTransformSetter;

	// 상수 버퍼에서 "TRANSFORMDATA" 찾기
	auto Range = Ext_DirectXShader::ConstantBufferSetters.equal_range("TRANSFORMDATA");
	for (auto Iter = Range.first; Iter != Range.second; ++Iter)
	{
		CBTransformSetter = Iter->second;
		break; // 첫 번째만 사용
	}

	// CBTransform 구조체 채우기
	CBTransform TransformData;

	for (auto& [Key, ActorList] : Actors)
	{
		for (const std::shared_ptr<Ext_Actor>& CurActor : ActorList)
		{
			if (false == CurActor->GetIsDeath())
			{
				if ("RectActor" == CurActor->GetName())
				{
					TransformData.WorldMatrix = CurActor->GetTransform()->GetWorldMatrix();
					float4 Temp = CurActor->GetTransform()->GetWorldPosition();
				}
			}
		}
	}


	TransformData.ViewMatrix = MainCamera->GetViewMatrix();
	TransformData.ProjectionMatrix = MainCamera->GetProjectionMatrix();

	TransformData.WorldMatrix.Transpose();
	TransformData.ViewMatrix.Transpose();
	TransformData.ProjectionMatrix.Transpose();

	// GPU 상수 버퍼에 데이터 업데이트
	ID3D11Buffer* Buffer = CBTransformSetter.Res->GetConstantBuffer().Get(); // 버퍼 포인터 획득

	// GPU 상수 버퍼에 업데이트
	D3D11_MAPPED_SUBRESOURCE Mapped = {};
	Ext_DirectXDevice::GetContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	memcpy(Mapped.pData, &TransformData, sizeof(CBTransform));
	Ext_DirectXDevice::GetContext()->Unmap(Buffer, 0);
	CBTransformSetter.Res->VSSetting(0); // ? 바인딩
	CBTransformSetter.Res->PSSetting(0); // ? 바인딩

	Ext_DirectXDevice::GetContext()->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &Offset);
	Ext_DirectXDevice::GetContext()->IASetIndexBuffer(IB->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Ext_DirectXDevice::GetContext()->IASetInputLayout(Ext_DirectXResourceLoader::InputLayOut);

	std::shared_ptr<Ext_DirectXVertexShader> VS = Ext_DirectXVertexShader::Find("Basic_VS");
	std::shared_ptr<Ext_DirectXPixelShader> PS = Ext_DirectXPixelShader::Find("Basic_PS");

	Ext_DirectXDevice::GetContext()->VSSetShader(VS->GetVertexShader(), nullptr, 0);
	// Ext_DirectXDevice::GetContext()->VSSetConstantBuffers(0, 1, &Buffer);
	Ext_DirectXDevice::GetContext()->PSSetShader(PS->GetPixelShader(), nullptr, 0);

	Ext_DirectXDevice::GetContext()->DrawIndexed(IB->GetVertexCount(), 0, 0);

	// 5. 화면 출력
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}