#pragma once

#include "Ext_DirectXTexture.h"
#include "Ext_ResourceManager.h"

// 렌더 타겟 관리 클래스
class Ext_DirectXRenderTarget : public Ext_ResourceManager<Ext_DirectXRenderTarget>
{
public:
	// constrcuter destructer
	Ext_DirectXRenderTarget() {};
	~Ext_DirectXRenderTarget() {};

	// delete Function
	Ext_DirectXRenderTarget(const Ext_DirectXRenderTarget& _Other) = delete;
	Ext_DirectXRenderTarget(Ext_DirectXRenderTarget&& _Other) noexcept = delete;
	Ext_DirectXRenderTarget& operator=(const Ext_DirectXRenderTarget& _Other) = delete;
	Ext_DirectXRenderTarget& operator=(Ext_DirectXRenderTarget&& _Other) noexcept = delete;

	// 렌더타겟 생성, [1] 렌더타겟 이름 지정, [2] Ext_DirectXTexture 포인터, [3] 렌더타겟 색상
	static std::shared_ptr<Ext_DirectXRenderTarget> CreateRenderTarget(std::string_view _Name, std::shared_ptr<class Ext_DirectXTexture> _Texture, const float4& _Color)
	{
		std::shared_ptr<Ext_DirectXRenderTarget> NewRenderTarget = Ext_ResourceManager::CreateNameResource(_Name);
		NewRenderTarget->CreateRenderTarget(_Texture, _Color);
		return NewRenderTarget;
	}

	void CreateDepthTexture(int _Index = 0);

	// Getter, Setter
	std::shared_ptr<Ext_DirectXTexture> GetTexture(int _Index) { return Textures[_Index]; }
	std::shared_ptr<Ext_DirectXTexture> GetDepthTexture() { return DepthTexture; }
	D3D11_VIEWPORT* GetViewPort(int _Index) { return &ViewPorts[_Index]; }

protected:
	
private:
	void CreateRenderTarget(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color); // View를 기반으로 렌더타겟 생성

	std::vector<float4> Colors; // 생성된 렌더타겟 색상 저장
	std::vector<std::shared_ptr<Ext_DirectXTexture>> Textures = {}; // Ext_DirectXTexture(생성 주체) 포인터 저장 컨테이너
	std::shared_ptr<Ext_DirectXTexture> DepthTexture = {};
	std::vector<D3D11_VIEWPORT> ViewPorts = {}; // 생성 주체의 ViewPort 정보 저장 컨테이너

	// std::vector<ID3D11RenderTargetView*> RTVs = {}; // 생성 주체의 렌더타겟뷰 정보 저장 컨테이너
	// std::vector<ID3D11ShaderResourceView*> SRVs = {}; // 생성 주체의 셰이더리소스뷰 정보 저장 컨테이너
};
// [RenderTarget]
// 렌더 타겟은 렌더링 결과를 기록하는 최종 목적지가 된다. 즉, 셰이더의 출력 결과가 그려지는 공간이다.
// 일반적으로 화면에 출력되는 프레임 버퍼(백 버퍼, 프론트 버퍼)나 후처리용 텍스쳐 등이 렌더 타겟이 된다.
// 주로 ID3D11Texture2D의 형태가 된다.

// [D3D11_VIEWPORT]
// 렌더링할 출력 영역(ViewPort)을 정의하는 구조체이다. 픽셀이 실제로 그려질 화면상의 사각형 영역을 설정하는데 사용된다.
// <<설명>>
/*1. TopLeftX : 뷰포트 좌상단 X 위치 (픽셀 단위)*/
/*2. TopLeftY : 뷰포트 좌상단 Y 위치 (픽셀 단위)*/
/*3. Width : 뷰포트 너비 (픽셀 단위)*/
/*4. Height : 뷰포트 높이 (픽셀 단위)*/
/*5. MinDepth : 깊이값 최소 (보통 0.0f)*/
/*6. MaxDepth : 깊이값 최대 (보통 1.0f)*/

// [std::shared_ptr<Ext_DirectXTexture> DepthTexture]
// DepthTexture는 GPU가 깊이(Depth), 스탠실(Stencil) 정보를 저장하는 특수한 2D 텍스쳐이다.
// 일반 텍스처처럼 색상을 저장하는 것이 아니라, 각 픽셀이 얼마나 카메라에 멀리 있는지를 부동 소수값으로 저장한다.
// 렌더링 시 깊이 테스트(Z-test)에 사용된다.
// <<깊이 테스트>>
// 1. GPU는 화면에 픽셀을 찍기 전, 기존 픽셀보다 더 가까운지 확인해야 함
// 2. 그 깊이 비교를 위해 Depth Buffer(여기서의 DepthTexture)가 필요
// <<스텐실 테스트>>
// 마스킹, 클리핑 효과를 만들 때 사용(실루엣, 그림자 영역 등)
