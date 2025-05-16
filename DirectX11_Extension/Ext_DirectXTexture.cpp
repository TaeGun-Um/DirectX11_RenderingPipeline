#include "PrecompileHeader.h"
#include "Ext_DirectXTexture.h"
#include "Ext_DirectXDevice.h"

// ����� ����Ÿ�ٺ� ������ ���� ȣ���
void Ext_DirectXTexture::CreateRenderTargetView(ID3D11Texture2D* _Texture)
{
	Texture2D = _Texture;
	Texture2D->GetDesc(&Desc); // Desc(D3D11_TEXTURE2D_DESC)�� �� ����
	CreateRenderTargetView();
}

// Desc�� BindFlags�� ���� RTV, SRV, DSV �� �ϳ��� Create
void Ext_DirectXTexture::CreateView(const D3D11_TEXTURE2D_DESC& _Value)
{
	Desc = _Value;

	Ext_DirectXDevice::GetDevice()->CreateTexture2D(&Desc, nullptr, &Texture2D);

	if (D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET & Desc.BindFlags)
	{
		CreateRenderTargetView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags)
	{
		CreateShaderResourcesView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL & Desc.BindFlags)
	{
		CreateDepthStencilView();
	}

	if (nullptr == Texture2D)
	{
		MsgAssert("�ؽ�ó ������ �����߽��ϴ�.");
	}
}

// ����Ÿ�ٺ�(RTV) ����, CreateRenderTargetView()
void Ext_DirectXTexture::CreateRenderTargetView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("�ؽ�ó�� �������� �ʴ� ����Ÿ�ٺ��� ���� ���� �����ϴ�.");
		return;
	}

	ID3D11RenderTargetView* NewRTV = nullptr;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRenderTargetView(Texture2D, nullptr, &NewRTV))
	{
		MsgAssert("����Ÿ�� �� ������ �����߽��ϴ�.");
		return;
	}
	// <<����>> CreateRenderTargetView()�� ���� ����
	/*1. ���� Ÿ������ ����� 2D �ؽ�ó ���ҽ�*/
	/*2. ����Ÿ�ٺ並 ����Ͽ� �׼��� �� �� �ִ� ���ҽ����� �����ϴ� ����ü, ������ �ʿ���� nullptr ����*/
	/*3. ������ RTV ��ü�� ���� ������*/

	RTVs.push_back(NewRTV);
}

// �������ٽǺ�(DSV) ����, CreateDepthStencilView()
void Ext_DirectXTexture::CreateDepthStencilView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("�ؽ�ó�� �������� �ʴµ� ���� ���ٽ� �� ���� ���� �����ϴ�.");
		return;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateDepthStencilView(Texture2D, nullptr, &DepthStencilView))
	{
		MsgAssert("���� ���ٽ� �� ������ �����߽��ϴ�.");
		return;
	}
	// <<����>> CreateRenderTargetView()�� ���� ����
	/*1. ����/���ٽ� ������ ������ �ؽ�ó*/
	/*2. �⺻ �� ���� ���*/
	/*3. ������ DSV ��ü ������*/
}

// ���̴����ҽ���(SRV) ����, CreateShaderResourceView()
void Ext_DirectXTexture::CreateShaderResourcesView()
{
	if (nullptr == Texture2D)
	{
		MsgAssert("�ؽ�ó�� �������� �ʴ� ���̴� ���ҽ� �並 ���� ���� �����ϴ�.");
		return;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateShaderResourceView(Texture2D, nullptr, &ShaderResourceView))
	{
		MsgAssert("���̴� ���ҽ� �� ������ �����߽��ϴ�.");
		return;
	}
	// <<����>> CreateRenderTargetView()�� ���� ����
	/*1. ���̴����� ���� 2D �ؽ�ó*/
	/*2. ��ü �ؽ�ó�� �⺻ �������� ����*/
	/*3. ������ SRV�� ������ ������*/
}

