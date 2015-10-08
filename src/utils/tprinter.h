// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: Xu Peilin (xupeilin@baidu.com)

#ifndef  TERA_UTILS_T_PRINTER_H_
#define  TERA_UTILS_T_PRINTER_H_

#include <stdint.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::string;

namespace tera {

class TPrinter {
public:
    struct PrintOpt {
    public:
        // if print table header
        bool print_head;
        // >0 for ascending order, <0 for decending order
        int  sort_dir;
        // select column num for sorting
        int  sort_col_num;
        // select column name for sorting
        string sort_col_name;

        PrintOpt()
            : print_head(true), sort_dir(0),
              sort_col_num(0), sort_col_name("") {}
    };

    TPrinter();
    TPrinter(int cols, ...);
    ~TPrinter();

    bool AddRow(int cols, ...);
    bool AddRow(const std::vector<string>& row);
    bool AddRow(const std::vector<int64_t>& row);

    void Print(const PrintOpt& opt = PrintOpt());

    string ToString(const PrintOpt& opt = PrintOpt());

    void Reset(int cols, ...);
    void Reset(const std::vector<string>& head);

private:
    enum CellType {
        INT,
        DOUBLE,
        STRING
    };
    struct Cell {
        CellType type;
        union {
            int64_t i;
            double  d;
            string* s;
        } value;

        string ToString();

        Cell (int64_t v,       CellType t) { value.i = v; type = t; }
        Cell (double  v,       CellType t) { value.d = v; type = t; }
        Cell (const string& v, CellType t) { value.s = new string(v); type = t; }
        Cell (const Cell& ref) { *this = ref; }
        ~Cell () { if (type == STRING) delete value.s; }
        Cell& operator=(const Cell& ref);
    };
    typedef std::vector<Cell> Line;

    // column format: "name<int[,unit]>"
    // e.g. "name<string>", "money<int,yuan>", "speed<int_1024,B>"
    bool ParseColType(const string& item, string* name,
                      CellType* type, string* unit = NULL);
    void FormatOneLine(Line& ori, std::vector<string>* dst);
    static string NumToStr(const double num);

private:
    int cols_;
    std::vector<string> head_;
    std::vector<CellType> type_;
    std::vector<string> unit_;
    std::vector<Line> body_;
    std::vector<size_t> col_width_;
};

} // namespace tera
#endif // TERA_UTILS_T_PRINTER_H_
