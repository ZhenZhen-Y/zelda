#ifndef ZELDA_CSV_H
#define ZELDA_CSV_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
public:
    string const& operator[](size_t index) const
    {
        return m_data[index];
    }
    size_t size() const
    {
        return m_data.size();
    }
    void readNextRow(istream& str)
    {
        string         line;
        getline(str, line);

        stringstream   lineStream(line);
        string         cell;

        m_data.clear();
        while(getline(lineStream, cell, ','))
        {
            m_data.push_back(cell);
        }
        // This checks for a trailing comma with no data after it.
        if (!lineStream && cell.empty())
        {
            // If there was a trailing comma then add an empty element.
            m_data.push_back("");
        }
    }
private:
    vector<string>    m_data;
};

inline istream& operator>>(istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

class CSVIterator
{
public:
    typedef input_iterator_tag     iterator_category;
    typedef CSVRow                      value_type;
    typedef size_t                 difference_type;
    typedef CSVRow*                     pointer;
    typedef CSVRow&                     reference;

    CSVIterator(istream& str)  :m_str(str.good()?&str:NULL) { ++(*this); }
    CSVIterator()                   :m_str(NULL) {}

    // Pre Increment
    CSVIterator& operator++()               {if (m_str) { if (!((*m_str) >> m_row)){m_str = NULL;}}return *this;}
    // Post increment
    CSVIterator operator++(int)             {CSVIterator    tmp(*this);++(*this);return tmp;}
    CSVRow const& operator*()   const       {return m_row;}
    CSVRow const* operator->()  const       {return &m_row;}

    bool operator==(CSVIterator const& rhs) {return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));}
    bool operator!=(CSVIterator const& rhs) {return !((*this) == rhs);}
private:
    istream*       m_str;
    CSVRow              m_row;
};

#endif //ZELDA_CSV_H
