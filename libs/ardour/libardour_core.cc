#include "chan_count.cc" // at top because of conflict with INFINITE via windows.h
#include "chan_mapping.cc"

#include "disk_reader.cc"
#include "disk_writer.cc"

#include "disk_io.cc"

#include "buffer.cc"
#include "buffer_manager.cc"
#include "buffer_set.cc"

#include "bundle.cc"
#include "control_protocol_manager.cc"
#include "data_type.cc"
#include "debug.cc"

#include "io.cc"
#include "location.cc"
#include "mute_master.cc"
#include "operations.cc"
#include "parameter_descriptor.cc"

#include "source.cc"
#include "source_factory.cc"
#include "file_source.cc"
#include "resampled_source.cc"

#include "region.cc"
#include "region_factory.cc"

#include "playlist.cc"
#include "playlist_source.cc"
#include "playlist_factory.cc"

#include "presentation_info.cc"
#include "stripable.cc"

#include "fixed_delay.cc"

#include "session_object.cc"
#include "scene_change.cc"
#include "speakers.cc"

#include "tempo.cc"
#include "user_bundle.cc"

#include "selection.cc"

#ifdef WITH_DEV_TOOLS
#include "logging.cc"
#endif
