#pragma once

// Object �Ӽ��� ����ϴ� Ŭ����
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

	// this ��������
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

	std::weak_ptr<class Ext_Scene> OwnerScene;	// �ڽ��� ���� Scene ����
	std::string Name = "";										// ������Ʈ �̸�
	int Order = 0;													// Order ����(���� �� �׷�ȭ�� �� Ȱ��)
	bool IsUpdate = true;											// ������Ʈ �Ұ����� ���Ұ����� ����
	bool IsDeath = false;											// Delete �Ұ����� ���Ұ����� ����(���� �Ⱦ�)

private:

};