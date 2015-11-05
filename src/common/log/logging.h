// Copyright (c) 2014, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: yanshiguang02@baidu.com

#ifndef  TERA_COMMON_LOGGING_H_
#define  TERA_COMMON_LOGGING_H_

namespace common {

enum LogLevel {
    DEBUG = 2,
    INFO = 4,
    WARNING = 8,
    FATAL = 16,
};

void SetLogLevel(int level);
bool SetLogFile(const char* path, bool append = false);
bool SetWarningFile(const char* path, bool append = false);

void Log(int level, const char* fmt, ...);

} // namespace common

using common::DEBUG;
using common::INFO;
using common::WARNING;
using common::FATAL;

#define LOG(level, fmt, args...) common::Log(level, "[%s:%d] "fmt, __FILE__, __LINE__, ##args)

#endif  // TERA_COMMON_LOGGING_H_
