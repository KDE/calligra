#ifndef _kpresenter_utils_h__
#define _kpresenter_utils_h__

#include <qpixmap.h>
#include <qstring.h>
#include <qregexp.h>

QString load_pixmap(const char *_file);
QString load_pixmap_native_format(const char *_file);
QPixmap string_to_pixmap(const char *_pixmap);

#endif
