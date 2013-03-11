#ifndef FAKE_KSQUEEZEDTEXTLABEL_H
#define FAKE_KSQUEEZEDTEXTLABEL_H

#include <QLabel>

class KSqueezedTextLabel : public QLabel
{
public:
    KSqueezedTextLabel(QWidget *parent = 0) : QLabel(parent) {}
    KSqueezedTextLabel(const QString &text, QWidget *parent = 0) : QLabel(text, parent) {}
};

#endif
