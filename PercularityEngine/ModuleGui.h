#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "ConfigWindow.h"
#include "SceneWindow.h"
#include "ConsoleWindow.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "ProjectWindow.h"
#include "MainMenuBar.h"
#include "ImGui/imgui.h"

#define FILE_MAX 250

// ---------------------------------------------------
class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleGui();

	// Called when before render is available
	bool Init();

	// Call before first frame
	bool Start();

	// Called before all Updates
	update_status PreUpdate(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//Save & Load
	void Load(const nlohmann::json &config);
	void Save(nlohmann::json &config);

	// Draw all the GUI
	void DrawImGui(float dt);

	void LoadFile(const char* filter_extension = nullptr, const char* from_dir = nullptr);
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);

public:
	//bool show_demo_window = true;
	ImGuiIO* io = nullptr;
	SceneWindow* scene_window = nullptr;
	ConfigWindow* settings = nullptr;

private:
	enum
	{
		CLOSED,
		OPENED,
		READY_TO_CLOSE
	} file_dialog = CLOSED;

	MainMenuBar* main_menu_bar = nullptr;
	ConsoleWindow* console = nullptr;
	InspectorWindow* inspector = nullptr;
	HierarchyWindow* hierarchy = nullptr;
	ProjectWindow* project = nullptr;

	std::vector<UIElement*> ui_elements_list;

	bool p_open = true;
	char selected_file[FILE_MAX];
};

#endif // __ModuleGui_H__
