#include "PrecompileHeader.h"
#include "ExtGui.h"

#include <DirectX11_Base/Base_Deltatime.h>

ExtGui::ExtGui()
{
}

ExtGui::~ExtGui()
{
}

void ExtGui::OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime)
{
	FPSView(_DeltaTime);

	if (ImGui::Button("DebugSwitch"))
	{
		int a = 0;
	}
}

void ExtGui::FPSView(float _DeltaTime)
{
	static float Rate = 0;
	FPSTime += _DeltaTime;

	if (FPSTime >= 1.0)
	{
		FPSTime = 0.f;
		Rate = Base_Deltatime::GetGlobalTime().GetFrameRate();
	}

	ImGui::Text(std::to_string(Rate).c_str());
}