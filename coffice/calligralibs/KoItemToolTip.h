#ifndef CALLIGRADOCUMENTSLIBS_KOITEMTOOLTIP_H
#define CALLIGRADOCUMENTSLIBS_KOITEMTOOLTIP_H

#include <QObject>
#include <QTextDocument>
#include <QModelIndex>
#include <QPointF>
#include <QStyleOptionViewItem>

class KoItemToolTip : public QObject
{
public:
    void showTip(QWidget *view, const QPointF&, const QStyleOptionViewItem&, const QModelIndex &index = QModelIndex()) {}
    void hide() {}
protected:
    virtual QTextDocument *createDocument(const QModelIndex &index) = 0;
};

#endif
 
 
