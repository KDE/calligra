#ifndef LEINPUT_H
#define LEINPUT_H

#include <QtCore/QtEndian>
#include <QtCore/QByteArray>
#include <QtCore/QDebug>

template <typename T>
const T*
toPtr(const T& t) {
    return (t.isValid()) ?&t :0;
}

class FixedSizeParsedObject {
private:
    const char* _data;
protected:
    explicit FixedSizeParsedObject() :_data(0) {}
    inline void init(const char* data) {
        _data = data;
    }
public:
    inline const char* getData() const { return _data; }
    inline bool isValid() const { return _data; }
    inline bool operator==(const FixedSizeParsedObject& o) {
        return _data && o._data && _data == o._data;
    }
};

class ParsedObject : public FixedSizeParsedObject {
private:
    quint32 _size;
protected:
    explicit ParsedObject() :FixedSizeParsedObject(), _size(0) {}
    inline void init(const char* data, quint32 size) {
        FixedSizeParsedObject::init(data);
        _size = size;
    }
public:
    inline quint32 getSize() const { return _size; }
};

template <typename T>
class MSOCastArray {
private:
    const T* _data;
    quint32 _count;
public:
    explicit MSOCastArray() :_data(0), _count(0) {}
    explicit MSOCastArray(const T* data, qint32 count) :_data(data), _count(count) {}
    inline const T* getData() const { return _data; }
    QByteArray mid(int pos, int len = -1) const {
        if (pos > _count) return QByteArray();
        if (len < 0 || len > _count - pos) {
            len = _count - pos;
        }
        return QByteArray(_data + pos, len);
    }
    inline int getCount() const { return _count; }
    inline T operator[](int pos) const { return _data[pos]; }
    inline bool operator!=(const QByteArray& b) {
        return QByteArray::fromRawData(_data, _count) != b;
    }
    inline operator QByteArray() const { return QByteArray(_data, _count); }
};
template <typename T> class MSOArray;
template <typename T>
class MSOconst_iterator {
private:
    T currentItem;
    const MSOArray<T>& c;
    quint32 offset;
public:
    explicit MSOconst_iterator(const MSOArray<T>& c_, int o) :c(c_), offset(o) {
        currentItem = T(c.getData(), c.getSize());
    }
    inline bool operator!=(const MSOconst_iterator &o) const {
        return offset != o.offset;
    }
    inline void operator++() {
        offset += currentItem.getSize();
        currentItem = T(c.getData() + offset, c.getSize() - offset);
    }
    inline const T& operator*() const {
        return currentItem;
    }
};
template <typename T>
class MSONullable {
private:
    T t;
public:
    MSONullable() {}
    MSONullable(const T& data) :t(data) {}
    inline bool isPresent() const { return t.isValid(); }
    inline quint32 getSize() const { return t.getSize(); }
    inline const T& operator * () const { return t; }
};
template <typename T>
class MSOBasicNullable {
private:
    T _value;
    bool _set;
public:
    explicit MSOBasicNullable() :_value(0), _set(false) {}
    MSOBasicNullable(T value) :_value(value), _set(true) {}
    inline bool isPresent() const { return _set; }
    inline T operator * () const { return _value; }
};
template <typename T>
class MSOArray : public ParsedObject {
friend class MSOconst_iterator<T>;
private:
    quint32 _count;
public:
    typedef MSOconst_iterator<T> const_iterator;
    MSOArray() :_count(0) {}
    MSOArray(const char* d, quint32 maxsize) :_count(0) {
        quint32 msize = 0;
        quint32 mcount = 0;
        while (msize < maxsize) {
            T v(d + msize, maxsize - msize);
            if (!v.isValid()) {
                break;
            }
            msize += v.getSize();
            mcount++;
        }
        ParsedObject::init(d, msize);
        _count = mcount;
    }
    MSOArray(const char* d, quint32 maxsize, quint32 mcount) :_count(0) {
        quint32 msize = 0;
        for (quint32 i = 0; i < mcount; ++i) {
            T v(d + msize, maxsize - msize);
            if (!v.isValid()) {
                return;
            }
            msize += v.getSize();
            if (msize > maxsize) {
                return;
            }
        }
        ParsedObject::init(d, msize);
        _count = mcount;
    }
    inline quint32 getCount() const {
        return _count;
    }
    inline MSOconst_iterator<T> begin() const {
        return MSOconst_iterator<T>(*this, 0);
    }
    inline MSOconst_iterator<T> end() const {
        return MSOconst_iterator<T>(*this, getSize());
    }
    T operator[](quint32 pos) const {
        T t(ParsedObject::getData(), ParsedObject::getSize());
        quint32 i = 0;
        quint32 offset = 0;
        while (i < pos && t.isValid()) {
            offset += t.getSize();
            t = T(ParsedObject::getData() + offset,
                  ParsedObject::getSize() - offset);
            ++i;
        }
        return t;
    }
};

inline quint8 readuint8(const char* d) {
    return *d;
}
inline quint16 readuint16(const char* d) {
    return qFromLittleEndian<quint16>((const unsigned char*)d);
}
inline qint16 readint16(const char* d) {
    return qFromLittleEndian<qint16>((const unsigned char*)d);
}
inline quint32 readuint32(const char* d) {
    return qFromLittleEndian<quint32>((const unsigned char*)d);
}
inline qint32 readint32(const char* d) {
    return qFromLittleEndian<qint32>((const unsigned char*)d);
}

inline bool readbit(const char* d) {
    return *d & 0x01;
}
inline bool readbit_1(const char* d) {
    return *d >> 1 & 0x01;
}
inline bool readbit_2(const char* d) {
    return *d >> 2 & 0x01;
}
inline bool readbit_3(const char* d) {
    return *d >> 3 & 0x01;
}
inline bool readbit_4(const char* d) {
    return *d >> 4 & 0x01;
}
inline bool readbit_5(const char* d) {
    return *d >> 5 & 0x01;
}
inline bool readbit_6(const char* d) {
    return *d >> 6 & 0x01;
}
inline bool readbit_7(const char* d) {
    return *d >> 7 & 0x01;
}
inline quint8 readuint2(const char* d) {
    return *d & 0x03;
}
inline quint8 readuint2_2(const char* d) {
    return *d >> 2 & 0x03;
}
inline quint8 readuint2_4(const char* d) {
    return *d >> 4 & 0x03;
}
inline quint8 readuint2_6(const char* d) {
    return *d >> 6 & 0x03;
}
inline quint8 readuint3(const char* d) {
    return *d & 0x07;
}
inline quint8 readuint3_2(const char* d) {
    return *d >> 2 & 0x07;
}
inline quint8 readuint3_5(const char* d) {
    return *d >> 5 & 0x07;
}
inline quint8 readuint4(const char* d) {
    return *d & 0x0F;
}
inline quint8 readuint4_2(const char* d) {
    return *d >> 2 & 0x0F;
}
inline quint8 readuint4_4(const char* d) {
    return *d >> 4 & 0x0F;
}
inline quint8 readuint5(const char* d) {
    return *d & 0x1F;
}
inline quint8 readuint5_3(const char* d) {
    return *d >> 3 & 0x1F;
}
inline quint8 readuint6(const char* d) {
    return *d & 0x3F;
}
inline quint8 readuint6_2(const char* d) {
    return *d >> 2 & 0x3F;
}
inline quint8 readuint7(const char* d) {
    return *d & 0x7F;
}
inline quint8 readuint7_1(const char* d) {
    return *d >> 1 & 0x7F;
}
inline quint16 readuint9(const char* d) {
    return readuint16(d) & 0x01FF;
}
inline quint16 readuint12_4(const char* d) {
    return readuint16(d) >> 4 & 0x0FFF;
}
inline quint16 readuint13_3(const char* d) {
    return readuint16(d) >> 3 & 0x1FFF;
}
inline quint16 readuint14(const char* d) {
    return readuint16(d) & 0x3FFF;
}
inline quint16 readuint14_2(const char* d) {
    return readuint16(d) >> 2 & 0x3FFF;
}
inline quint16 readuint15_1(const char* d) {
    return readuint16(d) >> 1 & 0x7FFF;
}
inline quint32 readuint20(const char* d) {
    return readuint32(d) & 0x0FFFFF;
}
inline quint32 readuint20_4(const char* d) {
    return readuint32(d) >> 4 & 0x0FFFFF;
}
inline quint32 readuint30(const char* d) {
    return readuint32(d) & 0x3FFFFFFF;
}

#endif
