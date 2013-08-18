#ifndef FAKE_KLISTWIDGETSEARCHLINE_H
#define FAKE_KLISTWIDGETSEARCHLINE_H

#include <klineedit.h>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>

#include <kofake_export.h>

class KOFAKE_EXPORT KListWidgetSearchLine : public KLineEdit
{
    Q_OBJECT
public:
    KListWidgetSearchLine( QWidget *parent = 0, QListWidget *listWidget = 0 ) : KLineEdit(parent), m_listWidget(listWidget) {}
    Qt::CaseSensitivity caseSensitive() const { return Qt::CaseInsensitive; }
    QListWidget *listWidget() const { return m_listWidget; }

public Q_SLOTS:
    virtual void updateSearch( const QString &s = QString() ) {}
    void setCaseSensitivity( Qt::CaseSensitivity cs ) {}
    void setListWidget( QListWidget *lv ) { m_listWidget = lv; }
    void clear() {}

protected:
    virtual bool itemMatches( const QListWidgetItem *item, const QString &s ) const { return false; }

private:
    QListWidget *m_listWidget;
};

#endif
