#ifndef KOOASISSTYLES_H
#define KOOASISSTYLES_H

#include <qdom.h>
#include <qdict.h>

/**
 * Repository of styles used during loading of OASIS/OOo file
 */
class KoOasisStyles
{
public:
    KoOasisStyles();
    ~KoOasisStyles();

    /// Look into @p doc for styles and remember them
    void createStyleMap( const QDomDocument& doc );

    const QDict<QDomElement>& styles() const { return m_styles; }
    const QDomElement& defaultStyle() const { return m_defaultStyle; }
    const QDict<QDomElement>& listStyles() const { return m_listStyles; }
    const QDict<QDomElement>& masterPages() const { return m_masterPages; }

protected:
    /// Add styles to styles map
    void insertStyles( const QDomElement& styles );

private:
    KoOasisStyles( const KoOasisStyles & ); // forbidden
    KoOasisStyles& operator=( const KoOasisStyles & ); // forbidden

    QDict<QDomElement>   m_styles;
    QDomElement m_defaultStyle;

    QDict<QDomElement>   m_masterPages;
    QDict<QDomElement>   m_listStyles;

    class Private;
    Private *d;
};

#endif /* KOOASISSTYLES_H */
