// Some commonly used functions

#ifndef MISC_H
#define MISC_H

const unsigned short read16(const unsigned char *d);
const unsigned long read32(const unsigned char *d);

static const short CP2UNI[] = { 0x20ac, 0x0000, 0x201a, 0x0192,
                                0x201e, 0x2026, 0x2020, 0x2021,
                                0x02c6, 0x2030, 0x0160, 0x2039,
                                0x0152, 0x0000, 0x017d, 0x0000,
                                0x0000, 0x2018, 0x2019, 0x201c,
                                0x201d, 0x2022, 0x2013, 0x2014,
                                0x02dc, 0x2122, 0x0161, 0x203a,
                                0x0153, 0x0000, 0x017e, 0x0178 };

const short char2uni(const unsigned char &c);

#endif // MISC_H
