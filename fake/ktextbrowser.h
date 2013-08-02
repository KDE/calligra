#ifndef FAKE_KTEXTBROWSER_H
#define FAKE_KTEXTBROWSER_H

#include <QTextBrowser>

#include "kofake_export.h"

class KOFAKE_EXPORT KTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    KTextBrowser(QWidget *parent=0 , bool = false) : QTextBrowser(parent) {}
};

#endif
