#include "PrecompileHeader.h"
#include "Ext_DirectXRenderTarget.h"

void Ext_DirectXRenderTarget::CreateDepthTexture(int _Index)
{
	D3D11_TEXTURE2D_DESC Desc = { 0, };

	Desc.ArraySize = 1;
	Desc.Width = Textures[_Index]->GetWidth();
	Desc.Height = Textures[_Index]->GetHeight();
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;    // 4����Ʈ�� 3����Ʈ�� 0 ~ 1.0f ǥ���ϴµ� ����ϰ�, ������ 1����Ʈ�� ���ٽ� ������ ����� �� �ְ� ���ִ� ����
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.MipLevels = 1;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;

	DepthTexture = Ext_DirectXTexture::Create(Desc); // �������ٽǺ� ����
}

// ����Ÿ�� ����
void Ext_DirectXRenderTarget::ResourceCreate(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color)
{
	Color.push_back(_Color);
	D3D11_VIEWPORT ViewPortData;

	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = _Texture->GetScale().x;
	ViewPortData.Height = _Texture->GetScale().y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	ViewPortDatas.push_back(ViewPortData);

	Textures.push_back(_Texture);       // ������ �ؽ��� ���� ���Ϳ� Ǫ����
	SRVs.push_back(_Texture->GetSRV());
	RTVs.push_back(_Texture->GetRTV()); // ������ �ؽ����� RTV ���� ���Ϳ� Ǫ����
}
