#ifndef LEINPUT_H
#define LEINPUT_H

#include <QtCore/QtEndian>

template <typename T>
class MSOCastArray {
private:
    const T* _data;
    quint32 _count;
public:
    MSOCastArray() :_data(0), _count(0) {}
    MSOCastArray(const T* data, qint32 count) :_data(data), _count(count) {}
    const T* data() const;
    QByteArray mid(int pos, int len = -1) const;
    int size() const;
    T operator[](int pos) const { return _data[pos]; }
    bool operator!=(const QByteArray&);
    operator QByteArray() const;
};

template <typename T> class MSOArray;
template <typename T>
class MSOconst_iterator {
private:
    T currentItem;
    const MSOArray<T>& c;
    quint32 offset;
public:
    MSOconst_iterator(const MSOArray<T>& c_, int o) :c(c_), offset(o) {
        currentItem = T(c._data, c._size);
    }
    inline bool operator!=(const MSOconst_iterator &o) const {
        return offset != o.offset;
    }
    inline void operator++() {
        offset += currentItem._size;
        currentItem = T(c._data + offset, c._size - offset);
    }
    inline const T& operator*() const {
        return currentItem;
    }
};
template <typename T>
class MSOArray {
public:
    typedef MSOconst_iterator<T> const_iterator;
    const char* _data;
    quint32 _count;
    quint32 _size;
    MSOArray() :_data(0), _count(0), _size(0) {}
    MSOArray(const char* d, quint32 maxsize) :_data(0), _count(0), _size(0) {
        quint32 msize = 0;
        quint32 mcount = 0;
        while (msize < maxsize) {
            T v(d + msize, maxsize - msize);
            if (v._data == 0) {
                break;
            }
            msize += v._size;
            mcount++;
        }
        _data = d;
        _count = mcount;
        _size = msize;
    }
    MSOArray(const char* d, quint32 maxsize, quint32 mcount) :_data(0), _count(0), _size(0) {
        quint32 msize = 0;
        for (quint32 i = 0; i < mcount; ++i) {
            T v(d + msize, maxsize - msize);
            if (v._data == 0) {
                return;
            }
            msize += v._size;
            if (msize > maxsize) {
                return;
            }
        }
        _data = d;
        _count = mcount;
        _size = msize;
    }
    inline int size() const {
        return _count;
    }
    inline MSOconst_iterator<T> begin() const {
        return MSOconst_iterator<T>(*this, 0);
    }
    inline MSOconst_iterator<T> end() const {
        return MSOconst_iterator<T>(*this, _size);
    }
    T operator[](quint32 pos) const {
        T t(_data, _size);
        quint32 i = 0;
        quint32 offset = 0;
        while (i < pos) {
            offset += t._size;
            t = T(_data + offset, _size - offset);
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
