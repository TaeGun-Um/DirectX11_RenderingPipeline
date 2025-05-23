#include "PrecompileHeader.h"
#include "Ext_DirectXBufferSetter.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXShader.h"

Ext_DirectXBufferSetter::Ext_DirectXBufferSetter()
{
}

Ext_DirectXBufferSetter::~Ext_DirectXBufferSetter()
{
}

// 버퍼 복붙하기
void Ext_DirectXBufferSetter::Copy(const Ext_DirectXBufferSetter& _OtherBufferSetter)
{
	for (const std::pair<std::string, ConstantBufferSetter>& Setter : _OtherBufferSetter.ConstantBufferSetters)
	{
		ConstantBufferSetters.insert(Setter);
	}

	//for (const std::pair<std::string, TextureBufferSetter>& Setter : _BufferSetter.TextureSetters)
	//{
	//	TextureSetters.insert(Setter);
	//}

	//for (const std::pair<std::string, SamplerBufferSetter>& Setter : _BufferSetter.SamplerSetters)
	//{
	//	SamplerSetters.insert(Setter);
	//}

	//for (const std::pair<std::string, StructuredBufferSetter>& Setter : _BufferSetter.StructuredBufferSetters)
	//{
	//	StructuredBufferSetters.insert(Setter);
	//}
}

void Ext_DirectXBufferSetter::SetConstantBufferLink(std::string_view _Name, const void* _Data, UINT _Size)
{
	std::string UpperName = Base_String::ToUpper(_Name);

	std::multimap<std::string, ConstantBufferSetter>::iterator FindIter = ConstantBufferSetters.find(UpperName);

	if (ConstantBufferSetters.end() == FindIter)
	{
		MsgAssert("존재하지 않는 상수버퍼를 세팅하려고 했습니다." + UpperName);
		return;
	}

	std::multimap<std::string, ConstantBufferSetter>::iterator NameStartIter = ConstantBufferSetters.lower_bound(UpperName);
	std::multimap<std::string, ConstantBufferSetter>::iterator NameEndIter = ConstantBufferSetters.upper_bound(UpperName);

	for (; NameStartIter != NameEndIter; ++NameStartIter)
	{
		ConstantBufferSetter& BufferSetter = NameStartIter->second;

		if (BufferSetter.ConstantBuffer->GetBufferSize() != _Size)
		{
			MsgAssert("상수버퍼와 세팅하려는 데이터의 크기가 다릅니다. 상수버퍼 : " + std::to_string(BufferSetter.ConstantBuffer->GetBufferSize()) + "유저가 세팅한 데이터" + std::to_string(_Size) + UpperName);
			return;
		}

		BufferSetter.CPUData = _Data;
		BufferSetter.CPUDataSize = _Size;
	}
}

void Ext_DirectXBufferSetter::BufferSetting()
{
	std::multimap<std::string, ConstantBufferSetter>::iterator StartIter = ConstantBufferSetters.begin();
	std::multimap<std::string, ConstantBufferSetter>::iterator EndIter = ConstantBufferSetters.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		ConstantBufferSetter& Setter = StartIter->second;
		Setter.Setting();
	}
}

void ConstantBufferSetter::Setting()
{
	ConstantBuffer->ChangeData(CPUData, CPUDataSize);

	ShaderType Type = OwnerShader.lock()->GetType();

	switch (Type)
	{
	case ShaderType::Unknown:
	{
		MsgAssert("어떤 쉐이더에 세팅될지 알수없는 상수버퍼 입니다.");
		break;
	}
	case ShaderType::Vertex:
	{
		ConstantBuffer->VSSetting(BindPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		ConstantBuffer->PSSetting(BindPoint);
		break;
	}
	default:
		break;
	}
}
