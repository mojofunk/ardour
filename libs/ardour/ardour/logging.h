#ifndef ARDOUR_LOGGING_H
#define ARDOUR_LOGGING_H

#include "pbd/dev_tools.h"

#ifdef WITH_DEV_TOOLS
namespace ARDOUR {

namespace LOG {

adt::LogString midi_event_to_string (size_t size, uint8_t const* data);

A_DECLARE_LOG_CATEGORY(Butler);
A_DECLARE_LOG_CATEGORY(Export);
A_DECLARE_LOG_CATEGORY(Graph);
A_DECLARE_LOG_CATEGORY(Import);
A_DECLARE_LOG_CATEGORY(Processors);
A_DECLARE_LOG_CATEGORY(ProcessThreads);
A_DECLARE_LOG_CATEGORY(Selection);
A_DECLARE_LOG_CATEGORY(Session);
A_DECLARE_LOG_CATEGORY(SessionCleanup);
A_DECLARE_LOG_CATEGORY(SessionEvents);
A_DECLARE_LOG_CATEGORY(SessionHistory);
A_DECLARE_LOG_CATEGORY(Serialization);
A_DECLARE_LOG_CATEGORY(Solo);
A_DECLARE_LOG_CATEGORY(Transport);

struct ProcessingPreset : public adt::Preset {
	ProcessingPreset () : Preset ("ARDOUR::Processing")
	{
		category_names = {"Engine", "Process", "Butler", "Graph"};
		enable_message_type = true;
		enable_function_type = true;
		enable_data_type = true;
	}

	static std::shared_ptr<adt::Preset>
	create ()
	{
		return std::shared_ptr<adt::Preset> (new ProcessingPreset);
	}
};

struct IOPreset : public adt::Preset {
	IOPreset () : Preset ("ARDOUR::IO")
	{
		category_names = {"Butler", "Source", "Diskstream"};
		enable_message_type = true;
		enable_function_type = true;
		enable_data_type = true;
	}

	static std::shared_ptr<adt::Preset>
	create ()
	{
		return std::shared_ptr<adt::Preset> (new IOPreset);
	}
};

struct ProcessorsPreset : public adt::Preset {
	ProcessorsPreset () : Preset ("ARDOUR::Processors")
	{
		category_names = {"Amp", "DelayLine", "Delivery", "Meter", "PannerShell", "PeakMeter", "PortInsert", "PluginInsert", "SideChain", "UnknownProcessor"};
		enable_message_type = true;
		enable_function_type = true;
		enable_data_type = true;
	}

	static std::shared_ptr<adt::Preset>
	create ()
	{
		return std::shared_ptr<adt::Preset> (new ProcessorsPreset);
	}
};

struct PluginsPreset : public adt::Preset {
	PluginsPreset () : Preset ("ARDOUR::Plugins")
	{
		category_names = {"Process", "Plugin"};
		enable_message_type = true;
		enable_function_type = true;
		enable_data_type = true;
	}

	static std::shared_ptr<adt::Preset>
	create ()
	{
		return std::shared_ptr<adt::Preset> (new PluginsPreset);
	}
};

} // namespace LOG

} // namespace ARDOUR
#endif

#endif // ARDOUR_LOGGING_H
