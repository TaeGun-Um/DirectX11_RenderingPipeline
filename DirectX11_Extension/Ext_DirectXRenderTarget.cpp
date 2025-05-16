#include "PrecompileHeader.h"
#include "Ext_DirectXRenderTarget.h"

// View�� ������� ����Ÿ�� ����
void Ext_DirectXRenderTarget::CreateRenderTarget(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color)
{
	Colors.push_back(_Color);

	// �������� ��� ���� ���� ����ü ����
	D3D11_VIEWPORT ViewPortData;
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = _Texture->GetScale().x;
	ViewPortData.Height = _Texture->GetScale().y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;
	// <<����>>
	/*1. TopLeftX : ����Ʈ �»�� X ��ġ (�ȼ� ����)*/
	/*2. TopLeftY : ����Ʈ �»�� Y ��ġ (�ȼ� ����)*/
	/*3. Width : ����Ʈ �ʺ� (�ȼ� ����)*/
	/*4. Height : ����Ʈ ���� (�ȼ� ����)*/
	/*5. MinDepth : ���̰� �ּ� (���� 0.0f)*/
	/*6. MaxDepth : ���̰� �ִ� (���� 1.0f)*/

	ViewPorts.push_back(ViewPortData); // D3D11_VIEWPORT ����
	Textures.push_back(_Texture); // Ext_DirectXTexture ����
	SRVs.push_back(_Texture->GetSRV()); // ID3D11ShaderResourceView ����
	RTVs.push_back(_Texture->GetRTV()); // ID3D11RenderTargetView ����
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
	// <����>
	/*1. 	D3D11_TEXTURE2D_DESC ����ü���� �ش� �ؽ�ó�� �� ���� �迭 ���(Array Slice)�� �����Ǿ� �ִ����� ��Ÿ����. 1�� ���� 2D �ؽ�ó��� ��*/
	/*2. ���� �ؽ��� ����(���� Ÿ�� ũ��� �����ϰ�)*/
	/*3. ���� �ؽ��� ����(���� Ÿ�� ũ��� �����ϰ�)*/
	/*4. 4byte(32bit)�� ���� 24bit�� ���̰�(0.0 ~ 1.0), ���� 8bit�� ���ٽ� ��(0~255)���� Ȱ��*/
	/*5. ��Ƽ ���ø�(��Ƽ�ٸ����) ����, Count = 1�� ��Ȱ��ȭ��� ��*/
	/*6. ��Ƽ ���ø�(��Ƽ�ٸ����) ����, Quality = 0�� �⺻ ǰ�� ���� ����Ѵٴ� ��*/
	/*7. �ؽ��� �Ӹ� ���� ����, Depth Texture�� ���� �� ������ �ʿ��ؼ� 1�� ����*/
	/*8. ���ҽ� ��� ��� ����, D3D11_USAGE_DEFAULT�� GPU���� �а� ���� �Ϲ����� ���ҽ� Ÿ��*/
	/*9. �ش� �ؽ�ó�� � �뵵�� ���ε��� �� �ִ��� �����Ѵ�. */

	DepthTexture = Ext_DirectXTexture::CreateViews(Desc); // �������ٽǺ� ����
	// ���� �ؽ��Ĵ� 
	// 1. �ٸ� ��ü�� �������� ������ ���� �ʵ��� �ϱ� ���� ���� ����(0.0 ~ 1.0�̸� 1.0�� ���� �� ��ü)�� ���� ���� ����� �ȼ� ������
	// 2. Ư�� ��Ȳ������ ���� ���۷θ� ��ü�� ������ �� ������ �Ǵ��ϱ� ��Ʊ� ������(Ư�� ���� ������ ����(���ٽ� ���۰��� ä���������� �׷�����, ������� �ȱ׸���)) ���� ���� ���ٽ� ���� Ȱ��
	//   2-1. ���� ��� �ſ��� �ִ� ���� ���� ��, �ſ� �κ��� ���ٽ� ���۸� ���� ��
	// ���� �� ���� ������ ���Ͽ� �����Ѵ�.
}

