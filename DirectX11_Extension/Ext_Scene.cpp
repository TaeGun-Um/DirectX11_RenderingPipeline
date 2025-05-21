#include "PrecompileHeader.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"

#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXResourceLoader.h"

Ext_Scene::Ext_Scene()
{
}

Ext_Scene::~Ext_Scene()
{
}

void Ext_Scene::SetActorName(std::shared_ptr<Ext_Actor> _Actor, std::string_view _Name)
{
	_Actor->SetName(_Name);
}

void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, int _Order)
{
	_Actor->SetOwnerScene(_Level);
	_Actor->Start();
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
	for (auto& Iter : Actors)
	{
		std::shared_ptr<Ext_Actor> CurActor = Iter.second;
		if (false == CurActor->GetIsDeath())
		{
			CurActor->Update(_DeltaTime);
		}
	}
}

void Ext_Scene::Rendering(float _DeltaTime)
{
	// Rendering ¾÷µ¥ÀÌÆ®
	RenderTest();
}

void Ext_Scene::Destroy()
{

}

void Ext_Scene::RenderTest()
{
	// 1. ¸ÞÀÎ ·»´õ Å¸°Ù °¡Á®¿À±â
	std::shared_ptr<Ext_DirectXRenderTarget> MainRenderTarget = Ext_DirectXDevice::GetMainRenderTarget();

	// 2. ·»´õ Å¸°Ù ¹× ºäÆ÷Æ® ¹ÙÀÎµù
	COMPTR<ID3D11RenderTargetView> RTV = MainRenderTarget->GetTexture(0)->GetRTV();
	COMPTR<ID3D11DepthStencilView> DSV = MainRenderTarget->GetDepthTexture()->GetDSV();
	D3D11_VIEWPORT* ViewPort = MainRenderTarget->GetViewPort(0);

	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, ViewPort);

	// 3. ·»´õ Å¸°Ù ¹× µª½º Å¬¸®¾î
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // ÆÄ¶õ»ö
	Ext_DirectXDevice::GetContext()->ClearRenderTargetView(RTV.Get(), ClearColor);
	Ext_DirectXDevice::GetContext()->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 4. ==========·»´õ¸µ==============
	std::shared_ptr<Ext_DirectXVertexBuffer> VB = Ext_DirectXVertexBuffer::Find("Triangle");
	std::shared_ptr<Ext_DirectXIndexBuffer> IB = Ext_DirectXIndexBuffer::Find("Triangle");
	COMPTR<ID3D11Buffer>& VertexBuffer = VB->GetVertexBuffer();
	UINT stride = VB->GetVertexSize();
	UINT Offset = 0;

	Ext_DirectXDevice::GetContext()->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &Offset);
	Ext_DirectXDevice::GetContext()->IASetIndexBuffer(IB->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	Ext_DirectXDevice::GetContext()->IASetInputLayout(Ext_DirectXResourceLoader::GetInputLayout().Get());
	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Ext_DirectXDevice::GetContext()->VSSetShader(Ext_DirectXResourceLoader::GetVertexShader(), nullptr, 0);
	Ext_DirectXDevice::GetContext()->PSSetShader(Ext_DirectXResourceLoader::GetPixelShader(), nullptr, 0);

	Ext_DirectXDevice::GetContext()->DrawIndexed(IB->GetVertexCount(), 0, 0);
	// ==========·»´õ¸µ ³¡==============

	// 5. È­¸é Ãâ·Â
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}