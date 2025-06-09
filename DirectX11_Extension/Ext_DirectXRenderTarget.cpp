#include "PrecompileHeader.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXDevice.h"

Ext_MeshComponentUnit Ext_DirectXRenderTarget::MergeUnit;

// MergeUnit 초기화용
void Ext_DirectXRenderTarget::RenderTargetMergeUnitInitialize()
{
	MergeUnit.MeshComponentUnitInitialize("FullRect", MaterialType::Merge);
}

// View를 기반으로 렌더타겟 생성
void Ext_DirectXRenderTarget::CreateRT(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color)
{
	Colors.push_back(_Color);

	// 렌더링할 출력 영역 설정 구조체 정의
	D3D11_VIEWPORT ViewPortData;
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = _Texture->GetScale().x;
	ViewPortData.Height = _Texture->GetScale().y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;
	// <<설명>>
	/*1. TopLeftX : 뷰포트 좌상단 X 위치 (픽셀 단위)*/
	/*2. TopLeftY : 뷰포트 좌상단 Y 위치 (픽셀 단위)*/
	/*3. Width : 뷰포트 너비 (픽셀 단위)*/
	/*4. Height : 뷰포트 높이 (픽셀 단위)*/
	/*5. MinDepth : 깊이값 최소 (보통 0.0f)*/
	/*6. MaxDepth : 깊이값 최대 (보통 1.0f)*/

	Textures.push_back(_Texture);
	ViewPorts.push_back(ViewPortData);
	RTVs.push_back(_Texture->GetRTV());
	SRVs.push_back(_Texture->GetSRV());
}

// 텍스쳐를 생성해서 렌더타겟 생성
void Ext_DirectXRenderTarget::CreateRT(DXGI_FORMAT _Format, float4 _Scale, float4 _Color)
{
	Colors.push_back(_Color);

	D3D11_TEXTURE2D_DESC Desc = { 0 };
	Desc.ArraySize = 1;
	Desc.Width = _Scale.uix();
	Desc.Height = _Scale.uiy();
	Desc.Format = _Format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.MipLevels = 1;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

	std::shared_ptr<Ext_DirectXTexture> Tex = Ext_DirectXTexture::CreateViews(Desc);

	D3D11_VIEWPORT ViewPortData;
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = _Scale.x;
	ViewPortData.Height = _Scale.y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	ViewPorts.push_back(ViewPortData);
	Textures.push_back(Tex);
	RTVs.push_back(Tex->GetRTV());
	SRVs.push_back(Tex->GetSRV());
}

// 뎁스텍스쳐 생성(기본 형식)
void Ext_DirectXRenderTarget::CreateDepthTexture(int _Index)
{
	D3D11_TEXTURE2D_DESC Desc = { 0, };

	Desc.ArraySize = 1;
	Desc.Width = Textures[_Index]->GetWidth();
	Desc.Height = Textures[_Index]->GetHeight();
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.MipLevels = 1;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	// <설명>
	/*1. 	D3D11_TEXTURE2D_DESC 구조체에서 해당 텍스처가 몇 개의 배열 요소(Array Slice)로 구성되어 있는지를 나타낸다. 1은 단일 2D 텍스처라는 뜻*/
	/*2. 뎁스 텍스쳐 가로(렌더 타겟 크기와 통일하게)*/
	/*3. 뎁스 텍스쳐 세로(렌더 타겟 크기와 통일하게)*/
	/*4. 4byte(32bit)중 상위 24bit는 깊이값(0.0 ~ 1.0), 하위 8bit는 스텐실 값(0~255)으로 활용*/
	/*5. 멀티 샘플링(안티앨리어싱) 설정, Count = 1은 비활성화라는 뜻*/
	/*6. 멀티 샘플링(안티앨리어싱) 설정, Quality = 0은 기본 품질 수준 사용한다는 뜻*/
	/*7. 텍스쳐 밉맵 수준 설정, Depth Texture는 보통 한 레벨만 필요해서 1을 넣음*/
	/*8. 리소스 사용 방식 설정, D3D11_USAGE_DEFAULT는 GPU에서 읽고 쓰는 일반적인 리소스 타입*/
	/*9. 해당 텍스처가 어떤 용도로 바인딩될 수 있는지 설정한다. */

	DepthTexture = Ext_DirectXTexture::CreateViews(Desc); // 뎁스스텐실뷰 생성
	// 깊이 텍스쳐는 
	// 1. 다른 물체에 가려지면 렌더링 되지 않도록 하기 위한 깊이 버퍼(0.0 ~ 1.0이며 1.0이 제일 먼 물체)를 통해 가장 가까운 픽셀 렌더링
	// 2. 특정 상황에서는 깊이 버퍼로만 물체를 렌더링 할 것인지 판단하기 어렵기 때문에(특정 영역 렌더링 막기(스텐실 버퍼값이 채워져있으면 그려지고, 비워지면 안그리는)) 쓰기 위한 스텐실 버퍼 활용
	//   2-1. 예를 들어 거울이 있는 방을 만들 때, 거울 부분의 스텐실 버퍼를 비우는 것
	// 위의 두 가지 목적을 위하여 생성한다.
}

// Textures에 저장된 렌더 타겟 뷰들을 모두 클리어
void Ext_DirectXRenderTarget::RenderTargetViewsClear()
{
	for (size_t i = 0; i < Textures.size(); i++) 
	{
		for (size_t j = 0; j < Textures[i]->GetRTVSize(); j++)
		{
			COMPTR<ID3D11RenderTargetView> RTV = Textures[i]->GetRTV(j); // 하나의 텍스쳐가 여러 개를 가지고 있을 수 있음

			if (nullptr == RTV)
			{
				MsgAssert("존재하지 않는 랜더타겟뷰를 클리어할 수는 없음");
				return;
			}

			Ext_DirectXDevice::GetContext()->ClearRenderTargetView(RTV.Get(), Colors[i].Arr1D); // 기본 컬러(파란색)으로 클리어
			// 1. 클리어 대상
			// 2. 무슨 색으로 클리어 할것인가
		}
	}
}

// 렌더링 시작 시 깊이 및 스텐실 버퍼를 초기화하여, 이전 프레임에 남아있는 데이터 제거
// 메인 렌더타겟 외에는 아마 뎁스 안만들어서 이렇게 해야할듯
void Ext_DirectXRenderTarget::DepthStencilViewClear()
{
	COMPTR<ID3D11DepthStencilView> DSV = DepthTexture != nullptr ? DepthTexture->GetDSV() : nullptr;

	if (nullptr != DSV)
	{
		Ext_DirectXDevice::GetContext()->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	// 1. 클리어 대상
	// 2. 어떻게 클리어 할것인가
	// 3. 깊이 클리어값(가장 멀리)
	// 4. 스텐실 클리어값(일반적인 0을 넣음)
}

// 렌더타겟뷰, 뎁스스텐실뷰 클리어
void Ext_DirectXRenderTarget::RenderTargetClear()
{
	RenderTargetViewsClear();
	DepthStencilViewClear();
}

// 렌더 타겟 바인딩 담당, Draw(), Clear(), Shader Binding 등이 올바른 렌더 타겟에 수행될 수 있도록 설정
void Ext_DirectXRenderTarget::RenderTargetSetting()
{
	COMPTR<ID3D11RenderTargetView> RTV = Textures[0]->GetRTV(0); // 첫 번째 RTV(RenderTargetView) 를 가져옴(백 버퍼임)

	if (nullptr == RTV)
	{
		MsgAssert("랜더타겟 뷰가 존재하지 않아서 클리어가 불가능합니다.");
	}

	COMPTR<ID3D11DepthStencilView> DSV = DepthTexture != nullptr ? DepthTexture->GetDSV() : nullptr;

	if (false == DepthSetting)
	{
		DSV = nullptr;
	}
	
	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(static_cast<UINT>(RTVs.size()), RTV.GetAddressOf(), DSV.Get()); // Output-Merger 스테이지에 렌더 타겟 + 뎁스 설정
	// 1. 바인딩할 렌더타겟뷰 갯수
	// 2. RTV의 시작 주소
	// 3. 깊이/스텐실 뷰 포인터(딱히 없으면nullptr 가능)
	Ext_DirectXDevice::GetContext()->RSSetViewports(static_cast<UINT>(ViewPorts.size()), &ViewPorts[0]); // Rasterizer 스테이지에 현재 프레임에서 사용할 ViewPort 영역 설정, 이게 있어야 NDC > 픽셀 공간 변환이 올바르게 수행됨
}

// 렌더 타겟 바인딩 담당, Draw(), Clear(), Shader Binding 등이 올바른 렌더 타겟에 수행될 수 있도록 설정
void Ext_DirectXRenderTarget::RenderTargetSetting(size_t _Index)
{
	COMPTR<ID3D11RenderTargetView> RTV = Textures[0]->GetRTV(_Index);

	if (nullptr == RTV)
	{
		MsgAssert("랜더타겟 뷰가 존재하지 않아서 클리어가 불가능합니다.");
	}

	COMPTR<ID3D11DepthStencilView> DSV = DepthTexture != nullptr ? DepthTexture->GetDSV() : nullptr;

	if (false == DepthSetting)
	{
		DSV = nullptr;
	}

	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	Ext_DirectXDevice::GetContext()->RSSetViewports(static_cast<UINT>(ViewPorts.size()), &ViewPorts[0]);
}

void Ext_DirectXRenderTarget::RenderTargetReset()
{
	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

// 렌더타겟끼리 합치기
void Ext_DirectXRenderTarget::Merge(std::shared_ptr<Ext_DirectXRenderTarget> _OtherRenderTarget, size_t _Index /*= 0*/)
{
	RenderTargetSetting(); // Merge를 호출한 RenderTarget에 그림

	MergeUnit.BufferSetter.SetTexture(_OtherRenderTarget->Textures[_Index], "DiffuseTex"); // 텍스쳐슬롯의 텍스쳐 값을 변경
	MergeUnit.Rendering(0.0f); // MergeUnit의 렌더링 파이프라인 진행 후 드로우콜
	MergeUnit.BufferSetter.AllTextureResourceReset(); // MergeUnit은 전역 변수라서 이거로 비워줌(다른곳에서 쓰게)
}