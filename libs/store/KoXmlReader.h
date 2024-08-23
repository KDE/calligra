/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_XMLREADER_H
#define KO_XMLREADER_H

// KOXML_USE_QDOM is defined there
#include "KoXmlReaderForward.h"

#include "kostore_export.h"

#include <QDomDocument>
#include <QPair>

class QIODevice;

#ifdef KOXML_USE_QDOM

typedef QDomNode KoXmlNode;
typedef QDomElement KoXmlElement;
typedef QDomText KoXmlText;
typedef QDomCDATASection KoXmlCDATASection;
typedef QDomDocumentType KoXmlDocumentType;
typedef QDomDocument KoXmlDocument;

#else

class QString;
class QXmlStreamReader;

class KoXmlNode;
class KoXmlText;
class KoXmlCDATASection;
class KoXmlDocumentType;
class KoXmlDocument;
class KoXmlNodeData;
class KoXmlDocumentData;

/**
 * The office-text-content-prelude type.
 */
enum KoXmlNamedItemType {
    KoXmlTextContentPrelude ///< office-text-content-prelude
    // KoXmlTextContentMain, ///< office-text-content-main
    // KoXmlTextContentEpilogue ///< office-text-content-epilogue
};

/**
 * KoXmlNode represents a node in a DOM tree.
 *
 * KoXmlNode is a base class for KoXmlElement, KoXmlText.
 * Often, these subclasses are used for getting the data instead of KoXmlNode.
 * However, as base class, KoXmlNode is very helpful when for example iterating
 * all child nodes within one parent node.
 *
 * KoXmlNode implements an explicit sharing, a node shares its data with
 * other copies (if exist).
 *
 * XXX: DO NOT ADD CONVENIENCE API HERE BECAUSE THIS CLASS MUST REMAIN COMPATIBLE WITH QDOMNODE!
 *
 * @author Ariya Hidayat <ariya@kde.org>
 */
class KOSTORE_EXPORT KoXmlNode
{
public:
    enum NodeType { NullNode = 0, ElementNode, TextNode, CDATASectionNode, ProcessingInstructionNode, DocumentNode, DocumentTypeNode };

    KoXmlNode();
    KoXmlNode(const KoXmlNode &node);
    KoXmlNode &operator=(const KoXmlNode &node);
    bool operator==(const KoXmlNode &) const;
    bool operator!=(const KoXmlNode &) const;
    virtual ~KoXmlNode();

    virtual KoXmlNode::NodeType nodeType() const;
    virtual bool isNull() const;
    virtual bool isElement() const;
    virtual bool isText() const;
    virtual bool isCDATASection() const;
    virtual bool isDocument() const;
    virtual bool isDocumentType() const;

    virtual void clear();
    KoXmlElement toElement() const;
    KoXmlText toText() const;
    KoXmlCDATASection toCDATASection() const;
    KoXmlDocument toDocument() const;

    virtual QString nodeName() const;
    virtual QString namespaceURI() const;
    virtual QString prefix() const;
    virtual QString localName() const;

    KoXmlDocument ownerDocument() const;
    KoXmlNode parentNode() const;

    bool hasChildNodes() const;
    KoXmlNode firstChild() const;
    KoXmlNode lastChild() const;
    KoXmlNode nextSibling() const;
    KoXmlNode previousSibling() const;

    KoXmlElement firstChildElement() const;

    // equivalent to node.childNodes().count() if node is a QDomNode instance
    int childNodesCount() const;

    // workaround to get and iterate over all attributes
    QStringList attributeNames() const;
    QList<QPair<QString, QString>> attributeFullNames() const;

    KoXmlNode namedItem(const QString &name) const;
    KoXmlNode namedItemNS(const QString &nsURI, const QString &name) const;
    KoXmlNode namedItemNS(const QString &nsURI, const QString &name, KoXmlNamedItemType type) const;

    /**
     * Loads all child nodes (if any) of this node. Normally you do not need
     * to call this function as the child nodes will be automatically
     * loaded when necessary.
     */
    void load(int depth = 1);

    /**
     * Releases all child nodes of this node.
     */
    void unload();

    // compatibility
    /**
     * @internal do not call directly
     * Use KoXml::asQDomDocument(), KoXml::asQDomElement() or KoXml::asQDomNode() instead
     */
    void asQDomNode(QDomDocument &ownerDoc) const;

protected:
    KoXmlNodeData *d;
    explicit KoXmlNode(KoXmlNodeData *);
};

/**
 * KoXmlElement represents a tag element in a DOM tree.
 *
 * KoXmlElement holds information about an XML tag, along with its attributes.
 *
 * @author Ariya Hidayat <ariya@kde.org>
 */

class KOSTORE_EXPORT KoXmlElement : public KoXmlNode
{
public:
    KoXmlElement();
    KoXmlElement(const KoXmlElement &element);
    KoXmlElement &operator=(const KoXmlElement &element);
    ~KoXmlElement() override;
    bool operator==(const KoXmlElement &) const;
    bool operator!=(const KoXmlElement &) const;

    QString tagName() const;
    QString text() const;

    QString attribute(const QString &name) const;
    QString attribute(const QString &name, const QString &defaultValue) const;
    QString attributeNS(const QString &namespaceURI, const QString &localName, const QString &defaultValue = QString()) const;
    bool hasAttribute(const QString &name) const;
    bool hasAttributeNS(const QString &namespaceURI, const QString &localName) const;

private:
    friend class KoXmlNode;
    friend class KoXmlDocument;
    explicit KoXmlElement(KoXmlNodeData *);
};

/**
 * KoXmlText represents a text in a DOM tree.
 * @author Ariya Hidayat <ariya@kde.org>
 */
class KOSTORE_EXPORT KoXmlText : public KoXmlNode
{
public:
    KoXmlText();
    KoXmlText(const KoXmlText &text);
    KoXmlText &operator=(const KoXmlText &text);
    ~KoXmlText() override;

    QString data() const;
    bool isText() const override;

private:
    friend class KoXmlNode;
    friend class KoXmlCDATASection;
    friend class KoXmlDocument;
    explicit KoXmlText(KoXmlNodeData *);
};

/**
 * KoXmlCDATASection represents a CDATA section in a DOM tree.
 * @author Ariya Hidayat <ariya@kde.org>
 */
class KOSTORE_EXPORT KoXmlCDATASection : public KoXmlText
{
public:
    KoXmlCDATASection();
    KoXmlCDATASection(const KoXmlCDATASection &cdata);
    KoXmlCDATASection &operator=(const KoXmlCDATASection &cdata);
    ~KoXmlCDATASection() override;

    bool isCDATASection() const override;

private:
    friend class KoXmlNode;
    friend class KoXmlDocument;
    explicit KoXmlCDATASection(KoXmlNodeData *);
};

/**
 * KoXmlDocumentType represents the DTD of the document. At the moment,
 * it can used only to get the document type, i.e. no support for
 * entities etc.
 *
 * @author Ariya Hidayat <ariya@kde.org>
 */

class KOSTORE_EXPORT KoXmlDocumentType : public KoXmlNode
{
public:
    KoXmlDocumentType();
    KoXmlDocumentType(const KoXmlDocumentType &);
    KoXmlDocumentType &operator=(const KoXmlDocumentType &);
    ~KoXmlDocumentType() override;

    QString name() const;

private:
    friend class KoXmlNode;
    friend class KoXmlDocument;
    friend class KoXmlDocumentData;
    explicit KoXmlDocumentType(KoXmlNodeData *);
};

/**
 * KoXmlDocument represents an XML document, structured in a DOM tree.
 *
 * KoXmlDocument is designed to be memory efficient. Unlike QDomDocument from
 * Qt's XML module, KoXmlDocument does not store all nodes in the DOM tree.
 * Some nodes will be loaded and parsed on-demand only.
 *
 * KoXmlDocument is read-only, you can not modify its content.
 *
 * @author Ariya Hidayat <ariya@kde.org>
 */

class KOSTORE_EXPORT KoXmlDocument : public KoXmlNode
{
public:
    explicit KoXmlDocument(bool stripSpaces = false);
    KoXmlDocument(const KoXmlDocument &node);
    KoXmlDocument &operator=(const KoXmlDocument &node);
    bool operator==(const KoXmlDocument &) const;
    bool operator!=(const KoXmlDocument &) const;
    ~KoXmlDocument() override;

    KoXmlElement documentElement() const;

    KoXmlDocumentType doctype() const;

    QString nodeName() const override;
    void clear() override;

    bool setContent(QIODevice *device, bool namespaceProcessing, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    bool setContent(QIODevice *device, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    bool setContent(QXmlStreamReader *reader, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    bool setContent(const QByteArray &text, bool namespaceProcessing, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    bool setContent(const QString &text, bool namespaceProcessing, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);

    // no namespace processing
    bool setContent(const QString &text, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    /**
     * Change the way an XMLDocument will be read:
     * if stripSpaces = true then a will only have one child
     * if stripSpaces = false then a will have 3 children.
     */
    void setWhitespaceStripping(bool stripSpaces);

private:
    friend class KoXmlNode;
    explicit KoXmlDocument(KoXmlDocumentData *);
};

#endif // KOXML_USE_QDOM

/**
 * This namespace contains a few convenience functions to simplify code using QDom
 * (when loading OASIS documents, in particular).
 *
 * To find the child element with a given name, use KoXml::namedItemNS.
 *
 * To find all child elements with a given name, use
 * QDomElement e;
 * forEachElement( e, parent )
 * {
 *     if ( e.localName() == "..." && e.namespaceURI() == KoXmlNS::... )
 *     {
 *         ...
 *     }
 * }
 * Note that this means you don't ever need to use QDomNode nor toElement anymore!
 * Also note that localName is the part without the prefix, this is the whole point
 * of namespace-aware methods.
 *
 * To find the attribute with a given name, use QDomElement::attributeNS.
 *
 * Do not use getElementsByTagNameNS, it's recursive (which is never needed in Calligra).
 * Do not use tagName() or nodeName() or prefix(), since the prefix isn't fixed.
 *
 * @author David Faure <faure@kde.org>
 */
namespace KoXml
{

/**
 * A namespace-aware version of QDomNode::namedItem(),
 * which also takes care of casting to a QDomElement.
 *
 * Use this when a domelement is known to have only *one* child element
 * with a given tagname.
 *
 * Note: do *NOT* use getElementsByTagNameNS, it's recursive!
 */
KOSTORE_EXPORT KoXmlElement namedItemNS(const KoXmlNode &node, const QString &nsURI, const QString &localName);

/**
 * A namespace-aware version of QDomNode::namedItem().
 * which also takes care of casting to a QDomElement.
 *
 * Use this when you like to return the first or an invalid
 * KoXmlElement with a known type.
 *
 * This is an optimized version of the namedItemNS above to
 * give fast access to certain sections of the document using
 * the office-text-content-prelude condition as @a KoXmlNamedItemType .
 */
KOSTORE_EXPORT KoXmlElement namedItemNS(const KoXmlNode &node, const QString &nsURI, const QString &localName, KoXmlNamedItemType type);

/**
 * Explicitly load child nodes of specified node, up to given depth.
 * This function has no effect if QDom is used.
 */
KOSTORE_EXPORT void load(KoXmlNode &node, int depth = 1);

/**
 * Unload child nodes of specified node.
 * This function has no effect if QDom is used.
 */
KOSTORE_EXPORT void unload(KoXmlNode &node);

/**
 * Get the number of child nodes of specified node.
 */
KOSTORE_EXPORT int childNodesCount(const KoXmlNode &node);

/**
 * Return the name of all attributes of specified node.
 */
KOSTORE_EXPORT QStringList attributeNames(const KoXmlNode &node);

/**
 * Convert KoXmlNode classes to the corresponding QDom classes, which has
 * @p ownerDoc as the owner document (QDomDocument instance).
 * The converted @p node (and its children) are added to ownerDoc.
 *
 * NOTE:
 * - If ownerDoc is not empty, this may fail, @see QDomDocument
 * - @p node must not be a KoXmlDocument, use asQDomDocument()
 *
 * @see asQDomDocument, asQDomElement
 */
KOSTORE_EXPORT void asQDomNode(QDomDocument &ownerDoc, const KoXmlNode &node);

/**
 * Convert KoXmlNode classes to the corresponding QDom classes, which has
 * @p ownerDoc as the owner document (QDomDocument instance).
 * The converted @p element (and its children) is added to ownerDoc.
 *
 * NOTE: If ownerDoc is not empty, this may fail, @see QDomDocument
 *
 */
KOSTORE_EXPORT void asQDomElement(QDomDocument &ownerDoc, const KoXmlElement &element);

/**
 * Converts the whole @p document into a QDomDocument
 * If KOXML_USE_QDOM is defined, just returns @p document
 */
KOSTORE_EXPORT QDomDocument asQDomDocument(const KoXmlDocument &document);

/*
 * Load an XML document from specified device to a document. You can of
 * course use it with QFile (which inherits QIODevice).
 * This is much more memory efficient than standard QDomDocument::setContent
 * because the data from the device is buffered, unlike
 * QDomDocument::setContent which just loads everything in memory.
 *
 * Note: it is assumed that the XML uses UTF-8 encoding.
 */
KOSTORE_EXPORT bool
setDocument(KoXmlDocument &doc, QIODevice *device, bool namespaceProcessing, QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
}

/**
 * \def forEachElement( elem, parent )
 * \brief Loop through all child elements of \p parent.
 * This convenience macro is used to implement the forEachElement loop.
 * The \p elem parameter is a name of a QDomElement variable and the \p parent
 * is the name of the parent element. For example:
 *
 * \code
 * QDomElement e;
 * forEachElement( e, parent )
 * {
 *     kDebug() << e.localName() << " element found.";
 *     ...
 * }
 * \endcode
 */
#define forEachElement(elem, parent)                                                                                                                           \
    for (KoXmlNode _node = (parent).firstChild(); !_node.isNull(); _node = _node.nextSibling())                                                                \
        if (((elem) = _node.toElement()).isNull()) {                                                                                                           \
        } else

#endif // KO_XMLREADER_H
