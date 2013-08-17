#ifndef FAKE_KGUIITEM_H
#define FAKE_KGUIITEM_H

#include <QPair>
#include <QPushButton>
#include <QDebug>

#include <kconfiggroup.h>
#include <kicon.h>
#include <klocale.h>

class KGuiItem
{
public:
    KGuiItem( const QString &text, const QString &iconName = QString(), const QString &toolTip = QString(), const QString &whatsThis = QString() ) : m_text(text) {}
    KGuiItem( const QString &text, const KIcon &icon, const QString &toolTip = QString(), const QString &whatsThis = QString() ) : m_text(text) {}

    QString text() const { return m_text; }
    void setText(const QString &text) { m_text = text; }

    QString plainText() const { return m_text; }

    KIcon icon( ) const { return m_icon; }
    void setIcon( const KIcon &icon ) { m_icon = icon; }
    bool hasIcon() const  { return !m_icon.isNull(); }

    QString iconName() const { return QString(); }
    void setIconName( const QString &iconName ) {}

    QString toolTip() const { return m_tooltip; }
    void setToolTip( const QString &tooltip ) { m_tooltip = tooltip; }

    QString whatsThis() const { return m_whatsThis; }
    void setWhatsThis(const QString &text) { m_whatsThis = text; }

    bool isEnabled() const { return m_enable; }
    void setEnabled( bool enable ) { m_enable = enable; }

private:
    QString m_text, m_tooltip, m_whatsThis;
    KIcon m_icon;
    bool m_enable;
};

#endif
