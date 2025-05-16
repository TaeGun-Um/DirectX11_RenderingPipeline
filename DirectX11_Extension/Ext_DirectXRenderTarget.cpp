#include "PrecompileHeader.h"
#include "Ext_DirectXRenderTarget.h"

// View를 기반으로 렌더타겟 생성
void Ext_DirectXRenderTarget::CreateRenderTarget(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color)
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

	ViewPorts.push_back(ViewPortData); // D3D11_VIEWPORT 저장
	Textures.push_back(_Texture); // Ext_DirectXTexture 저장
	SRVs.push_back(_Texture->GetSRV()); // ID3D11ShaderResourceView 저장
	RTVs.push_back(_Texture->GetRTV()); // ID3D11RenderTargetView 저장
}

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

