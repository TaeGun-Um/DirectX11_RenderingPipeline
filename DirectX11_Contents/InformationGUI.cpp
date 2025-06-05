#include "PrecompileHeader.h"
#include "InformationGUI.h"

InformationGUI::InformationGUI()
{
}

InformationGUI::~InformationGUI()
{
}

void InformationGUI::OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime)
{
	// FPSView(_DeltaTime);

	if (ImGui::Button("DebugSwitch"))
	{
		int a = 0;
	}
}

void InformationGUI::FPSView(float _DeltaTime)
{
	static float Rate = Base_Deltatime::GetGlobalTime().GetFrameRate();
	ImGui::Text(std::to_string(Rate).c_str());
}