#ifndef KWOASISLOADER_H
#define KWOASISLOADER_H

class KWLoadingInfo;
class KoOasisContext;
class QDomDocument;
class KoOasisSettings;
class QDomElement;
class KWFrame;
class KoStore;
class KoTextCursor;
class KWDocument;
template <class T> class QValueList;

// KWDocument delegates to this class the loading of OASIS documents or fragments
class KWOasisLoader
{
public:
    KWOasisLoader( KWDocument* doc );

    /// Insert everything from an oasis store - used when pasting
    /// (either text or entire frames)
    /// TODO: dontCreateFootNote
    QValueList<KWFrame *> insertOasisData( KoStore* store, KoTextCursor* cursor );

    void loadOasisSettings( const QDomDocument&settingsDoc );
    void loadOasisHeaderFooter( const QDomElement& headerFooter, bool hasEvenOdd,
                                QDomElement& style, KoOasisContext& context );

    KWFrame* loadOasisTextBox( const QDomElement& frame, const QDomElement& tag,
                               KoOasisContext& context );

private:
    void loadOasisIgnoreList( const KoOasisSettings& settings );

private:
    KWDocument* m_doc;
};


#endif
