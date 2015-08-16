/*
    Copyright (C) 2015 Tim Mayberry

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef ARDOUR_TYPES_CONVERT_H
#define ARDOUR_TYPES_CONVERT_H

#include "pbd/enum_convert.h"

#include "ardour/types.h"

namespace PBD {

DEFINE_ENUM_CONVERT(Timecode::TimecodeFormat)

DEFINE_ENUM_CONVERT(ARDOUR::SampleFormat)
DEFINE_ENUM_CONVERT(ARDOUR::HeaderFormat)
DEFINE_ENUM_CONVERT(ARDOUR::AutoConnectOption)
DEFINE_ENUM_CONVERT(ARDOUR::TracksAutoNamingRule)
DEFINE_ENUM_CONVERT(ARDOUR::EditMode)
DEFINE_ENUM_CONVERT(ARDOUR::MonitorModel)
DEFINE_ENUM_CONVERT(ARDOUR::AFLPosition)
DEFINE_ENUM_CONVERT(ARDOUR::PFLPosition)
DEFINE_ENUM_CONVERT(ARDOUR::RemoteModel)
DEFINE_ENUM_CONVERT(ARDOUR::ListenPosition)
DEFINE_ENUM_CONVERT(ARDOUR::LayerModel)
DEFINE_ENUM_CONVERT(ARDOUR::InsertMergePolicy)
DEFINE_ENUM_CONVERT(ARDOUR::SyncSource)
DEFINE_ENUM_CONVERT(ARDOUR::ShuttleBehaviour)
DEFINE_ENUM_CONVERT(ARDOUR::ShuttleUnits)
DEFINE_ENUM_CONVERT(ARDOUR::DenormalModel)
DEFINE_ENUM_CONVERT(ARDOUR::PositionLockStyle)
DEFINE_ENUM_CONVERT(ARDOUR::FadeShape)
DEFINE_ENUM_CONVERT(ARDOUR::RegionSelectionAfterSplit)
DEFINE_ENUM_CONVERT(ARDOUR::BufferingPreset)
DEFINE_ENUM_CONVERT(ARDOUR::AutoReturnTarget)
DEFINE_ENUM_CONVERT(ARDOUR::MeterType)

DEFINE_ENUM_CONVERT(ARDOUR::WaveformScale)
DEFINE_ENUM_CONVERT(ARDOUR::WaveformShape)
DEFINE_ENUM_CONVERT(ARDOUR::VUMeterStandard)
DEFINE_ENUM_CONVERT(ARDOUR::MeterLineUp)

} // namespace PBD

#endif // ARDOUR_TYPES_CONVERT_H
