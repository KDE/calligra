#ifndef KOOASISSTYLES_H
#define KOOASISSTYLES_H

#include <qdom.h>
#include <qdict.h>
#include <qvaluevector.h>

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

    /// Contains *all* styles, hashed by name
    const QDict<QDomElement>& styles() const { return m_styles; }

    /// Contains the sub-set of "user styles", i.e. those from office:styles
    const QValueVector<QDomElement> &userStyles() const { return m_userStyles; }

    /// @return the default style
    const QDomElement& defaultStyle() const { return m_defaultStyle; }

    /// @return all list styles ("text:list-style" elements), hashed by name
    const QDict<QDomElement>& listStyles() const { return m_listStyles; }

    /// @return master pages ("style:master-page" elements), hashed by name
    const QDict<QDomElement>& masterPages() const { return m_masterPages; }

protected:
    /// Add styles to styles map
    void insertStyles( const QDomElement& styles );

private:
    KoOasisStyles( const KoOasisStyles & ); // forbidden
    KoOasisStyles& operator=( const KoOasisStyles & ); // forbidden

    QDict<QDomElement>   m_styles;
    QValueVector<QDomElement> m_userStyles;
    QDomElement m_defaultStyle;

    QDict<QDomElement>   m_masterPages;
    QDict<QDomElement>   m_listStyles;

    class Private;
    Private *d;
};

#endif /* KOOASISSTYLES_H */
