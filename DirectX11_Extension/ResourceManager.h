#pragma once

// �����Ҵ�� ���ҽ����� �������ִ� Ŭ����
template<typename ResourcesType>
class ResourceManager
{
public:
	// constrcuter destructer
	ResourceManager() {}
	virtual ~ResourceManager() {};
	// ���ø� Ŭ������ ������ �� �ν��Ͻ�ȭ �ǹǷ�, ������ ������ �ش� Ÿ���� �ٸ� ���� ���� �� ��ü�� ��ã��
	// ���ø� Ŭ������ ��� �Լ�(�Ҹ��� ����)�� ������� ���ǵ��� ������ ��ũ �Ұ�

	// delete Function
	ResourceManager(const ResourceManager& _Other) = delete;
	ResourceManager(ResourceManager&& _Other) noexcept = delete;
	ResourceManager& operator=(const ResourceManager& _Other) = delete;
	ResourceManager& operator=(ResourceManager&& _Other) noexcept = delete;

	static std::shared_ptr<ResourcesType> Find(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (NameResources.end() == NameResources.find(UpperName.c_str()))
		{
			return nullptr;
		}

		return NameResources[UpperName];
	}

	static std::shared_ptr<ResourcesType> CreateResource()
	{
		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		Resources.push_back(NewRes);

		return NewRes;
	}

	static std::shared_ptr<ResourcesType> CreateNameResource(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (nullptr != Find(UpperName))
		{
			MsgAssert("�̹� �����ϴ� �̸��� ���ҽ��� �� ������� �߽��ϴ�.");
			return nullptr;
		}

		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		//NewRes->SetName(UpperName);
		NameResources.insert(std::map<std::string, std::shared_ptr<ResourcesType>>::value_type(UpperName, NewRes));

		return NewRes;
	}

protected:
	
private:
	static std::vector<std::shared_ptr<ResourcesType>> Resources; // �׳� ������ ���ҽ�
	static std::map<std::string, std::shared_ptr<ResourcesType>> NameResources; // �̸����� ������ ���ҽ�
	
};

template<typename ResourcesType>
std::vector<std::shared_ptr<ResourcesType>> ResourceManager<ResourcesType>::Resources;

template<typename ResourcesType>
std::map<std::string, std::shared_ptr<ResourcesType>> ResourceManager<ResourcesType>::NameResources;