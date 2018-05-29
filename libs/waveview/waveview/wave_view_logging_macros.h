#ifndef __CANVAS_WAVE_VIEW_LOGGING_MACROS_H__
#define __CANVAS_WAVE_VIEW_LOGGING_MACROS_H__

#define LOG_PROPERTIES(props)                                                                      \
	A_LOG_DATA8 (LOG::WaveViewProperties, props.region_start, props.region_end,                      \
	             props.get_sample_start (), props.get_sample_end (), props.channel,                  \
	             props.get_width_pixels (), props.height, props.samples_per_pixel);                  \
	A_LOG_DATA8 (LOG::WaveViewProperties, props.amplitude, props.amplitude_above_axis,               \
	             props.fill_color, props.outline_color, props.zero_color, props.show_zero,           \
	             props.clip_color, props.show_zero);                                                 \
	A_LOG_DATA2 (LOG::WaveViewProperties, props.logscaled, props.shape);

#define LOG_DIFF(val1, val2)                                                                       \
	if (val1 != val2) {                                                                              \
		A_LOG_DATA2 (LOG::WaveViewProperties, val1, val2);                                             \
	}

#define LOG_PROPERTY_DIFF(prop1, prop2)                                                            \
	LOG_DIFF (prop1.region_start, prop2.region_start);                                               \
	LOG_DIFF (prop1.region_end, prop2.region_end);                                                   \
	LOG_DIFF (prop1.get_sample_start (), prop2.get_sample_start ());                                 \
	LOG_DIFF (prop1.get_sample_end (), prop2.get_sample_end ());                                     \
	LOG_DIFF (prop1.channel, prop2.channel);                                                         \
	LOG_DIFF (prop1.get_length_samples (), prop2.get_length_samples ());                             \
	LOG_DIFF (prop1.height, prop2.height);                                                           \
	LOG_DIFF (prop1.samples_per_pixel, prop2.samples_per_pixel);                                     \
	LOG_DIFF (prop1.amplitude, prop2.amplitude);                                                     \
	LOG_DIFF (prop1.amplitude_above_axis, prop2.amplitude_above_axis);                               \
	LOG_DIFF (prop1.fill_color, prop2.fill_color);                                                   \
	LOG_DIFF (prop1.zero_color, prop2.zero_color);                                                   \
	LOG_DIFF (prop1.clip_color, prop2.clip_color);                                                   \
	LOG_DIFF (prop1.show_zero, prop2.show_zero);                                                     \
	LOG_DIFF (prop1.logscaled, prop2.logscaled);                                                     \
	LOG_DIFF (prop1.shape, prop2.shape);                                                             \
	LOG_DIFF (prop1.gradient_depth, prop2.gradient_depth);                                           \
	LOG_DIFF (prop1.start_shift, prop2.start_shift);

#endif // __CANVAS_WAVE_VIEW_LOGGING_MACROS_H__
