

#include "headers/VST3Plugin.h"
#include <functional>
#include <iostream>


//VST3Plugin::VST3Plugin(obs_source_t *sourceContext) : context(sourceContext)
//{
//	
//
//		
//	component = nullptr;
//	processor = nullptr;
//	factory = nullptr;
//}

//VST3Plugin::~VST3Plugin()
//{
//}
/*
//Setters
void VST3Plugin::setPluginName(std::string PluginName_) {

	sourceName = PluginName_;
}


//Will generally not be called but there are edge cases
void VST3Plugin::setPluginPath(std::string PluginPath_) {

	VST3pluginPath = PluginPath_;
}

void VST3Plugin::setPluginCompany(std::string PluginCompany_) {
	VST3PluginCompany = PluginCompany_;
}

//Getters
std::string  VST3Plugin::getPluginName() const {

	return sourceName;
}

std::string VST3Plugin::getPluginPath() const {
	return VST3pluginPath;
}

std::string VST3Plugin::getPluginCompany() const {
	return VST3PluginCompany;
}
*/
//Load and unload
void VST3Plugin::loadEffectFromPath(const std::string &path) {

	
	typedef Steinberg::IPluginFactory *(PLUGIN_API * loadDLL)();

	//Finn utav datatypuni her
	VST3Module = LoadLibraryA(path.c_str());

	

	if (!VST3Module) {
		DWORD error = GetLastError();
		std::cout << error << std::endl;
		return;
	}

	#if defined(_WIN32)
	auto getFactory = (loadDLL)(GetProcAddress(VST3Module, "GetPluginFactory"));
	#else
	auto getFactory =  (loadDLL)(dlsym(VST3Module, "GetPluginFactory"));
	#endif

	if (!getFactory) {
		return;
	}

	factory.reset(getFactory());

	if (!factory) {
		return;
	}

	Steinberg::PClassInfo classInfo;

	if (factory->getClassInfo(0, &classInfo) != Steinberg::kResultOk) {
		return;
	}

	Steinberg::FUID classId(Steinberg::FUID::fromTUID(classInfo.cid));

	

	static const Steinberg::FUID inline_component_iid(Steinberg::Vst::IComponent_iid);

	if (factory->createInstance(classId.toTUID(), inline_component_iid, (void **)&component) !=
	    Steinberg::kResultOk) {
		return;
	}

	component->initialize(nullptr);

	
	static const Steinberg::FUID inline_processor_iid(Steinberg::Vst::IAudioProcessor_iid);

	if (component->queryInterface(inline_processor_iid,(void**)&processor) != Steinberg::kResultOk) {
		return;
	}

	Steinberg::Vst::ProcessSetup setup;

	setup.processMode = Steinberg::Vst::kRealtime;
	setup.symbolicSampleSize = Steinberg::Vst::kSample32;
	setup.maxSamplesPerBlock = 512;
	setup.sampleRate = 44100.0;

	processor->setupProcessing(setup);

	component->setActive(true);
	processor->setProcessing(true);

	//VST should be loaded. Send audio through it.

}

void VST3Plugin::unloadEffect() {

	if (!this) {
		return;
	}
	if (component) {

		component->setActive(false);
		component->terminate();
		component->release();
	}

	if (processor) {

		processor->setProcessing(false);
		processor->release();
	}
}

void VST3Plugin::openEditor()
{

	Steinberg::Vst::IEditController *editor = nullptr;
	Steinberg::IPlugView *view = nullptr;

	editorWidget.reset(new EditorWidget(nullptr, this));

	static const Steinberg::FUID inline_editor_iid(Steinberg::Vst::IEditController_iid);

	if (component->queryInterface(inline_editor_iid, (void **)&editor) == Steinberg::kResultOk) {
		view = editor->createView(Steinberg::Vst::ViewType::kEditor);

		editorWidget->setMinimumSize(800, 600);

		void *windowHandle = (void *)editorWidget->winId();

		#if defined(_WIN32)
		const char *platformType = Steinberg::kPlatformTypeHWND;
#elif defined(__APPLE__)
		const char *platformType = Steinberg::kPlatformTypeNSView;
#else
		const char *platformType = Steinberg::kPlatformTypeX11EmbedWindowID;
#endif

		if (view->isPlatformTypeSupported(platformType) == Steinberg::kResultOk) {
			view->attached(windowHandle, platformType);
		}
	}

	//	// This check logic is refer to open source project : Audacity
	//	if (!effFlagsHasEditor) {
	//		blog(LOG_WARNING, "VST Plug-in: Can't support edit feature. '%s'", "C:/Program Files/Common Files/VST3");
	//		return;
	//	}

	//	editorOpened = true;
	//	editorWidget = new EditorWidget(nullptr, this);
	//	//editorWidget->buildEffectContainer(effect);

	//	if (sourceName.empty()) {
	//		sourceName = "VST Plugin";
	//	}

	//	if (filterName.empty()) {
	//		editorWidget->setWindowTitle(QString("%1 - %2").arg(sourceName.c_str(), effectName));
	//	} else {
	//		editorWidget->setWindowTitle(
	//			QString("%1: %2 - %3").arg(sourceName.c_str(), filterName.c_str(), effectName));
	//	}
	//	editorWidget->show();
	//}
}

void VST3Plugin::closeEditor() {}
