#pragma once

#include <DirectXTex.h>
#include <DirectX11_Base/Base_Directory.h>
#include "Ext_ResourceManager.h"

// DirectX의 Texture2D와 View(RTV, SRV, DSV)의 생성 및 저장, 관리 클래스
class Ext_DirectXTexture : public Ext_ResourceManager<Ext_DirectXTexture>
{
	friend class Ext_DirectXDevice;

public:
	// constrcuter destructer
	Ext_DirectXTexture() {};
	~Ext_DirectXTexture() {};

	// delete Function
	Ext_DirectXTexture(const Ext_DirectXTexture& _Other) = delete;
	Ext_DirectXTexture(Ext_DirectXTexture&& _Other) noexcept = delete;
	Ext_DirectXTexture& operator=(const Ext_DirectXTexture& _Other) = delete;
	Ext_DirectXTexture& operator=(Ext_DirectXTexture&& _Other) noexcept = delete;

	// Texture2DInfo의 BindFlags에 따라 RTV, SRV, DSV 중 하나를 Create
	static std::shared_ptr<Ext_DirectXTexture> CreateViews(const D3D11_TEXTURE2D_DESC& _Texture2DInfo)
	{
		std::shared_ptr<Ext_DirectXTexture> NewTexture = Ext_ResourceManager::CreateResource();
		NewTexture->CreateView(_Texture2DInfo);
		return NewTexture;
	}

	// 텍스쳐 로드
	static std::shared_ptr<Ext_DirectXTexture> LoadTexture(std::string_view _Path)
	{
		std::string ResourceName = Base_Directory::GetFileName(_Path);
		std::string ExtensionName = Base_Directory::GetExtension(_Path);
		ResourceName += ExtensionName;

		std::string UpperName = Base_String::ToUpper(ResourceName);
		std::shared_ptr<Ext_DirectXTexture> NewTexture = Ext_ResourceManager::CreateNameResource(UpperName);
		NewTexture->TextureLoad(_Path, ExtensionName.c_str());

		return NewTexture;
	}

	// 렌더타겟 생성 시 호출(지금은 백버퍼 생성시에만 호출됨)
	void CreateRenderTargetView(COMPTR<ID3D11Texture2D>& _Texture); 

	// Getter, Setter
	int GetWidth() {	return Texture2DInfo.Width; }
	int GetHeight() { return Texture2DInfo.Height; }
	size_t GetRTVSize() {	return RTVs.size(); } // 렌더타겟뷰 클리어용
	float4 GetScale() {	return float4(static_cast<float>(Texture2DInfo.Width), static_cast<float>(Texture2DInfo.Height)); }
	COMPTR<ID3D11Texture2D>& GetTexture2D() { return Texture2D; }
	COMPTR<ID3D11RenderTargetView>& GetRTV(size_t _Index = 0) { return RTVs[_Index]; }
	COMPTR<ID3D11ShaderResourceView>& GetSRV() { return SRV; }
	COMPTR<ID3D11DepthStencilView>& GetDSV() { return DSV; }

	// Texture 세팅, Texture를 로드해서 사용할 경우, 버퍼 세터를 통해 세팅되고, 아래 함수들 호출하여 사용
	void VSSetting(UINT _Slot);
	void PSSetting(UINT _Slot);
	void VSReset(UINT _Slot);
	void PSReset(UINT _Slot);

	// 큐브맵 텍스쳐 만들기, 이름 지정해주고 저장 및 로드
	static std::shared_ptr<Ext_DirectXTexture> LoadCubeMap(std::string_view _TextureName, std::vector<std::string>& _Paths)
	{
		std::string UpperName = Base_String::ToUpper(_TextureName.data());
		std::shared_ptr<Ext_DirectXTexture> NewTexture = Ext_ResourceManager::CreateNameResource(UpperName);
		NewTexture->CubeMapLoad(_Paths);

		return NewTexture;
	}

	static void LoadCubeMap(std::string_view _TextureName)
	{
		
	}

protected:
	
private:
	void CreateView(const D3D11_TEXTURE2D_DESC& _Texture2DInfo); // Desc의 BindFlags에 따라 RTV, SRV, DSV 중 하나를 Create
	void CreateRenderTargetView(); // 렌더타겟뷰 생성
	void CreateDepthStencilView(); // 뎁스스텐실뷰 생성
	void CreateShaderResourcesView(); // 쉐이더리소스뷰 생성

	void TextureLoad(std::string_view _Path, std::string_view _ExtensionName); // 텍스쳐 로드

	D3D11_TEXTURE2D_DESC Texture2DInfo;
	COMPTR<ID3D11Texture2D> Texture2D;
	
	std::vector<COMPTR<ID3D11RenderTargetView>> RTVs; // 생성된 렌더타겟뷰들을 저장하는 컨테이너
	COMPTR<ID3D11ShaderResourceView> SRV; // 생성된 쉐이더 리소스 뷰
	COMPTR<ID3D11DepthStencilView> DSV; // 생성된 뎁스스텐실 뷰
	std::vector<float4> Colors;

	// Texture 로드용
	DirectX::ScratchImage Image;
	DirectX::TexMetadata TexData;

	// 큐브맵 만들기
	void CubeMapLoad(std::vector<std::string>& _Paths); // 큐브맵 로드
	D3D11_SHADER_RESOURCE_VIEW_DESC CubeSRVDesc = {};

};
// [ID3D11Texture2D]
// 2D 텍스처를 나타내는 인터페이스, GPU에 올라가는 2차원 이미지(텍스처), 렌더타겟, 뎁스 버퍼 등의 자원을 표현하는데 사용

// [D3D11_TEXTURE2D_DESC]
// 2D 텍스쳐를 생성할 때 사용하는 구조체로 텍스쳐 해상도, 포맷, 샘플링 정보, BindFlags(사용 용도) 등을 담고 있음
// <<설명>>
/*1. Width : 텍스처의 가로 해상도(픽셀 단위)*/ 
/*2. Height : 텍스처의 세로 해상도(픽셀 단위)*/
/*3. MipLevels : 생성할 miplevel 수, 0이면 Direct3D가 모든 밉맵 자동 생성*/
/*4. ArraySize : 텍스처 배열의 개수(배열 텍스처 또는 큐브맵의 6면 등)*/
/*5. Format : 픽셀 포멧(DXGI_FORMAT_R8G8B8A8_UNORM)*/
/*6. SampleDesc : 멀티샘플링의 정보(MSAA), Count, Quality 등이 담겨있음*/
/*7.Usage : 텍스처 사용 패턴(DEFAULT, IMMUTABLE, DYNAMIC, STAGING)*/
/*8. BindFlags : 바인딩 가능한 형태(RENDER_TARGET, DEPTH_STENCIL, SHADER_RESOURCE)*/
/*9. CPUAccessFlags : CPU가 접근할 권한(보통 DYNAMIC이나 STAGING에서 사용)*/
/*10. MiscFlags : 기타 플래그 (TEXTURECUBE, GENERATE_MIPS 등)*/

// [ID3D11RenderTargetView] 
// GPU가 픽셀 출력을 기록할 수 있도록 설정된 2D 텍스쳐(그림판) 역할을 하는 뷰(View)이다.
// 디바이스의 백버퍼에 바운딩되면서 생성된다.
// 백 버퍼에 모든 화면을 렌더링하고 난 다음에는 SwapChain을 통해 백버퍼를 화면에 그리면 되는 것이다.
// <<역할>>
/*1. 렌더 타겟 연결 : GPU 파이프라인의 렌더 타겟 슬롯에 연결되어, 픽셀 셰이더의 출력이 여기에 기록됨*/  
/*2. View : 텍스처 리소스(ID3D11Texture2D)의 일부분 또는 전체를 GPU가 렌더 타겟으로 사용할 수 있게 해주는 접근 지정*/
/*3. Output : 최종 화면 출력용 백버퍼(스왑 체인 텍스처) 또는 다른 텍스처에 대한 출력을 목표로 함*/

// [ID3D11ShaderResourceView]
// Shader가 GPU 자원(리소스)에 접근할 수 있도록(읽을 수 있도록) 해주는 뷰(View)이다.
// 주로 텍스처, 버퍼 등의 리소스를 셰이더에서 접근 가능하게 만드는 역할을 수행한다.
// <<역할>>
/*1. 셰이더 리소스 접근 : 셰이더(픽셀 셰이더, 버텍스 셰이더 등)에서 텍스처나 버퍼 데이터를 읽기 위해 필요*/
/*2. 리소스 View : 원본 리소스(예: ID3D11Texture2D)의 특정 범위나 포맷을 셰이더에서 읽을 수 있도록 정의*/
/*3. 사용처 : 텍스처 샘플링, 버퍼 참조, G-Buffer 접근, 후처리(PP) 효과 등*/

// [ID3D11DepthStencilView]
// 깊이(Depth)와 스텐실(Stencil) 정보를 읽고 쓸 수 있도록 해주는 뷰(View)이다.
// 쉽게 말해, 깊이 버퍼(Depth Buffer)나 스텐실 버퍼(Stencil Buffer)로 사용할 수 있는 텍스처에 대한 접근 인터페이스이다.
// 렌더링 과정에서 GPU가 픽셀을 그릴 때, 두 가지 주요한 테스트(Depth Test, Stencil Test)를 수행할 수도 있다.
/*1. Depth Test (깊이 테스트) : 이미 그려진 픽셀보다 가까운지 판단해 렌더링할지 결정*/ 
/*2. Stencil Test (스텐실 테스트) : 지정된 조건을 만족하는 경우에만 픽셀을 렌더링*/
