#pragma once

// 동적 할당된 DirectX 리소스들을 저장 및 탐색하기 위해 선언된 클래스
template<typename ResourcesType>
class Ext_ResourceManager
{
public:
	// constrcuter destructer
	Ext_ResourceManager() {}
	virtual ~Ext_ResourceManager() {};
	// 템플릿 클래스는 컴파일 시 인스턴스화 되므로, 선언이 없으면 해당 타입이 다른 곳에 사용될 때 몸체를 못찾음
	// 템플릿 클래스의 멤버 함수(소멸자 포함)는 헤더에서 정의되지 않으면 링크 불가

	// delete Function
	Ext_ResourceManager(const Ext_ResourceManager& _Other) = delete;
	Ext_ResourceManager(Ext_ResourceManager&& _Other) noexcept = delete;
	Ext_ResourceManager& operator=(const Ext_ResourceManager& _Other) = delete;
	Ext_ResourceManager& operator=(Ext_ResourceManager&& _Other) noexcept = delete;

	// 저장된 리소스를 이름으로 찾기
	static std::shared_ptr<ResourcesType> Find(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (NameResources.end() == NameResources.find(UpperName.c_str()))
		{
			return nullptr;
		}

		return NameResources[UpperName];
	}

	// 리소스 생성(그냥 생성)
	static std::shared_ptr<ResourcesType> CreateResource()
	{
		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		Resources.push_back(NewRes);

		return NewRes;
	}

	// 리소스 생성(이름으로 생성 후 저장)
	static std::shared_ptr<ResourcesType> CreateNameResource(std::string_view _Name)
	{
		std::string UpperName = Base_String::ToUpper(_Name);

		if (nullptr != Find(UpperName))
		{
			MsgAssert("이미 존재하는 이름의 리소스를 또 만들려고 했습니다.");
			return nullptr;
		}

		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		NameResources.insert(std::map<std::string, std::shared_ptr<ResourcesType>>::value_type(UpperName, NewRes));

		return NewRes;
	}

protected:
	
private:
	static std::vector<std::shared_ptr<ResourcesType>> Resources; // 저장할 리소스
	static std::map<std::string, std::shared_ptr<ResourcesType>> NameResources; // 이름으로 저장할 리소스
	
};

template<typename ResourcesType>
std::vector<std::shared_ptr<ResourcesType>> Ext_ResourceManager<ResourcesType>::Resources;

template<typename ResourcesType>
std::map<std::string, std::shared_ptr<ResourcesType>> Ext_ResourceManager<ResourcesType>::NameResources;