
#pragma once

#include <string>
#include <memory>

#include "obs-module.h"
#include "EditorWidget.h"
#include "VSTPlugin.h"


//VST3 headers
#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces\gui/iplugview.h"


#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class EditorWidget;

class VST3Plugin: QObject {

	private:
	//std::string VST3PluginCompany;

	std::unique_ptr<Steinberg::Vst::IComponent> component{nullptr}; //Plugin component
	std::unique_ptr<Steinberg::IPluginFactory> factory{nullptr};    //Entry point
	std::unique_ptr<Steinberg::Vst::IAudioProcessor> processor{nullptr}; //Processing


	//std::string sourceName{nullptr};
	//std::string filterName{nullptr};
	char effectName[64];
	
	std::unique_ptr<EditorWidget> editorWidget{nullptr};
	//(QWidget *parent, this);
	
	bool editorOpened = false;

	#if defined(_WIN32)
		HMODULE VST3Module;
	#else
		void *VST3Module;
	#endif

	obs_source_t *context{nullptr};


	public:

	VST3Plugin(obs_source_t *sourceContext) {}
	~VST3Plugin();


	//void setPluginName(std::string PluginName_);

	////Will generally not be called but there are edge cases
	//void setPluginPath(std::string PluginPath_);
	//void setPluginCompany(std::string PluginCompany_);

	////Getters
	//std::string getPluginName() const;

	//std::string getPluginPath() const;
	//std::string getPluginCompany() const;

	//bool load(std::string pluginPath_);

	void loadEffectFromPath(const std::string &path);

	void unloadEffect();

	public slots:

	void openEditor();
	void closeEditor();




};
