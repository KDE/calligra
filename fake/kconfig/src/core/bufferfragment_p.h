/*
   This file is part of the KDE libraries
   Copyright (c) 2008 Jakub Stachowski <qbast@go2.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef BUFFERFRAGMENT_H
#define BUFFERFRAGMENT_H

#define bf_isspace(str) ((str == ' ') || (str == '\t') || (str == '\r'))

// This class provides wrapper around fragment of existing buffer (array of bytes). 
// If underlying buffer gets deleted, all BufferFragment objects referencing it become invalid.
// Use toByteArray() to make deep copy of the buffer fragment.
// 
// API is designed to subset of QByteArray methods with some changes:
// - trim() is like QByteArray.trimmed(), but it modifies current object
// - truncateLeft() provides way to cut off beginning of the buffer
// - split() works more like strtok_r than QByteArray.split()
// - truncateLeft() and mid() require position argument to be valid
    
class KConfigIniBackend::BufferFragment 
{
    
public:

    BufferFragment() : d(0), len(0) 
    {
    }
    
    BufferFragment(char* buf, int size) : d(buf), len(size) 
    {
    }

    int length() const 
    {
        return len;
    }

    char at(unsigned int i) const 
    {
        Q_ASSERT(i < len);
        return d[i];
    }

    void clear() 
    {
        len = 0;
    }

    const char* constData() const 
    {
        return d;
    }

    char* data() const 
    {
        return d;
    }

    void trim() 
    {
        while (bf_isspace(*d) && len > 0) {
            d++;
            len--;
        }
        while (len > 0 && bf_isspace(d[len - 1])) 
            len--;
    }

    // similar to strtok_r . On first call variable pointed by start should be set to 0.
    // Each call will update *start to new starting position. 
    BufferFragment split(char c, unsigned int* start) 
    {
        while (*start < len) {
            int end = indexOf(c, *start);
            if (end == -1) end = len;
            BufferFragment line(d + (*start), end - (*start));
            *start = end + 1;
            return line;
        }
        return BufferFragment();
    }
    
    bool isEmpty() const 
    {
        return (len == 0);
    }

    BufferFragment left(unsigned int size) const 
    {
        return BufferFragment(d, qMin(size,len));
    }

    void truncateLeft(unsigned int size) 
    {
        Q_ASSERT(size <= len);
        d += size;
        len -= size;
    }

    void truncate(unsigned int pos) 
    {
        if (pos < len) len = pos;
    }

    bool isNull() const 
    {
        return (d == 0);
    }
        
    BufferFragment mid(unsigned int pos, int length=-1) const 
    {
        Q_ASSERT(pos < len);
        int size = length;
        if (length == -1 || (pos + length) > len) 
            size = len - pos;
        return BufferFragment(d + pos, size);
    }

    bool operator==(const QByteArray& other) const
    {
        return (other.size() == (int)len && memcmp(d,other.constData(),len) == 0);
    }

    bool operator!=(const QByteArray& other) const 
    {
        return (other.size() != (int)len || memcmp(d,other.constData(),len) != 0);
    }
    
    int indexOf(char c, unsigned int from = 0) const 
    {
        const char* cursor = d + from - 1;
        const char* end = d + len;
        while ( ++cursor < end) 
            if (*cursor ==c ) 
                return cursor - d; 
        return -1;
    }

    int lastIndexOf(char c) const 
    {
        int from = len - 1;
        while (from >= 0) 
            if (d[from] == c) 
                return from; 
            else 
                from--;
        return -1;
    }

    QByteArray toByteArray() const {
        return QByteArray(d,len);
    }
    
    // this is faster than toByteArray, but returned QByteArray becomes invalid
    // when buffer for this BufferFragment disappears
    QByteArray toVolatileByteArray() const {
        return QByteArray::fromRawData(d, len);
    }

private:
    char* d;
    unsigned int len;
};

#endif
