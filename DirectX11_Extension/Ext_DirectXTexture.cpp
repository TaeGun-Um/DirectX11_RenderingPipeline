#include "PrecompileHeader.h"
#include "Ext_DirectXTexture.h"
#include "Ext_DirectXDevice.h"

// 백버퍼 렌더타겟뷰 생성을 위해 호출됨
void Ext_DirectXTexture::CreateRenderTargetView(COMPTR<ID3D11Texture2D>& _Texture)
{
	Texture2D = _Texture;
	Texture2D->GetDesc(&Texture2DInfo); // Texture2DInfo(D3D11_TEXTURE2D_DESC)에 값 복사
	CreateRenderTargetView();
}

// Desc의 BindFlags에 따라 RTV, SRV, DSV 중 하나를 Create
void Ext_DirectXTexture::CreateView(const D3D11_TEXTURE2D_DESC& _Texture2DInfo)
{
	Texture2DInfo = _Texture2DInfo;

	Ext_DirectXDevice::GetDevice()->CreateTexture2D(&Texture2DInfo, nullptr, Texture2D.GetAddressOf());

	if (D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET & Texture2DInfo.BindFlags)
	{
		CreateRenderTargetView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE & Texture2DInfo.BindFlags)
	{
		CreateShaderResourcesView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL & Texture2DInfo.BindFlags)
	{
		CreateDepthStencilView();
	}

	if (nullptr == Texture2D)
	{
		MsgAssert("텍스처 생성에 실패했습니다.");
	}
}

// 랜더타겟뷰(RTV) 생성, CreateRenderTargetView()
void Ext_DirectXTexture::CreateRenderTargetView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("텍스처가 존재하지 않는 랜더타겟뷰을 만들 수는 없습니다.");
		return;
	}

	COMPTR<ID3D11RenderTargetView> NewRTV = nullptr;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRenderTargetView(Texture2D, nullptr, NewRTV.GetAddressOf()))
	{
		MsgAssert("랜더타겟 뷰 생성에 실패했습니다.");
		return;
	}
	// <<설명>> CreateRenderTargetView()의 전달 인자
	/*1. 렌더 타겟으로 사용할 2D 텍스처 리소스*/
	/*2. 렌더타겟뷰를 사용하여 액세스 할 수 있는 리소스들을 지정하는 구조체, 지정이 필요없어 nullptr 전달*/
	/*3. 생성된 RTV 객체를 받을 포인터*/

	RTVs.push_back(NewRTV);
}

// 뎁스스텐실뷰(DSV) 생성, CreateDepthStencilView()
void Ext_DirectXTexture::CreateDepthStencilView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("텍스처가 존재하지 않는데 뎁스 스텐실 뷰 만들 수는 없습니다.");
		return;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateDepthStencilView(Texture2D, nullptr, DSV.GetAddressOf()))
	{
		MsgAssert("뎁스 스텐실 뷰 생성에 실패했습니다.");
		return;
	}
	// <<설명>> CreateRenderTargetView()의 전달 인자
	/*1. 깊이/스텐실 정보를 저장할 텍스처*/
	/*2. 기본 뷰 설정 사용*/
	/*3. 생성된 DSV 객체 포인터*/
}

// 쉐이더리소스뷰(SRV) 생성, CreateShaderResourceView()
void Ext_DirectXTexture::CreateShaderResourcesView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("텍스처가 존재하지 않는 쉐이더 리소스 뷰를 만들 수는 없습니다.");
		return;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateShaderResourceView(Texture2D, nullptr, SRV.GetAddressOf()))
	{
		MsgAssert("쉐이더 리소스 뷰 생성에 실패했습니다.");
		return;
	}
	// <<설명>> CreateRenderTargetView()의 전달 인자
	/*1. 셰이더에서 읽을 2D 텍스처*/
	/*2. 전체 텍스처를 기본 설정으로 접근*/
	/*3. 생성된 SRV를 저장할 포인터*/
}

void Ext_DirectXTexture::TextureLoad(std::string_view _Path, std::string_view _ExtensionName)
{
	std::wstring Path = Base_String::AnsiToUniCode(_Path);
	std::string Extention = Base_String::ToUpper(_ExtensionName.data());

	if (Extention == ".TGA")
	{
		if (S_OK != DirectX::LoadFromTGAFile(Path.c_str(), DirectX::TGA_FLAGS_NONE, &TexData, Image))
		{
			MsgAssert("TGA 포멧 텍스쳐 로드 실패" + std::string(_Path.data()));
		}
	}
	else if (Extention == ".DDS")
	{
		if (S_OK != DirectX::LoadFromDDSFile(Path.c_str(), DirectX::DDS_FLAGS_NONE, &TexData, Image))
		{
			MsgAssert("DDS 포멧 텍스쳐 로드 실패" + std::string(_Path.data()));
		}
	}
	else if (S_OK != DirectX::LoadFromWICFile(Path.c_str(), DirectX::WIC_FLAGS_NONE, &TexData, Image))
	{
		MsgAssert("텍스쳐 로드 실패" + std::string(_Path.data()));
	}

	if (S_OK != DirectX::CreateShaderResourceView
	(
		Ext_DirectXDevice::GetDevice(),
		Image.GetImages(),
		Image.GetImageCount(),
		Image.GetMetadata(),
		&SRV
	))
	{
		MsgAssert("텍스쳐의 SRV 생성 실패" + std::string(_Path.data()));
	}

	Texture2DInfo.Width = static_cast<UINT>(TexData.width);
	Texture2DInfo.Height = static_cast<UINT>(TexData.height);
	Texture2DInfo.Format = TexData.format;
	Texture2DInfo.ArraySize = (UINT)TexData.arraySize;
	Texture2DInfo.MiscFlags = (UINT)TexData.miscFlags;
	Texture2DInfo.MipLevels = (UINT)TexData.mipLevels;
}

// 텍스쳐를 사용한 경우, 여기서 추가로 VSSetting 실시
void Ext_DirectXTexture::VSSetting(UINT _Slot)
{
	if (nullptr == SRV)
	{
		MsgAssert("SRV가 존재하지 않는 텍스처를 쉐이더에 세팅할수 없습니다.");
		return;
	}

	Ext_DirectXDevice::GetContext()->VSSetShaderResources(_Slot, 1, &SRV);
}

// 텍스쳐를 사용한 경우, 여기서 추가로 PSSetting 실시
void Ext_DirectXTexture::PSSetting(UINT _Slot)
{
	if (nullptr == SRV)
	{
		MsgAssert("SRV가 존재하지 않는 텍스처를 쉐이더에 세팅할수 없습니다.");
		return;
	}

	Ext_DirectXDevice::GetContext()->PSSetShaderResources(_Slot, 1, &SRV);
}

// 텍스쳐를 사용한 경우, 여기서 추가로 VSReset 실시
void Ext_DirectXTexture::VSReset(UINT _Slot)
{
	static ID3D11ShaderResourceView* Nullptr = nullptr;

	Ext_DirectXDevice::GetContext()->VSSetShaderResources(_Slot, 1, &Nullptr);
}

// 텍스쳐를 사용한 경우, 여기서 추가로 PSReset 실시
void Ext_DirectXTexture::PSReset(UINT _Slot)
{
	static ID3D11ShaderResourceView* Nullptr = nullptr;

	Ext_DirectXDevice::GetContext()->PSSetShaderResources(_Slot, 1, &Nullptr);
}