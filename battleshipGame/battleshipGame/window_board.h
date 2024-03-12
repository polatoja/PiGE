#pragma once
#define NOMINMAX
#include <array>
#include <windows.h>

struct field
{
    RECT position;
};

class board
{
public:
    static constexpr LONG columns = 10;
    static constexpr LONG rows = 10;
    static constexpr LONG margin = 5;
    static constexpr LONG field_count = rows * columns;
    static constexpr LONG field_size = 30;
    static constexpr LONG width =
        columns * (field_size + margin) + margin * 2;
    static constexpr LONG height =
        rows * (field_size + margin) + margin * 2;
    using field_array = std::array<field, field_count>;
    board();
    field_array const& fields() const { return m_fields; }
private:
    field_array m_fields;
};
