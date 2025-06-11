#include "PrecompileHeader.h"
#include "InformationGUI.h"

#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>

InformationGUI::InformationGUI()
{
}

InformationGUI::~InformationGUI()
{
}

void InformationGUI::OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime)
{
	FPSView(_DeltaTime);

	if (ImGui::Button("DebugSwitch"))
	{

	}

	if (nullptr != Function1) Function1;

	if (nullptr != Transform1)
	{
		float4 LocalPos = Transform1->GetLocalPosition();
		float4 LocalRot = Transform1->GetLocalRotation();
		float4 LocalScale = Transform1->GetLocalScale();

		float4 WorldPos = Transform1->GetWorldPosition();
		float4 WorldRot = Transform1->GetWorldRotation();
		float4 WorldScale = Transform1->GetWorldScale();

		LocalPos.x;

		ImGui::Text("LocalPosition");
		ImGui::Text("LocalPosition");
		ImGui::Text(std::to_string(LocalPos.x).c_str());
		ImGui::Text(std::to_string(LocalPos.y).c_str());
		ImGui::Text(std::to_string(LocalPos.z).c_str());

		//ImGui::Text("LocalRotation");
		//ImGui::Text(std::to_string(LocalRot.x).c_str());
		//ImGui::Text(std::to_string(LocalRot.y).c_str());
		//ImGui::Text(std::to_string(LocalRot.z).c_str());
		//
		//ImGui::Text("LocalScale");
		//ImGui::Text(std::to_string(LocalScale.x).c_str());
		//ImGui::Text(std::to_string(LocalScale.y).c_str());
		//ImGui::Text(std::to_string(LocalScale.z).c_str());

		ImGui::Text("WorldPosition");
		ImGui::Text(std::to_string(WorldPos.x).c_str());
		ImGui::Text(std::to_string(WorldPos.y).c_str());
		ImGui::Text(std::to_string(WorldPos.z).c_str());

		//ImGui::Text("WorldRotation");
		//ImGui::Text(std::to_string(WorldRot.x).c_str());
		//ImGui::Text(std::to_string(WorldRot.y).c_str());
		//ImGui::Text(std::to_string(WorldRot.z).c_str());
		//
		//ImGui::Text("WorldScale");
		//ImGui::Text(std::to_string(WorldScale.x).c_str());
		//ImGui::Text(std::to_string(WorldScale.y).c_str());
		//ImGui::Text(std::to_string(WorldScale.z).c_str());
	}

	if (nullptr != Transform2)
	{
		float4 LocalPos = Transform2->GetLocalPosition();
		float4 LocalRot = Transform2->GetLocalRotation();
		float4 LocalScale = Transform2->GetLocalScale();

		float4 WorldPos = Transform2->GetWorldPosition();
		float4 WorldRot = Transform2->GetWorldRotation();
		float4 WorldScale = Transform2->GetWorldScale();

		LocalPos.x;

		ImGui::Text("LocalPosition");
		ImGui::Text(std::to_string(LocalPos.x).c_str());
		ImGui::Text(std::to_string(LocalPos.y).c_str());
		ImGui::Text(std::to_string(LocalPos.z).c_str());

		//ImGui::Text("LocalRotation");
		//ImGui::Text(std::to_string(LocalRot.x).c_str());
		//ImGui::Text(std::to_string(LocalRot.y).c_str());
		//ImGui::Text(std::to_string(LocalRot.z).c_str());
		//
		//ImGui::Text("LocalScale");
		//ImGui::Text(std::to_string(LocalScale.x).c_str());
		//ImGui::Text(std::to_string(LocalScale.y).c_str());
		//ImGui::Text(std::to_string(LocalScale.z).c_str());

		ImGui::Text("WorldPosition");
		ImGui::Text(std::to_string(WorldPos.x).c_str());
		ImGui::Text(std::to_string(WorldPos.y).c_str());
		ImGui::Text(std::to_string(WorldPos.z).c_str());

		//ImGui::Text("WorldRotation");
		//ImGui::Text(std::to_string(WorldRot.x).c_str());
		//ImGui::Text(std::to_string(WorldRot.y).c_str());
		//ImGui::Text(std::to_string(WorldRot.z).c_str());
		//
		//ImGui::Text("WorldScale");
		//ImGui::Text(std::to_string(WorldScale.x).c_str());
		//ImGui::Text(std::to_string(WorldScale.y).c_str());
		//ImGui::Text(std::to_string(WorldScale.z).c_str());
	}
}

void InformationGUI::FPSView(float _DeltaTime)
{
	static float Rate = Base_Deltatime::GetGlobalTime().GetFrameRate();
	ImGui::Text(std::to_string(Rate).c_str());
}