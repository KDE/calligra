#ifndef FAKE_KPAGEWIDGETITEM_H
#define FAKE_KPAGEWIDGETITEM_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <kicon.h>

class KFakePageWidgetItem : public QObject
{
public:
    KFakePageWidgetItem(QWidget *widget, const QString &title = QString()) : m_widget(widget), m_title(title) {}
    QWidget* widget() const { return m_widget; }
    QString name() const { return m_title; }
    QString header() const { return m_title; }
    void setHeader(const QString &title) { m_title = title; }
    KIcon icon() const { return m_icon; }
    void setIcon(const KIcon &icon) { m_icon = icon; }
private:
    QWidget *m_widget;
    QString m_title;
    KIcon m_icon;
};

#endif
