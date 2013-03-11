#ifndef CALLIGRALIBS_KoColorConversions_H
#define CALLIGRALIBS_KoColorConversions_H

#include <KoID.h>

const KoID AlphaColorModelID("A", ki18n("Alpha"));
const KoID RGBAColorModelID("RGBA", ki18n("Red Green Blue"));
const KoID XYZAColorModelID("XYZA", ki18n("XYZ"));
const KoID LABAColorModelID("LABA", ki18n("L a* b*"));
const KoID CMYKAColorModelID("CMYKA", ki18n("Cyan Magenta Yellow Black"));
const KoID GrayAColorModelID("GRAYA", ki18n("Grayscale"));
const KoID GrayColorModelID("GRAY", ki18n("Grayscale (without transparency)"));
const KoID YCbCrAColorModelID("YCbCrA", ki18n("YCbCr"));

const KoID Integer8BitsColorDepthID("U8", ki18n("8 Bits"));
const KoID Integer16BitsColorDepthID("U16", ki18n("16 Bits"));
const KoID Float16BitsColorDepthID("F16", ki18n("16 Bits Float"));
const KoID Float32BitsColorDepthID("F32", ki18n("32 Bits Float"));
const KoID Float64BitsColorDepthID("F64", ki18n("64 Bits Float"));

#endif
