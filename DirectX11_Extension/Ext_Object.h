#pragma once

// Object 속성을 담당하는 클래스
class Ext_Object : public std::enable_shared_from_this<Ext_Object>
{
public:
	// constrcuter destructer
	Ext_Object() {}
	virtual ~Ext_Object() = 0;

	// delete Function
	Ext_Object(const Ext_Object& _Other) = delete;
	Ext_Object(Ext_Object&& _Other) noexcept = delete;
	Ext_Object& operator=(const Ext_Object& _Other) = delete;
	Ext_Object& operator=(Ext_Object&& _Other) noexcept = delete;

	// this 가져오기
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}
	
	// Getter, Setter
	bool GetIsUpdate() { return IsUpdate; }
	void UpdateOn() { IsUpdate = true; }
	void UpdateOff() { IsUpdate = false; }

	bool GetIsDeath() { return IsDeath; }
	bool SetIsDeath(bool _IsDeath) { IsDeath = _IsDeath; }

	int GetOrder() { return Order; }
	std::string GetName() { return Name; }
	std::weak_ptr<class Ext_Scene> GetOwnerScene() { return OwnerScene; }

protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void Destroy() {}

	void SetOrder(int _Order) { Order = _Order; }
	void SetName(std::string_view _Name) { Name = _Name; }
	void SetOwnerScene(std::weak_ptr<class Ext_Scene> _OwnerScene) { OwnerScene = _OwnerScene; }

	std::weak_ptr<class Ext_Scene> OwnerScene;	// 자신이 속한 Scene 정보
	std::string Name = "";										// 오브젝트 이름
	int Order = 0;													// Order 지정(저장 시 그룹화할 때 활용)
	bool IsUpdate = true;											// 업데이트 할것인지 안할것인지 지정
	bool IsDeath = false;											// Delete 할것인지 안할것인지 지정(아직 안씀)

private:

};