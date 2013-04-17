#ifndef FAKE_KCOLORMIMEDATA_H
#define FAKE_KCOLORMIMEDATA_H

#include <QColor>
#include <QMimeData>
#include <QDrag>
#include <QWidget>

#include "kofake_export.h"


namespace KColorMimeData
{
    void populateMimeData(QMimeData *mimeData, const QColor& color) {}
    bool canDecode(const QMimeData *mimeData) { return false; }
    QColor fromMimeData(const QMimeData *mimeData) { return QColor(); }
    QDrag* createDrag(const QColor& color, QWidget *dragsource) { return 0; }
}

#endif
