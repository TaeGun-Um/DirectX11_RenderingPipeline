#pragma once

#include "Ext_DirectXTexture.h"
#include "Ext_ResourceManager.h"

// ���� Ÿ�� ���� Ŭ����
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

	// ����Ÿ�� ����, [1] ����Ÿ�� �̸� ����, [2] Ext_DirectXTexture ������, [3] ����Ÿ�� ����
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
	void CreateRenderTarget(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color); // View�� ������� ����Ÿ�� ����

	std::vector<float4> Colors; // ������ ����Ÿ�� ���� ����
	std::vector<std::shared_ptr<Ext_DirectXTexture>> Textures = {}; // Ext_DirectXTexture(���� ��ü) ������ ���� �����̳�
	std::shared_ptr<Ext_DirectXTexture> DepthTexture = {};
	std::vector<D3D11_VIEWPORT> ViewPorts = {}; // ���� ��ü�� ViewPort ���� ���� �����̳�

	// std::vector<ID3D11RenderTargetView*> RTVs = {}; // ���� ��ü�� ����Ÿ�ٺ� ���� ���� �����̳�
	// std::vector<ID3D11ShaderResourceView*> SRVs = {}; // ���� ��ü�� ���̴����ҽ��� ���� ���� �����̳�
};
// [RenderTarget]
// ���� Ÿ���� ������ ����� ����ϴ� ���� �������� �ȴ�. ��, ���̴��� ��� ����� �׷����� �����̴�.
// �Ϲ������� ȭ�鿡 ��µǴ� ������ ����(�� ����, ����Ʈ ����)�� ��ó���� �ؽ��� ���� ���� Ÿ���� �ȴ�.
// �ַ� ID3D11Texture2D�� ���°� �ȴ�.

// [D3D11_VIEWPORT]
// �������� ��� ����(ViewPort)�� �����ϴ� ����ü�̴�. �ȼ��� ������ �׷��� ȭ����� �簢�� ������ �����ϴµ� ���ȴ�.
// <<����>>
/*1. TopLeftX : ����Ʈ �»�� X ��ġ (�ȼ� ����)*/
/*2. TopLeftY : ����Ʈ �»�� Y ��ġ (�ȼ� ����)*/
/*3. Width : ����Ʈ �ʺ� (�ȼ� ����)*/
/*4. Height : ����Ʈ ���� (�ȼ� ����)*/
/*5. MinDepth : ���̰� �ּ� (���� 0.0f)*/
/*6. MaxDepth : ���̰� �ִ� (���� 1.0f)*/

// [std::shared_ptr<Ext_DirectXTexture> DepthTexture]
// DepthTexture�� GPU�� ����(Depth), ���Ľ�(Stencil) ������ �����ϴ� Ư���� 2D �ؽ����̴�.
// �Ϲ� �ؽ�óó�� ������ �����ϴ� ���� �ƴ϶�, �� �ȼ��� �󸶳� ī�޶� �ָ� �ִ����� �ε� �Ҽ������� �����Ѵ�.
// ������ �� ���� �׽�Ʈ(Z-test)�� ���ȴ�.
// <<���� �׽�Ʈ>>
// 1. GPU�� ȭ�鿡 �ȼ��� ��� ��, ���� �ȼ����� �� ������� Ȯ���ؾ� ��
// 2. �� ���� �񱳸� ���� Depth Buffer(���⼭�� DepthTexture)�� �ʿ�
// <<���ٽ� �׽�Ʈ>>
// ����ŷ, Ŭ���� ȿ���� ���� �� ���(�Ƿ翧, �׸��� ���� ��)
