#ifndef FAKE_KTEXTEDIT_H
#define FAKE_KTEXTEDIT_H

#include <QTextEdit>

class KTextEdit : public QTextEdit
{
public:
    KTextEdit(QWidget *parent = 0) : QTextEdit(parent) {}
    KTextEdit(const QString &text, QWidget *parent) : QTextEdit(text, parent) {}
};

#endif
