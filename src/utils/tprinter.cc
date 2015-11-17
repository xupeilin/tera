// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: Xu Peilin (xupeilin@baidu.com)

#include "tprinter.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace tera {

TPrinter::TPrinter() : cols_(0), col_width_(cols_) {
}

TPrinter::TPrinter(int cols, ...) : cols_(cols), col_width_(cols_) {
    assert (cols > 0);
    va_list args;
    va_start(args, cols);
    for (int i = 0; i < cols; ++i) {
        string item = va_arg(args, char*);
        string name, unit;
        CellType type;
        if (!ParseColType(item, &name, &type, &unit)) {
            name = item;
            type = STRING;
            unit = "";
        }
        head_.push_back(name);
        type_.push_back(type);
        unit_.push_back(unit);
        col_width_[i] = name.size();
    }
    va_end(args);
}

TPrinter::~TPrinter() {
}

bool TPrinter::AddRow(int cols, ...) {
    if (cols != cols_) {
        return false;
    }
    Line line;
    va_list args;
    va_start(args, cols);
    for (int i = 0; i < cols; ++i) {
        switch (type_[i]) {
        case INT:
            line.push_back(Cell((int64_t)va_arg(args, int64_t), INT));
            break;
        case DOUBLE:
            line.push_back(Cell((double)va_arg(args, double), DOUBLE));
            break;
        case STRING:
            line.push_back(Cell((char*)va_arg(args, char*), STRING));
            break;
        default:
            abort();
        }
    }
    va_end(args);
    FormatOneLine(line, NULL);   // modify column width
    body_.push_back(line);
    return true;
}

bool TPrinter::AddRow(const std::vector<string>& row) {
    if ((int)row.size() != cols_) {
        return false;
    }
    Line line;
    for (int i = 0; i < cols_; ++i) {
        line.push_back(Cell(row[i], STRING));
    }
    FormatOneLine(line, NULL);   // modify column width only
    body_.push_back(line);
    return true;
}

bool TPrinter::AddRow(const std::vector<int64_t>& row) {
    if ((int)row.size() != cols_) {
        return false;
    }
    Line line;
    for (int i = 0; i < cols_; ++i) {
        line.push_back(Cell(row[i], INT));
    }
    FormatOneLine(line, NULL);   // modify column width only
    body_.push_back(line);
    return true;
}

void TPrinter::Print(const PrintOpt& opt) {
    std::cout << ToString(opt);
}

string TPrinter::ToString(const PrintOpt& opt) {
    std::ostringstream ostr;
    if (head_.size() < 1) {
        return "";
    }
    if (opt.print_head) {
        int line_len = 0;
        for (int i = 0; i < cols_; ++i) {
            line_len += 2 + col_width_[i];
            ostr << "  " << std::setfill(' ')
                << std::setw(col_width_[i])
                << std::setiosflags(std::ios::left)
                << head_[i];
        }
        ostr << std::endl;
        for (int i = 0; i < line_len + 2; ++i) {
            ostr << "-";
        }
        ostr << std::endl;
    }

    for (size_t i = 0; i < body_.size(); ++i) {
        std::vector<string> line;
        FormatOneLine(body_[i], &line);
        for (int j = 0; j < cols_; ++j) {
            ostr << "  " << std::setfill(' ')
                << std::setw(col_width_[j])
                << std::setiosflags(std::ios::left)
                << line[j];
        }
        ostr << std::endl;
    }
    return ostr.str();
}

void TPrinter::Reset(int cols, ...) {
    assert (cols > 0);
    cols_ = cols;
    col_width_.resize(cols_, 0);
    head_.clear();
    type_.clear();
    body_.clear();

    va_list args;
    va_start(args, cols);
    for (int i = 0; i < cols; ++i) {
        string item = va_arg(args, char*);
        string name, unit;
        CellType type;
        if (!ParseColType(item, &name, &type, &unit)) {
            name = item;
            type = STRING;
            unit = "";
        }
        head_.push_back(name);
        type_.push_back(type);
        unit_.push_back(unit);
        col_width_[i] = name.size();
    }
    va_end(args);
}

void TPrinter::Reset(const std::vector<string>& row) {
    assert (row.size() > 0);
    cols_ = row.size();
    col_width_.resize(cols_, 0);
    head_.clear();
    type_.clear();
    unit_.clear();
    body_.clear();

    for (int i = 0; i < cols_; ++i) {
        head_.push_back(row[i]);
        type_.push_back(STRING);
        unit_.push_back("");
        col_width_[i] = row[i].size();
    }
}

bool TPrinter::ParseColType(const string& item, string* name,
                            CellType* type, string* unit) {
    string::size_type pos1;
    pos1 = item.find('<');
    if (pos1 == string::npos) {
        return false;
    }
    if (item[item.size() - 1] != '>') {
        return false;
    }
    string type_unit = item.substr(pos1 + 1, item.size() - pos1 - 2);
    string::size_type pos2 = type_unit.find(',');
    string type_str, unit_str;
    if (pos2 == string::npos) {
        type_str = type_unit;
        unit_str = "";
    } else {
        type_str = type_unit.substr(0, pos2);
        unit_str = type_unit.substr(pos2 + 1);
    }

    if (type_str == "int") {
        *type = INT;
    } else if (type_str == "double") {
        *type = DOUBLE;
    } else if (type_str == "string") {
        *type = STRING;
    } else {
        return false;
    }

    if (name) {
        *name = item.substr(0, pos1);
    }
    if (unit) {
        *unit = unit_str;
    }
    return true;
}

void TPrinter::FormatOneLine(Line& ori, std::vector<string>* dst) {
    if (dst) {
        dst->clear();
    }
    for (size_t i = 0; i < ori.size(); ++i) {
        string str = ori[i].ToString();
        if (ori[i].type == type_[i]) {
            str.append(unit_[i]);
        }
        if (col_width_[i] < str.size()) {
            col_width_[i] = str.size();
        }
        if (dst) {
            dst->push_back(str);
        }
    }
}

string TPrinter::NumToStr(const double num, Type type) {
    const int64_t kKB = 1000;
    const int64_t kMB = kKB * 1000;
    const int64_t kGB = kMB * 1000;
    const int64_t kTB = kGB * 1000;
    const int64_t kPB = kTB * 1000;

    string unit;
    double res;
    if (num > kPB) {
        res = (1.0 * num) / kPB;
        unit = "P";
    } else if (num > kTB) {
        res = (1.0 * num) / kTB;
        unit = "T";
    } else if (num > kGB) {
        res = (1.0 * num) / kGB;
        unit = "G";
    } else if (num > kMB) {
        res = (1.0 * num) / kMB;
        unit = "M";
    } else if (num > kKB) {
        res = (1.0 * num) / kKB;
        unit = "K";
    } else {
        res = num;
        unit = "";
    }
    const int buflen = 16;
    char buf[buflen];
    if ((int)res - res == 0) {
        snprintf(buf, buflen, "%d%s", (int)res, unit.c_str());
    } else {
        snprintf(buf, buflen, "%.2f%s", res, unit.c_str());
    }
    return string(buf);
}

TPrinter::PrintOpt::PrintOpt()
    : print_head(true),
      print_avg(false),
      print_sum(false),
      sort_dir(0),
      sort_col_num(0),
      sort_col_name("") {
}

string TPrinter::Cell::ToString(Type type) {
    switch (type) {
    case INT: {
        if (type_ != C_INT) {
            return "-"
        }
        return NumToStr(value.i);
    }
    case INT_K: {
        if (type_ != C_INT) {
            return "-"
        }
        return NumToStrK(value.i);
    }
    case INT_KI: {
        if (type_ != C_INT) {
            return "-"
        }
        return NumToStrKi(value.i);
    }
    case DOUBLE: {
        if (type_ != C_DOUBLE) {
            return "-"
        }
        return NumToStrK(value.d);
    }
    case STRING: {
        if (type_ != C_STRING) {
            return "-"
        }
        return *value.s;
    }
    default:
        abort();
    }
}

TPrinter::Cell& TPrinter::Cell::operator=(const TPrinter::Cell& ref) {
    type_ = ref.type_;
    if (type_ == STRING && this != &ref) {
        value_.s = new string(*ref.value_.s);
    } else {
        value_ = ref.value_;
    }
    return *this;
}
} // namespace tera
