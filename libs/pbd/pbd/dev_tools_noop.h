/*
    Copyright (C) 2017 Tim Mayberry

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

#ifndef __libpbd_dev_tools_noop_h__
#define __libpbd_dev_tools_noop_h__

// logging_macros.hpp

#define A_DECLARE_LOG_CATEGORY(VariableName)

#define A_DEFINE_LOG_CATEGORY(VariableName, LogCategoryName)

#define A_LOG_MSG(LogCategoryPtr, Message)

#define A_LOG_CLASS_MSG(LogCategoryPtr, Message)

#define A_LOG_DURATION(LogCategoryPtr, Message)

#define A_LOG_CLASS_DURATION(LogCategoryPtr, Message)

#define A_LOG_CALL(LogCategoryPtr)
#define A_LOG_CALL1(LogCategoryPtr, Arg1)
#define A_LOG_CALL2(LogCategoryPtr, Arg1, Arg2)
#define A_LOG_CALL3(LogCategoryPtr, Arg1, Arg2, Arg3)
#define A_LOG_CALL4(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4)
#define A_LOG_CALL5(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5)
#define A_LOG_CALL6(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
#define A_LOG_CALL7(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)
#define A_LOG_CALL8(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_LOG_CLASS_CALL(LogCategoryPtr)
#define A_LOG_CLASS_CALL1(LogCategoryPtr, Arg1)
#define A_LOG_CLASS_CALL2(LogCategoryPtr, Arg1, Arg2)
#define A_LOG_CLASS_CALL3(LogCategoryPtr, Arg1, Arg2, Arg3)
#define A_LOG_CLASS_CALL4(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4)
#define A_LOG_CLASS_CALL5(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5)
#define A_LOG_CLASS_CALL6(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
#define A_LOG_CLASS_CALL7(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)
#define A_LOG_CLASS_CALL8(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_LOG_DATA1(LogCategoryPtr, Arg1)
#define A_LOG_DATA2(LogCategoryPtr, Arg1, Arg2)
#define A_LOG_DATA3(LogCategoryPtr, Arg1, Arg2, Arg3)
#define A_LOG_DATA4(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4)
#define A_LOG_DATA5(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5)
#define A_LOG_DATA6(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
#define A_LOG_DATA7(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)
#define A_LOG_DATA8(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_LOG_CLASS_DATA1(LogCategoryPtr, Arg1)
#define A_LOG_CLASS_DATA2(LogCategoryPtr, Arg1, Arg2)
#define A_LOG_CLASS_DATA3(LogCategoryPtr, Arg1, Arg2, Arg3)
#define A_LOG_CLASS_DATA4(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4)
#define A_LOG_CLASS_DATA5(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5)
#define A_LOG_CLASS_DATA6(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
#define A_LOG_CLASS_DATA7(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)
#define A_LOG_CLASS_DATA8(LogCategoryPtr, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_LOG_ENABLED(LogCategoryPtr)

#define A_FMT(...)

// class_logging_macros.hpp

#define A_CLASS_MSG(Message) A_LOG_CLASS_MSG(class_category, Message)

#define A_CLASS_STATIC_MSG(Message) A_LOG_MSG(class_category, Message)

#define A_CLASS_DURATION(Message) A_LOG_CLASS_DURATION(class_category, Message)

#define A_CLASS_STATIC_DURATION(Message) A_LOG_DURATION(class_category, Message)

#define A_CLASS_CALL() A_LOG_CLASS_CALL(class_category)

#define A_CLASS_CALL1(Arg1) A_LOG_CLASS_CALL1(class_category, Arg1)

#define A_CLASS_CALL2(Arg1, Arg2) A_LOG_CLASS_CALL2(class_category, Arg1, Arg2)

#define A_CLASS_CALL3(Arg1, Arg2, Arg3)                                        \
	A_LOG_CLASS_CALL3(class_category, Arg1, Arg2, Arg3)

#define A_CLASS_CALL4(Arg1, Arg2, Arg3, Arg4)                                  \
	A_LOG_CLASS_CALL4(class_category, Arg1, Arg2, Arg3, Arg4)

#define A_CLASS_CALL5(Arg1, Arg2, Arg3, Arg4, Arg5)                            \
	A_LOG_CLASS_CALL5(class_category, Arg1, Arg2, Arg3, Arg4, Arg5)

#define A_CLASS_CALL6(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)                      \
	A_LOG_CLASS_CALL6(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)

#define A_CLASS_CALL7(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)                \
	A_LOG_CLASS_CALL7(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)

#define A_CLASS_CALL8(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)          \
	A_LOG_CLASS_CALL8(                                                            \
	    class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_CLASS_STATIC_CALL() A_LOG_CALL(class_category)

#define A_CLASS_STATIC_CALL1(Arg1) A_LOG_CALL1(class_category, Arg1)

#define A_CLASS_STATIC_CALL2(Arg1, Arg2) A_LOG_CALL2(class_category, Arg1, Arg2)

#define A_CLASS_STATIC_CALL3(Arg1, Arg2, Arg3)                                 \
	A_LOG_CALL3(class_category, Arg1, Arg2, Arg3)

#define A_CLASS_STATIC_CALL4(Arg1, Arg2, Arg3, Arg4)                           \
	A_LOG_CALL4(class_category, Arg1, Arg2, Arg3, Arg4)

#define A_CLASS_STATIC_CALL5(Arg1, Arg2, Arg3, Arg4, Arg5)                     \
	A_LOG_CALL5(class_category, Arg1, Arg2, Arg3, Arg4, Arg5)

#define A_CLASS_STATIC_CALL6(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)               \
	A_LOG_CALL6(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)

#define A_CLASS_STATIC_CALL7(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)         \
	A_LOG_CALL7(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)

#define A_CLASS_STATIC_CALL8(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)   \
	A_LOG_CALL7(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_CLASS_DATA1(Arg1) A_LOG_CLASS_DATA1(class_category, Arg1)

#define A_CLASS_DATA2(Arg1, Arg2) A_LOG_CLASS_DATA2(class_category, Arg1, Arg2)

#define A_CLASS_DATA3(Arg1, Arg2, Arg3)                                        \
	A_LOG_CLASS_DATA3(class_category, Arg1, Arg2, Arg3)

#define A_CLASS_DATA4(Arg1, Arg2, Arg3, Arg4)                                  \
	A_LOG_CLASS_DATA4(class_category, Arg1, Arg2, Arg3, Arg4)

#define A_CLASS_DATA5(Arg1, Arg2, Arg3, Arg4, Arg5)                            \
	A_LOG_CLASS_DATA5(class_category, Arg1, Arg2, Arg3, Arg4, Arg5)

#define A_CLASS_DATA6(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)                      \
	A_LOG_CLASS_DATA6(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)

#define A_CLASS_DATA7(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)                \
	A_LOG_CLASS_DATA7(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)

#define A_CLASS_DATA8(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)          \
	A_LOG_CLASS_DATA8(                                                            \
	    class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

#define A_CLASS_STATIC_DATA1(Arg1) A_LOG_DATA1(class_category, Arg1)

#define A_CLASS_STATIC_DATA2(Arg1, Arg2) A_LOG_DATA2(class_category, Arg1, Arg2)

#define A_CLASS_STATIC_DATA3(Arg1, Arg2, Arg3)                                 \
	A_LOG_DATA3(class_category, Arg1, Arg2, Arg3)

#define A_CLASS_STATIC_DATA4(Arg1, Arg2, Arg3, Arg4)                           \
	A_LOG_DATA4(class_category, Arg1, Arg2, Arg3, Arg4)

#define A_CLASS_STATIC_DATA5(Arg1, Arg2, Arg3, Arg4, Arg5)                     \
	A_LOG_DATA5(class_category, Arg1, Arg2, Arg3, Arg4, Arg5)

#define A_CLASS_STATIC_DATA6(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)               \
	A_LOG_DATA6(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)

#define A_CLASS_STATIC_DATA7(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)         \
	A_LOG_DATA7(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)

#define A_CLASS_STATIC_DATA8(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)   \
	A_LOG_DATA8(class_category, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)

// class_member_macros.hpp

#define A_DECLARE_CLASS_MEMBERS(ClassType)
#define A_DEFINE_CLASS_MEMBERS(ClassType)
#define A_DEFINE_CLASS_AS_MEMBERS(ClassType, ClassName)

// thread_macros.hpp
//
#define A_REGISTER_THREAD(Name, Priority)

#endif // __libpbd_dev_tools_noop_h__
