#pragma once

// ���� �Ҵ�� DirectX ���ҽ����� ���� �� Ž���ϱ� ���� ����� Ŭ����
template<typename ResourcesType>
class Ext_ResourceManager
{
public:
	// constrcuter destructer
	Ext_ResourceManager() {}
	virtual ~Ext_ResourceManager() {};
	// ���ø� Ŭ������ ������ �� �ν��Ͻ�ȭ �ǹǷ�, ������ ������ �ش� Ÿ���� �ٸ� ���� ���� �� ��ü�� ��ã��
	// ���ø� Ŭ������ ��� �Լ�(�Ҹ��� ����)�� ������� ���ǵ��� ������ ��ũ �Ұ�

	// delete Function
	Ext_ResourceManager(const Ext_ResourceManager& _Other) = delete;
	Ext_ResourceManager(Ext_ResourceManager&& _Other) noexcept = delete;
	Ext_ResourceManager& operator=(const Ext_ResourceManager& _Other) = delete;
	Ext_ResourceManager& operator=(Ext_ResourceManager&& _Other) noexcept = delete;

	// ����� ���ҽ��� �̸����� ã��
	static std::shared_ptr<ResourcesType> Find(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (NameResources.end() == NameResources.find(UpperName.c_str()))
		{
			return nullptr;
		}

		return NameResources[UpperName];
	}

	// ���ҽ� ����(�׳� ����)
	static std::shared_ptr<ResourcesType> CreateResource()
	{
		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		Resources.push_back(NewRes);

		return NewRes;
	}

	// ���ҽ� ����(�̸����� ���� �� ����)
	static std::shared_ptr<ResourcesType> CreateNameResource(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (nullptr != Find(UpperName))
		{
			MsgAssert("�̹� �����ϴ� �̸��� ���ҽ��� �� ������� �߽��ϴ�.");
			return nullptr;
		}

		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		NameResources.insert(std::map<std::string, std::shared_ptr<ResourcesType>>::value_type(UpperName, NewRes));

		return NewRes;
	}

protected:
	
private:
	static std::vector<std::shared_ptr<ResourcesType>> Resources; // ������ ���ҽ�
	static std::map<std::string, std::shared_ptr<ResourcesType>> NameResources; // �̸����� ������ ���ҽ�
	
};

template<typename ResourcesType>
std::vector<std::shared_ptr<ResourcesType>> Ext_ResourceManager<ResourcesType>::Resources;

template<typename ResourcesType>
std::map<std::string, std::shared_ptr<ResourcesType>> Ext_ResourceManager<ResourcesType>::NameResources;