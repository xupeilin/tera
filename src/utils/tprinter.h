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
        // if print table header
        bool print_head;
        // if print average row
        bool print_avg;
        // if print sum row
        bool print_sum;
        // >0 for ascending order, <0 for decending order
        int  sort_dir;
        // select column num for sorting
        int  sort_col_num;
        // select column name for sorting
        string sort_col_name;

        PrintOpt();
    };

    class Line {
    public:
        size_t size() { return cells_.size(); }
        void push_back (int64_t cell);
        void push_back (double cell);
        void push_back (const string& cell);
        void push_back (Cell& cell);
        Cell& operator[](int offset) { return cells_[i]; }

    private:
        std::vector<Cell> cells_;
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
    enum Type {
        INT = 0x10,
        INT_K = 0x11,
        INT_KI = 0x12,
        DOUBLE = 0x20,
        STRING = 0x30
    }
    class Cell {
    public:
        Cell (int64_t v) { value.i = v; type = C_INT; }
        Cell (double  v) { value.d = v; type = C_DOUBLE; }
        Cell (const string& v) { value.s = new string(v); type = C_STRING; }
        Cell (const Cell& ref) { *this = ref; }
        ~Cell () { if (type == C_STRING) delete value.s; }

        string ToString(Type type);

        Cell& operator=(const Cell& ref);

    private:
        enum CellType { C_INT, C_DOUBLE, C_STRING };
        CellType type_;
        union {
            int64_t i;
            double  d;
            string* s;
        } value_;
    };

    // column format: "name<int[,unit]>"
    // e.g. "name<string>", "money<int,yuan>", "speed<int_1024,B>"
    bool ParseColType(const string& item, string* name,
                      Type* type, string* unit = NULL);
    void FormatOneLine(Line& ori, std::vector<string>* dst);
    static string NumToStr(const double num);
    static string NumToStrK(const double num);
    static string NumToStrKi(const double num);

private:
    int cols_;
    std::vector<string> head_;
    std::vector<Type> type_;
    std::vector<string> unit_;
    std::vector<Line> body_;
    std::vector<size_t> col_width_;
};

} // namespace tera
#endif // TERA_UTILS_T_PRINTER_H_
