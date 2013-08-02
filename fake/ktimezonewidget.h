#ifndef FAKE_KTIMEZONEWIDGET_H
#define FAKE_KTIMEZONEWIDGET_H

#include <QTreeWidget>

#include <kdatetime.h>

#include "kofake_export.h"

class QTreeWidgetItem;

class KTimeZoneWidget : public QTreeWidget
{
public:
    KTimeZoneWidget(QWidget *parent) : QTreeWidget(parent) {}
};

#endif
