#ifndef FAKE_KTEXTBROWSER_H
#define FAKE_KTEXTBROWSER_H

#include <QTextBrowser>

class KTextBrowser : public QTextBrowser
{
public:
    KTextBrowser(QWidget *parent=0 , bool = false) : QTextBrowser(parent) {}
};

#endif
