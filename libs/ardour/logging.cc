#include "ardour/logging.h"

namespace ARDOUR {

namespace LOG {

A_DEFINE_LOG_CATEGORY (Butler, "ARDOUR::Butler");
A_DEFINE_LOG_CATEGORY (Export, "ARDOUR::Export");
A_DEFINE_LOG_CATEGORY (Graph, "ARDOUR::Graph");
A_DEFINE_LOG_CATEGORY (Import, "ARDOUR::Import");
A_DEFINE_LOG_CATEGORY (Processors, "ARDOUR::Processors");
A_DEFINE_LOG_CATEGORY (ProcessThreads, "ARDOUR::ProcessThreads");
A_DEFINE_LOG_CATEGORY (Selection, "ARDOUR::Selection");
A_DEFINE_LOG_CATEGORY (Session, "ARDOUR::Session");
A_DEFINE_LOG_CATEGORY (SessionEvents, "ARDOUR::SessionEvents");
A_DEFINE_LOG_CATEGORY (SessionCleanup, "ARDOUR::SessionCleanup");
A_DEFINE_LOG_CATEGORY (Serialization, "ARDOUR::Serialization");
A_DEFINE_LOG_CATEGORY (Solo, "ARDOUR::Solo");
A_DEFINE_LOG_CATEGORY (Transport, "ARDOUR::Transport");
A_DEFINE_LOG_CATEGORY (SessionHistory, "ARDOUR::SessionHistory");

adt::LogString midi_event_to_string (size_t size, uint8_t const* data)
{
	if (size == 1) {
		return A_FMT ("{:#x}", data[0]);
	} else if (size == 2) {
		return A_FMT ("{:#x} {:#x}", data[0], data[1]);
	} else if (size == 3) {
		return A_FMT ("{:#x} {:#x} {:#x}", data[0], data[1], data[2]);
	} else if (size > 3) {
		return A_FMT ("size {}, first 3 bytes: {:#x} {:#x} {:#x}", size, data[0], data[1], data[2]);
	}
	return adt::LogString();
}

} // namespace LOG

} // namespace ARDOUR
