#ifndef PBD_LOGGING_H
#define PBD_LOGGING_H

#include "pbd/dev_tools.h"

#ifdef WITH_DEV_TOOLS

namespace PBD {
namespace LOG {

A_DECLARE_LOG_CATEGORY(AbstractUI);
A_DECLARE_LOG_CATEGORY(Configuration);

} // namespace LOG
} // namespace PBD

#endif // WITH_DEV_TOOLS

#endif // PBD_LOGGING_H
