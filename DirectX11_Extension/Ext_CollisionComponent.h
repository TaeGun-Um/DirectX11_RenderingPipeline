#pragma once
#include "Ext_Component.h"

enum class CollsionType
{
	Sphere2D,
	AABB2D,
	OBB2D,
	Sphere3D,
	AABB3D,
	OBB3D,
	Unknown,
};

class CollisionData
{
public:
	CollisionData() {}
	~CollisionData() {}

	union
	{
		DirectX::BoundingSphere Sphere;
		DirectX::BoundingBox AABB;
		DirectX::BoundingOrientedBox OBB;
	};

	//void ScaleABS()
	//{
	//	OBB.Extents.x = abs(OBB.Extents.x);
	//	OBB.Extents.y = abs(OBB.Extents.y);
	//	OBB.Extents.z = abs(OBB.Extents.z);
	//}
};

class CollisionParameter
{
public:
	std::shared_ptr<class Ext_Transform> _OtherTrans = nullptr;
	CollsionType ThisType = CollsionType::Sphere3D;
	CollsionType OtherType = CollsionType::Sphere3D;
};

// 충돌체를 위한 클래스
class Ext_CollisionComponent : public Ext_Component
{
	friend class CollisionFunctionInitialize;

public:
	// constrcuter destructer
	Ext_CollisionComponent() {}
	~Ext_CollisionComponent() {}

	// delete Function
	Ext_CollisionComponent(const Ext_CollisionComponent& _Other) = delete;
	Ext_CollisionComponent(Ext_CollisionComponent&& _Other) noexcept = delete;
	Ext_CollisionComponent& operator=(const Ext_CollisionComponent& _Other) = delete;
	Ext_CollisionComponent& operator=(Ext_CollisionComponent&& _Other) noexcept = delete;

	template<typename EnumType>
	std::shared_ptr<Ext_CollisionComponent> CollisionCheck(EnumType _TargetGroup, CollsionType _ThisColType = CollsionType::Unknown, CollsionType _OtherColType = CollsionType::Unknown)
	{
		return Collision(static_cast<int>(_TargetGroup), _ThisColType, _OtherColType);
	}

	std::shared_ptr<Ext_CollisionComponent> CollisionCheck(int _TargetGroup, CollsionType _ThisColType = CollsionType::Unknown, CollsionType _OtherColType = CollsionType::Unknown);

	void SetCollsionType(CollsionType _Type) { ColType = _Type; }
	CollsionType GetCollisionType() { return ColType; }
	CollisionData GetCollisionData() { return ColData; }

protected:
	void Start() override;
	// void Update(float _DeltaTime) override;
	
private:
	static std::function<bool(const CollisionData&, const CollisionData&)> CollisionFunction[static_cast<int>(CollsionType::Unknown)][static_cast<int>(CollsionType::Unknown)];
	CollsionType ColType = CollsionType::Sphere3D; // 미지정시, 기본은 Sphere3D
	CollisionData ColData;

	static bool SphereToSpehre(const CollisionData&, const CollisionData&);
	static bool SphereToAABB(const CollisionData&, const CollisionData&);
	static bool SphereToOBB(const CollisionData&, const CollisionData&);

	static bool AABBToSpehre(const CollisionData&, const CollisionData&);
	static bool AABBToAABB(const CollisionData&, const CollisionData&);
	static bool AABBToOBB(const CollisionData&, const CollisionData&);

	static bool OBBToSpehre(const CollisionData&, const CollisionData&);
	static bool OBBToAABB(const CollisionData&, const CollisionData&);
	static bool OBBToOBB(const CollisionData&, const CollisionData&);

	static bool Sphere2DToSpehre2D(const CollisionData&, const CollisionData&);
	static bool Sphere2DToAABB2D(const CollisionData&, const CollisionData&);
	static bool Sphere2DToOBB2D(const CollisionData&, const CollisionData&);

	static bool AABB2DToSpehre2D(const CollisionData&, const CollisionData&);
	static bool AABB2DToAABB2D(const CollisionData&, const CollisionData&);
	static bool AABB2DToOBB2D(const CollisionData&, const CollisionData&);

	static bool OBB2DToSpehre2D(const CollisionData&, const CollisionData&);
	static bool OBB2DToAABB2D(const CollisionData&, const CollisionData&);
	static bool OBB2DToOBB2D(const CollisionData&, const CollisionData&);
};