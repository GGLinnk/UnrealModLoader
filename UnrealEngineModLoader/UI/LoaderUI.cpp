#include "LoaderUI.h"

#include "dependencies/imgui/imgui.h"
#include "../Utilities/Logger.h"
#include "../Memory/mem.h"
#include "../Utilities/Dumper.h"
#include "../Utilities/Globals.h"
#include "../Utilities/MinHook.h"
#include "../Version.h"

namespace TickVars
{
	bool bDumpObjects;
	bool bDumpEngineInfo;
	bool bDumpWorldActors;
	bool bExecuteConsoleCommand;
	std::wstring CurrentCommand;
};

namespace UITools
{
	void ObjectDump()
	{
		TickVars::bDumpObjects = true;
	}

	void EngineDump()
	{
		TickVars::bDumpEngineInfo = true;
	}

	void WorldDump()
	{
		TickVars::bDumpWorldActors = true;
	}

	void ExecuteCommand(std::wstring command)
	{
		TickVars::CurrentCommand = command;
		TickVars::bExecuteConsoleCommand = true;
	}
};

void UILogicTick()
{
	while (true)
	{
		if (TickVars::bDumpObjects)
		{
			TickVars::bDumpObjects = false;
			Dumper::GetDumper()->DumpObjectArray();
		}

		if (TickVars::bDumpEngineInfo)
		{
			TickVars::bDumpEngineInfo = false;
			Dumper::GetDumper()->DumpEngineInfo();
		}

		if (TickVars::bDumpWorldActors)
		{
			TickVars::bDumpWorldActors = false;
			Dumper::GetDumper()->DumpWorldActors();
		}

		if (TickVars::bExecuteConsoleCommand)
		{
			TickVars::bExecuteConsoleCommand = false;
			UE4::UGameplayStatics::ExecuteConsoleCommand(TickVars::CurrentCommand.c_str(), nullptr);
			TickVars::CurrentCommand = L"";
		}
		Sleep(1000 / 60);
	}
}

void ShowLogicMods()
{
	if (!ImGui::CollapsingHeader("Logic Mods"))
		return;

	for (size_t i = 0; i < Global::GetGlobals()->ModInfoList.size(); i++)
	{
		std::string str(Global::GetGlobals()->ModInfoList[i].ModName.begin(), Global::GetGlobals()->ModInfoList[i].ModName.end());
		std::string ModLabel = str + "##" + std::to_string(i);
		if (ImGui::TreeNode(ModLabel.c_str()))
		{
			std::string Author = "Created By: " + Global::GetGlobals()->ModInfoList[i].ModAuthor;
			ImGui::Text(Author.c_str());
			ImGui::Separator();
			std::string Description = "Description: " + Global::GetGlobals()->ModInfoList[i].ModDescription;
			ImGui::Text(Description.c_str());
			ImGui::Separator();
			std::string Version = "Version: " + Global::GetGlobals()->ModInfoList[i].ModVersion;
			ImGui::Text(Version.c_str());
			ImGui::Separator();
			if (ImGui::TreeNode("Mod Buttons"))
			{
				if (Global::GetGlobals()->ModInfoList[i].IsEnabled && Global::GetGlobals()->ModInfoList[i].CurrentModActor && Global::GetGlobals()->ModInfoList[i].ContainsButton)
				{
					for (size_t bi = 0; bi < Global::GetGlobals()->ModInfoList[i].ModButtons.size(); bi++)
					{
						auto currentmodbutton = Global::GetGlobals()->ModInfoList[i].ModButtons[bi];
						std::string ButtonLabel = currentmodbutton + "##" + std::to_string(i + bi);
						if (ImGui::Button(ButtonLabel.c_str()))
						{
							std::wstring FuncNameAndArgs = L"ModMenuButtonPressed " + std::to_wstring(bi);
							Global::GetGlobals()->ModInfoList[i].CurrentModActor->CallFunctionByNameWithArguments(FuncNameAndArgs.c_str(), nullptr, NULL, true);
						}
					}
					ImGui::Separator();
				}
				ImGui::TreePop();
			}
			std::string ActiveLabel = "Enable##" + std::to_string(i);
			ImGui::Checkbox(ActiveLabel.c_str(), &Global::GetGlobals()->ModInfoList[i].IsEnabled);
			ImGui::TreePop();
		}
	}
}

void ShowCoreMods()
{
	if (!ImGui::CollapsingHeader("Core Mods"))
		return;

	for (size_t i = 0; i < Global::GetGlobals()->CoreMods.size(); i++)
	{
		std::string str(Global::GetGlobals()->CoreMods[i]->ModName.begin(), Global::GetGlobals()->CoreMods[i]->ModName.end());
		std::string ModLabel = str + "##cm" + std::to_string(i);
		if (ImGui::TreeNode(ModLabel.c_str()))
		{

			std::string Author = "Created By: " + Global::GetGlobals()->CoreMods[i]->ModAuthors;
			ImGui::Text(Author.c_str());
			ImGui::Separator();
			std::string Description = "Description: " + Global::GetGlobals()->CoreMods[i]->ModDescription;
			ImGui::Text(Description.c_str());
			ImGui::Separator();
			std::string Version = "Version: " + Global::GetGlobals()->CoreMods[i]->ModVersion;
			ImGui::Text(Version.c_str());
			ImGui::Separator();

			if (Global::GetGlobals()->CoreMods[i]->UseMenuButton && Global::GetGlobals()->CoreMods[i]->IsFinishedCreating)
			{
				std::string ButtonLabel = str + " Button" + "##cm" + std::to_string(i);
				if (ImGui::Button(ButtonLabel.c_str()))
				{
					Global::GetGlobals()->CoreMods[i]->OnModMenuButtonPressed();
				}
			}

			ImGui::TreePop();
		}
	}
}

void ShowTools()
{
	if (!ImGui::CollapsingHeader("Tools"))
		return;

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Spacing();
	if (ImGui::Button("Dump Objects"))
	{
		UITools::ObjectDump();
	}
	if (ImGui::Button("Dump Engine Info"))
	{
		UITools::EngineDump();
	}
	if (ImGui::Button("Dump World Actors"))
	{
		UITools::WorldDump();
	}


	if (!DmgConfig::Instance.SkipCallFunctionByNameWithArguments) {
		static char Command[128];
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Execute Console Command");
		ImGui::InputText("", Command, IM_ARRAYSIZE(Command));
		if (ImGui::Button("Execute"))
		{
			std::string strCommand(Command);
			std::wstring wstrCommand = std::wstring(strCommand.begin(), strCommand.end());
			UITools::ExecuteCommand(wstrCommand);
		}
	} else {
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Console Command not available with current profile file.");
	}
}

void DrawImGui()
{
	ImGui::Begin("Unreal Mod Loader", NULL, NULL);
	ImGui::Spacing();
	ImGui::Text("Unreal Mod Loader V: %s", MODLOADER_VERSION);
	ShowLogicMods();
	ShowCoreMods();
	ShowTools();

	ImGui::End();
}
