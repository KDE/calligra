#ifndef FAKE_KTOOLBAR_H
#define FAKE_KTOOLBAR_H

#include <QToolBar>
#include "kofake_export.h"
class KOFAKE_EXPORT KToolBar : public QToolBar
{
    Q_OBJECT
public:
    KToolBar(QWidget *parent = 0) : QToolBar(parent) {}
};

#endif
