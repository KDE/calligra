#ifndef KOOASISSTYLES_H
#define KOOASISSTYLES_H

#include <qdom.h>
#include <qdict.h>
#include <qvaluevector.h>
#include <qmap.h>

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
    QValueVector<QDomElement> userStyles() const;

    /// @return the default style
    const QDomElement& defaultStyle() const { return m_defaultStyle; }

    /// @return the office:style element
    const QDomElement& officeStyle() const { return m_officeStyle; }

    /// @return all list styles ("text:list-style" elements), hashed by name
    const QDict<QDomElement>& listStyles() const { return m_listStyles; }

    /// @return master pages ("style:master-page" elements), hashed by name
    const QDict<QDomElement>& masterPages() const { return m_masterPages; }

    typedef QMap<QString, QString> DataFormatsMap;
    /// Date and time formats found while parsing styles. Used e.g. for fields.
    /// Key: format name. Value:
    const DataFormatsMap& dateTimeFormats() const { return m_dateTimeFormats; }

protected:
    /// Add styles to styles map
    void insertStyles( const QDomElement& styles );

private:
    void importDateTimeStyle( const QDomElement& parent );

    KoOasisStyles( const KoOasisStyles & ); // forbidden
    KoOasisStyles& operator=( const KoOasisStyles & ); // forbidden

private:
    QDict<QDomElement>   m_styles;
    QDomElement m_defaultStyle;
    QDomElement m_officeStyle;

    QDict<QDomElement>   m_masterPages;
    QDict<QDomElement>   m_listStyles;

    DataFormatsMap m_dateTimeFormats; // maybe generalize to include number formats.

    class Private;
    Private *d;
};

#endif /* KOOASISSTYLES_H */
