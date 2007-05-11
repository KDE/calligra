/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef UI4_H
#define UI4_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_HEADER

class QDomDocument;
class QDomElement;

#define QDESIGNER_UILIB_EXTERN Q_DECL_EXPORT
#define QDESIGNER_UILIB_IMPORT Q_DECL_IMPORT

#ifdef QT_DESIGNER_STATIC
#  define QDESIGNER_UILIB_EXPORT
#elif defined(QDESIGNER_UILIB_LIBRARY)
#  define QDESIGNER_UILIB_EXPORT QDESIGNER_UILIB_EXTERN
#else
#  define QDESIGNER_UILIB_EXPORT QDESIGNER_UILIB_IMPORT
#endif

#ifndef QDESIGNER_UILIB_EXPORT
#    define QDESIGNER_UILIB_EXPORT
#endif

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif


/*******************************************************************************
** Forward declarations
*/

class DomUI;
class DomIncludes;
class DomInclude;
class DomResources;
class DomResource;
class DomActionGroup;
class DomAction;
class DomActionRef;
class DomImages;
class DomImage;
class DomImageData;
class DomCustomWidgets;
class DomHeader;
class DomCustomWidget;
class DomProperties;
class DomPropertyData;
class DomSizePolicyData;
class DomLayoutDefault;
class DomLayoutFunction;
class DomTabStops;
class DomLayout;
class DomLayoutItem;
class DomRow;
class DomColumn;
class DomItem;
class DomWidget;
class DomSpacer;
class DomColor;
class DomColorGroup;
class DomPalette;
class DomFont;
class DomPoint;
class DomRect;
class DomSizePolicy;
class DomSize;
class DomDate;
class DomTime;
class DomDateTime;
class DomStringList;
class DomResourcePixmap;
class DomString;
class DomProperty;
class DomConnections;
class DomConnection;
class DomConnectionHints;
class DomConnectionHint;

/*******************************************************************************
** Declarations
*/

class QDESIGNER_UILIB_EXPORT DomUI {
public:
    DomUI();
    ~DomUI();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeVersion() { return m_has_attr_version; }
    inline QString attributeVersion() { return m_attr_version; }
    inline void setAttributeVersion(const QString& a) { m_attr_version = a; m_has_attr_version = true; }
    inline void clearAttributeVersion() { m_has_attr_version = false; }

    inline bool hasAttributeStdSetDef() { return m_has_attr_stdSetDef; }
    inline int attributeStdSetDef() { return m_attr_stdSetDef; }
    inline void setAttributeStdSetDef(int a) { m_attr_stdSetDef = a; m_has_attr_stdSetDef = true; }
    inline void clearAttributeStdSetDef() { m_has_attr_stdSetDef = false; }

    // child element accessors
    inline QString elementAuthor() { return m_author; }
    void setElementAuthor(const QString& a);

    inline QString elementComment() { return m_comment; }
    void setElementComment(const QString& a);

    inline QString elementExportMacro() { return m_exportMacro; }
    void setElementExportMacro(const QString& a);

    inline QString elementClass() { return m_class; }
    void setElementClass(const QString& a);

    inline DomWidget* elementWidget() { return m_widget; }
    void setElementWidget(DomWidget* a);

    inline DomLayoutDefault* elementLayoutDefault() { return m_layoutDefault; }
    void setElementLayoutDefault(DomLayoutDefault* a);

    inline DomLayoutFunction* elementLayoutFunction() { return m_layoutFunction; }
    void setElementLayoutFunction(DomLayoutFunction* a);

    inline QString elementPixmapFunction() { return m_pixmapFunction; }
    void setElementPixmapFunction(const QString& a);

    inline DomCustomWidgets* elementCustomWidgets() { return m_customWidgets; }
    void setElementCustomWidgets(DomCustomWidgets* a);

    inline DomTabStops* elementTabStops() { return m_tabStops; }
    void setElementTabStops(DomTabStops* a);

    inline DomImages* elementImages() { return m_images; }
    void setElementImages(DomImages* a);

    inline DomIncludes* elementIncludes() { return m_includes; }
    void setElementIncludes(DomIncludes* a);

    inline DomResources* elementResources() { return m_resources; }
    void setElementResources(DomResources* a);

    inline DomConnections* elementConnections() { return m_connections; }
    void setElementConnections(DomConnections* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_version;
    bool m_has_attr_version;

    int m_attr_stdSetDef;
    bool m_has_attr_stdSetDef;

    // child element data
    QString m_author;
    QString m_comment;
    QString m_exportMacro;
    QString m_class;
    DomWidget* m_widget;
    DomLayoutDefault* m_layoutDefault;
    DomLayoutFunction* m_layoutFunction;
    QString m_pixmapFunction;
    DomCustomWidgets* m_customWidgets;
    DomTabStops* m_tabStops;
    DomImages* m_images;
    DomIncludes* m_includes;
    DomResources* m_resources;
    DomConnections* m_connections;

    DomUI(const DomUI &other);
    void operator = (const DomUI&other);
};

class QDESIGNER_UILIB_EXPORT DomIncludes {
public:
    DomIncludes();
    ~DomIncludes();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomInclude*> elementInclude() { return m_include; }
    void setElementInclude(const QList<DomInclude*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomInclude*> m_include;

    DomIncludes(const DomIncludes &other);
    void operator = (const DomIncludes&other);
};

class QDESIGNER_UILIB_EXPORT DomInclude {
public:
    DomInclude();
    ~DomInclude();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeLocation() { return m_has_attr_location; }
    inline QString attributeLocation() { return m_attr_location; }
    inline void setAttributeLocation(const QString& a) { m_attr_location = a; m_has_attr_location = true; }
    inline void clearAttributeLocation() { m_has_attr_location = false; }

    inline bool hasAttributeImpldecl() { return m_has_attr_impldecl; }
    inline QString attributeImpldecl() { return m_attr_impldecl; }
    inline void setAttributeImpldecl(const QString& a) { m_attr_impldecl = a; m_has_attr_impldecl = true; }
    inline void clearAttributeImpldecl() { m_has_attr_impldecl = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_location;
    bool m_has_attr_location;

    QString m_attr_impldecl;
    bool m_has_attr_impldecl;

    // child element data

    DomInclude(const DomInclude &other);
    void operator = (const DomInclude&other);
};

class QDESIGNER_UILIB_EXPORT DomResources {
public:
    DomResources();
    ~DomResources();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
    inline QList<DomResource*> elementInclude() { return m_include; }
    void setElementInclude(const QList<DomResource*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    // child element data
    QList<DomResource*> m_include;

    DomResources(const DomResources &other);
    void operator = (const DomResources&other);
};

class QDESIGNER_UILIB_EXPORT DomResource {
public:
    DomResource();
    ~DomResource();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeLocation() { return m_has_attr_location; }
    inline QString attributeLocation() { return m_attr_location; }
    inline void setAttributeLocation(const QString& a) { m_attr_location = a; m_has_attr_location = true; }
    inline void clearAttributeLocation() { m_has_attr_location = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_location;
    bool m_has_attr_location;

    // child element data

    DomResource(const DomResource &other);
    void operator = (const DomResource&other);
};

class QDESIGNER_UILIB_EXPORT DomActionGroup {
public:
    DomActionGroup();
    ~DomActionGroup();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
    inline QList<DomAction*> elementAction() { return m_action; }
    void setElementAction(const QList<DomAction*>& a);

    inline QList<DomActionGroup*> elementActionGroup() { return m_actionGroup; }
    void setElementActionGroup(const QList<DomActionGroup*>& a);

    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

    inline QList<DomProperty*> elementAttribute() { return m_attribute; }
    void setElementAttribute(const QList<DomProperty*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    // child element data
    QList<DomAction*> m_action;
    QList<DomActionGroup*> m_actionGroup;
    QList<DomProperty*> m_property;
    QList<DomProperty*> m_attribute;

    DomActionGroup(const DomActionGroup &other);
    void operator = (const DomActionGroup&other);
};

class QDESIGNER_UILIB_EXPORT DomAction {
public:
    DomAction();
    ~DomAction();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    inline bool hasAttributeMenu() { return m_has_attr_menu; }
    inline QString attributeMenu() { return m_attr_menu; }
    inline void setAttributeMenu(const QString& a) { m_attr_menu = a; m_has_attr_menu = true; }
    inline void clearAttributeMenu() { m_has_attr_menu = false; }

    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

    inline QList<DomProperty*> elementAttribute() { return m_attribute; }
    void setElementAttribute(const QList<DomProperty*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    QString m_attr_menu;
    bool m_has_attr_menu;

    // child element data
    QList<DomProperty*> m_property;
    QList<DomProperty*> m_attribute;

    DomAction(const DomAction &other);
    void operator = (const DomAction&other);
};

class QDESIGNER_UILIB_EXPORT DomActionRef {
public:
    DomActionRef();
    ~DomActionRef();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    // child element data

    DomActionRef(const DomActionRef &other);
    void operator = (const DomActionRef&other);
};

class QDESIGNER_UILIB_EXPORT DomImages {
public:
    DomImages();
    ~DomImages();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomImage*> elementImage() { return m_image; }
    void setElementImage(const QList<DomImage*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomImage*> m_image;

    DomImages(const DomImages &other);
    void operator = (const DomImages&other);
};

class QDESIGNER_UILIB_EXPORT DomImage {
public:
    DomImage();
    ~DomImage();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
    inline DomImageData* elementData() { return m_data; }
    void setElementData(DomImageData* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    // child element data
    DomImageData* m_data;

    DomImage(const DomImage &other);
    void operator = (const DomImage&other);
};

class QDESIGNER_UILIB_EXPORT DomImageData {
public:
    DomImageData();
    ~DomImageData();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeFormat() { return m_has_attr_format; }
    inline QString attributeFormat() { return m_attr_format; }
    inline void setAttributeFormat(const QString& a) { m_attr_format = a; m_has_attr_format = true; }
    inline void clearAttributeFormat() { m_has_attr_format = false; }

    inline bool hasAttributeLength() { return m_has_attr_length; }
    inline int attributeLength() { return m_attr_length; }
    inline void setAttributeLength(int a) { m_attr_length = a; m_has_attr_length = true; }
    inline void clearAttributeLength() { m_has_attr_length = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_format;
    bool m_has_attr_format;

    int m_attr_length;
    bool m_has_attr_length;

    // child element data

    DomImageData(const DomImageData &other);
    void operator = (const DomImageData&other);
};

class QDESIGNER_UILIB_EXPORT DomCustomWidgets {
public:
    DomCustomWidgets();
    ~DomCustomWidgets();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomCustomWidget*> elementCustomWidget() { return m_customWidget; }
    void setElementCustomWidget(const QList<DomCustomWidget*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomCustomWidget*> m_customWidget;

    DomCustomWidgets(const DomCustomWidgets &other);
    void operator = (const DomCustomWidgets&other);
};

class QDESIGNER_UILIB_EXPORT DomHeader {
public:
    DomHeader();
    ~DomHeader();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeLocation() { return m_has_attr_location; }
    inline QString attributeLocation() { return m_attr_location; }
    inline void setAttributeLocation(const QString& a) { m_attr_location = a; m_has_attr_location = true; }
    inline void clearAttributeLocation() { m_has_attr_location = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_location;
    bool m_has_attr_location;

    // child element data

    DomHeader(const DomHeader &other);
    void operator = (const DomHeader&other);
};

class QDESIGNER_UILIB_EXPORT DomCustomWidget {
public:
    DomCustomWidget();
    ~DomCustomWidget();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QString elementClass() { return m_class; }
    void setElementClass(const QString& a);

    inline QString elementExtends() { return m_extends; }
    void setElementExtends(const QString& a);

    inline DomHeader* elementHeader() { return m_header; }
    void setElementHeader(DomHeader* a);

    inline DomSize* elementSizeHint() { return m_sizeHint; }
    void setElementSizeHint(DomSize* a);

    inline int elementContainer() { return m_container; }
    void setElementContainer(int a);

    inline DomSizePolicyData* elementSizePolicy() { return m_sizePolicy; }
    void setElementSizePolicy(DomSizePolicyData* a);

    inline QString elementPixmap() { return m_pixmap; }
    void setElementPixmap(const QString& a);

    inline DomProperties* elementProperties() { return m_properties; }
    void setElementProperties(DomProperties* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QString m_class;
    QString m_extends;
    DomHeader* m_header;
    DomSize* m_sizeHint;
    int m_container;
    DomSizePolicyData* m_sizePolicy;
    QString m_pixmap;
    DomProperties* m_properties;

    DomCustomWidget(const DomCustomWidget &other);
    void operator = (const DomCustomWidget&other);
};

class QDESIGNER_UILIB_EXPORT DomProperties {
public:
    DomProperties();
    ~DomProperties();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomPropertyData*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomPropertyData*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomPropertyData*> m_property;

    DomProperties(const DomProperties &other);
    void operator = (const DomProperties&other);
};

class QDESIGNER_UILIB_EXPORT DomPropertyData {
public:
    DomPropertyData();
    ~DomPropertyData();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeType() { return m_has_attr_type; }
    inline QString attributeType() { return m_attr_type; }
    inline void setAttributeType(const QString& a) { m_attr_type = a; m_has_attr_type = true; }
    inline void clearAttributeType() { m_has_attr_type = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_type;
    bool m_has_attr_type;

    // child element data

    DomPropertyData(const DomPropertyData &other);
    void operator = (const DomPropertyData&other);
};

class QDESIGNER_UILIB_EXPORT DomSizePolicyData {
public:
    DomSizePolicyData();
    ~DomSizePolicyData();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementHorData() { return m_horData; }
    void setElementHorData(int a);

    inline int elementVerData() { return m_verData; }
    void setElementVerData(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_horData;
    int m_verData;

    DomSizePolicyData(const DomSizePolicyData &other);
    void operator = (const DomSizePolicyData&other);
};

class QDESIGNER_UILIB_EXPORT DomLayoutDefault {
public:
    DomLayoutDefault();
    ~DomLayoutDefault();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeSpacing() { return m_has_attr_spacing; }
    inline int attributeSpacing() { return m_attr_spacing; }
    inline void setAttributeSpacing(int a) { m_attr_spacing = a; m_has_attr_spacing = true; }
    inline void clearAttributeSpacing() { m_has_attr_spacing = false; }

    inline bool hasAttributeMargin() { return m_has_attr_margin; }
    inline int attributeMargin() { return m_attr_margin; }
    inline void setAttributeMargin(int a) { m_attr_margin = a; m_has_attr_margin = true; }
    inline void clearAttributeMargin() { m_has_attr_margin = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    int m_attr_spacing;
    bool m_has_attr_spacing;

    int m_attr_margin;
    bool m_has_attr_margin;

    // child element data

    DomLayoutDefault(const DomLayoutDefault &other);
    void operator = (const DomLayoutDefault&other);
};

class QDESIGNER_UILIB_EXPORT DomLayoutFunction {
public:
    DomLayoutFunction();
    ~DomLayoutFunction();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeSpacing() { return m_has_attr_spacing; }
    inline QString attributeSpacing() { return m_attr_spacing; }
    inline void setAttributeSpacing(const QString& a) { m_attr_spacing = a; m_has_attr_spacing = true; }
    inline void clearAttributeSpacing() { m_has_attr_spacing = false; }

    inline bool hasAttributeMargin() { return m_has_attr_margin; }
    inline QString attributeMargin() { return m_attr_margin; }
    inline void setAttributeMargin(const QString& a) { m_attr_margin = a; m_has_attr_margin = true; }
    inline void clearAttributeMargin() { m_has_attr_margin = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_spacing;
    bool m_has_attr_spacing;

    QString m_attr_margin;
    bool m_has_attr_margin;

    // child element data

    DomLayoutFunction(const DomLayoutFunction &other);
    void operator = (const DomLayoutFunction&other);
};

class QDESIGNER_UILIB_EXPORT DomTabStops {
public:
    DomTabStops();
    ~DomTabStops();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QStringList elementTabStop() { return m_tabStop; }
    void setElementTabStop(const QStringList& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QStringList m_tabStop;

    DomTabStops(const DomTabStops &other);
    void operator = (const DomTabStops&other);
};

class QDESIGNER_UILIB_EXPORT DomLayout {
public:
    DomLayout();
    ~DomLayout();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeClass() { return m_has_attr_class; }
    inline QString attributeClass() { return m_attr_class; }
    inline void setAttributeClass(const QString& a) { m_attr_class = a; m_has_attr_class = true; }
    inline void clearAttributeClass() { m_has_attr_class = false; }

    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

    inline QList<DomProperty*> elementAttribute() { return m_attribute; }
    void setElementAttribute(const QList<DomProperty*>& a);

    inline QList<DomLayoutItem*> elementItem() { return m_item; }
    void setElementItem(const QList<DomLayoutItem*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_class;
    bool m_has_attr_class;

    // child element data
    QList<DomProperty*> m_property;
    QList<DomProperty*> m_attribute;
    QList<DomLayoutItem*> m_item;

    DomLayout(const DomLayout &other);
    void operator = (const DomLayout&other);
};

class QDESIGNER_UILIB_EXPORT DomLayoutItem {
public:
    DomLayoutItem();
    ~DomLayoutItem();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeRow() { return m_has_attr_row; }
    inline int attributeRow() { return m_attr_row; }
    inline void setAttributeRow(int a) { m_attr_row = a; m_has_attr_row = true; }
    inline void clearAttributeRow() { m_has_attr_row = false; }

    inline bool hasAttributeColumn() { return m_has_attr_column; }
    inline int attributeColumn() { return m_attr_column; }
    inline void setAttributeColumn(int a) { m_attr_column = a; m_has_attr_column = true; }
    inline void clearAttributeColumn() { m_has_attr_column = false; }

    inline bool hasAttributeRowSpan() { return m_has_attr_rowSpan; }
    inline int attributeRowSpan() { return m_attr_rowSpan; }
    inline void setAttributeRowSpan(int a) { m_attr_rowSpan = a; m_has_attr_rowSpan = true; }
    inline void clearAttributeRowSpan() { m_has_attr_rowSpan = false; }

    inline bool hasAttributeColSpan() { return m_has_attr_colSpan; }
    inline int attributeColSpan() { return m_attr_colSpan; }
    inline void setAttributeColSpan(int a) { m_attr_colSpan = a; m_has_attr_colSpan = true; }
    inline void clearAttributeColSpan() { m_has_attr_colSpan = false; }

    // child element accessors
    enum Kind { Unknown = 0, Widget, Layout, Spacer };
    inline Kind kind() { return m_kind; }

    inline DomWidget* elementWidget() { return m_widget; }
    void setElementWidget(DomWidget* a);

    inline DomLayout* elementLayout() { return m_layout; }
    void setElementLayout(DomLayout* a);

    inline DomSpacer* elementSpacer() { return m_spacer; }
    void setElementSpacer(DomSpacer* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    int m_attr_row;
    bool m_has_attr_row;

    int m_attr_column;
    bool m_has_attr_column;

    int m_attr_rowSpan;
    bool m_has_attr_rowSpan;

    int m_attr_colSpan;
    bool m_has_attr_colSpan;

    // child element data
    Kind m_kind;
    DomWidget* m_widget;
    DomLayout* m_layout;
    DomSpacer* m_spacer;

    DomLayoutItem(const DomLayoutItem &other);
    void operator = (const DomLayoutItem&other);
};

class QDESIGNER_UILIB_EXPORT DomRow {
public:
    DomRow();
    ~DomRow();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomProperty*> m_property;

    DomRow(const DomRow &other);
    void operator = (const DomRow&other);
};

class QDESIGNER_UILIB_EXPORT DomColumn {
public:
    DomColumn();
    ~DomColumn();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomProperty*> m_property;

    DomColumn(const DomColumn &other);
    void operator = (const DomColumn&other);
};

class QDESIGNER_UILIB_EXPORT DomItem {
public:
    DomItem();
    ~DomItem();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeRow() { return m_has_attr_row; }
    inline int attributeRow() { return m_attr_row; }
    inline void setAttributeRow(int a) { m_attr_row = a; m_has_attr_row = true; }
    inline void clearAttributeRow() { m_has_attr_row = false; }

    inline bool hasAttributeColumn() { return m_has_attr_column; }
    inline int attributeColumn() { return m_attr_column; }
    inline void setAttributeColumn(int a) { m_attr_column = a; m_has_attr_column = true; }
    inline void clearAttributeColumn() { m_has_attr_column = false; }

    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

    inline QList<DomItem*> elementItem() { return m_item; }
    void setElementItem(const QList<DomItem*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    int m_attr_row;
    bool m_has_attr_row;

    int m_attr_column;
    bool m_has_attr_column;

    // child element data
    QList<DomProperty*> m_property;
    QList<DomItem*> m_item;

    DomItem(const DomItem &other);
    void operator = (const DomItem&other);
};

class QDESIGNER_UILIB_EXPORT DomWidget {
public:
    DomWidget();
    ~DomWidget();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeClass() { return m_has_attr_class; }
    inline QString attributeClass() { return m_attr_class; }
    inline void setAttributeClass(const QString& a) { m_attr_class = a; m_has_attr_class = true; }
    inline void clearAttributeClass() { m_has_attr_class = false; }

    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
    inline QStringList elementClass() { return m_class; }
    void setElementClass(const QStringList& a);

    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

    inline QList<DomProperty*> elementAttribute() { return m_attribute; }
    void setElementAttribute(const QList<DomProperty*>& a);

    inline QList<DomRow*> elementRow() { return m_row; }
    void setElementRow(const QList<DomRow*>& a);

    inline QList<DomColumn*> elementColumn() { return m_column; }
    void setElementColumn(const QList<DomColumn*>& a);

    inline QList<DomItem*> elementItem() { return m_item; }
    void setElementItem(const QList<DomItem*>& a);

    inline QList<DomLayout*> elementLayout() { return m_layout; }
    void setElementLayout(const QList<DomLayout*>& a);

    inline QList<DomWidget*> elementWidget() { return m_widget; }
    void setElementWidget(const QList<DomWidget*>& a);

    inline QList<DomAction*> elementAction() { return m_action; }
    void setElementAction(const QList<DomAction*>& a);

    inline QList<DomActionGroup*> elementActionGroup() { return m_actionGroup; }
    void setElementActionGroup(const QList<DomActionGroup*>& a);

    inline QList<DomActionRef*> elementAddAction() { return m_addAction; }
    void setElementAddAction(const QList<DomActionRef*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_class;
    bool m_has_attr_class;

    QString m_attr_name;
    bool m_has_attr_name;

    // child element data
    QStringList m_class;
    QList<DomProperty*> m_property;
    QList<DomProperty*> m_attribute;
    QList<DomRow*> m_row;
    QList<DomColumn*> m_column;
    QList<DomItem*> m_item;
    QList<DomLayout*> m_layout;
    QList<DomWidget*> m_widget;
    QList<DomAction*> m_action;
    QList<DomActionGroup*> m_actionGroup;
    QList<DomActionRef*> m_addAction;

    DomWidget(const DomWidget &other);
    void operator = (const DomWidget&other);
};

class QDESIGNER_UILIB_EXPORT DomSpacer {
public:
    DomSpacer();
    ~DomSpacer();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    // child element accessors
    inline QList<DomProperty*> elementProperty() { return m_property; }
    void setElementProperty(const QList<DomProperty*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    // child element data
    QList<DomProperty*> m_property;

    DomSpacer(const DomSpacer &other);
    void operator = (const DomSpacer&other);
};

class QDESIGNER_UILIB_EXPORT DomColor {
public:
    DomColor();
    ~DomColor();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementRed() { return m_red; }
    void setElementRed(int a);

    inline int elementGreen() { return m_green; }
    void setElementGreen(int a);

    inline int elementBlue() { return m_blue; }
    void setElementBlue(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_red;
    int m_green;
    int m_blue;

    DomColor(const DomColor &other);
    void operator = (const DomColor&other);
};

class QDESIGNER_UILIB_EXPORT DomColorGroup {
public:
    DomColorGroup();
    ~DomColorGroup();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomColor*> elementColor() { return m_color; }
    void setElementColor(const QList<DomColor*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomColor*> m_color;

    DomColorGroup(const DomColorGroup &other);
    void operator = (const DomColorGroup&other);
};

class QDESIGNER_UILIB_EXPORT DomPalette {
public:
    DomPalette();
    ~DomPalette();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline DomColorGroup* elementActive() { return m_active; }
    void setElementActive(DomColorGroup* a);

    inline DomColorGroup* elementInactive() { return m_inactive; }
    void setElementInactive(DomColorGroup* a);

    inline DomColorGroup* elementDisabled() { return m_disabled; }
    void setElementDisabled(DomColorGroup* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    DomColorGroup* m_active;
    DomColorGroup* m_inactive;
    DomColorGroup* m_disabled;

    DomPalette(const DomPalette &other);
    void operator = (const DomPalette&other);
};

class QDESIGNER_UILIB_EXPORT DomFont {
public:
    DomFont();
    ~DomFont();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QString elementFamily() { return m_family; }
    void setElementFamily(const QString& a);

    inline int elementPointSize() { return m_pointSize; }
    void setElementPointSize(int a);

    inline int elementWeight() { return m_weight; }
    void setElementWeight(int a);

    inline bool elementItalic() { return m_italic; }
    void setElementItalic(bool a);

    inline bool elementBold() { return m_bold; }
    void setElementBold(bool a);

    inline bool elementUnderline() { return m_underline; }
    void setElementUnderline(bool a);

    inline bool elementStrikeOut() { return m_strikeOut; }
    void setElementStrikeOut(bool a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QString m_family;
    int m_pointSize;
    int m_weight;
    bool m_italic;
    bool m_bold;
    bool m_underline;
    bool m_strikeOut;

    DomFont(const DomFont &other);
    void operator = (const DomFont&other);
};

class QDESIGNER_UILIB_EXPORT DomPoint {
public:
    DomPoint();
    ~DomPoint();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementX() { return m_x; }
    void setElementX(int a);

    inline int elementY() { return m_y; }
    void setElementY(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_x;
    int m_y;

    DomPoint(const DomPoint &other);
    void operator = (const DomPoint&other);
};

class QDESIGNER_UILIB_EXPORT DomRect {
public:
    DomRect();
    ~DomRect();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementX() { return m_x; }
    void setElementX(int a);

    inline int elementY() { return m_y; }
    void setElementY(int a);

    inline int elementWidth() { return m_width; }
    void setElementWidth(int a);

    inline int elementHeight() { return m_height; }
    void setElementHeight(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_x;
    int m_y;
    int m_width;
    int m_height;

    DomRect(const DomRect &other);
    void operator = (const DomRect&other);
};

class QDESIGNER_UILIB_EXPORT DomSizePolicy {
public:
    DomSizePolicy();
    ~DomSizePolicy();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementHSizeType() { return m_hSizeType; }
    void setElementHSizeType(int a);

    inline int elementVSizeType() { return m_vSizeType; }
    void setElementVSizeType(int a);

    inline int elementHorStretch() { return m_horStretch; }
    void setElementHorStretch(int a);

    inline int elementVerStretch() { return m_verStretch; }
    void setElementVerStretch(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_hSizeType;
    int m_vSizeType;
    int m_horStretch;
    int m_verStretch;

    DomSizePolicy(const DomSizePolicy &other);
    void operator = (const DomSizePolicy&other);
};

class QDESIGNER_UILIB_EXPORT DomSize {
public:
    DomSize();
    ~DomSize();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementWidth() { return m_width; }
    void setElementWidth(int a);

    inline int elementHeight() { return m_height; }
    void setElementHeight(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_width;
    int m_height;

    DomSize(const DomSize &other);
    void operator = (const DomSize&other);
};

class QDESIGNER_UILIB_EXPORT DomDate {
public:
    DomDate();
    ~DomDate();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementYear() { return m_year; }
    void setElementYear(int a);

    inline int elementMonth() { return m_month; }
    void setElementMonth(int a);

    inline int elementDay() { return m_day; }
    void setElementDay(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_year;
    int m_month;
    int m_day;

    DomDate(const DomDate &other);
    void operator = (const DomDate&other);
};

class QDESIGNER_UILIB_EXPORT DomTime {
public:
    DomTime();
    ~DomTime();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementHour() { return m_hour; }
    void setElementHour(int a);

    inline int elementMinute() { return m_minute; }
    void setElementMinute(int a);

    inline int elementSecond() { return m_second; }
    void setElementSecond(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_hour;
    int m_minute;
    int m_second;

    DomTime(const DomTime &other);
    void operator = (const DomTime&other);
};

class QDESIGNER_UILIB_EXPORT DomDateTime {
public:
    DomDateTime();
    ~DomDateTime();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline int elementHour() { return m_hour; }
    void setElementHour(int a);

    inline int elementMinute() { return m_minute; }
    void setElementMinute(int a);

    inline int elementSecond() { return m_second; }
    void setElementSecond(int a);

    inline int elementYear() { return m_year; }
    void setElementYear(int a);

    inline int elementMonth() { return m_month; }
    void setElementMonth(int a);

    inline int elementDay() { return m_day; }
    void setElementDay(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    int m_hour;
    int m_minute;
    int m_second;
    int m_year;
    int m_month;
    int m_day;

    DomDateTime(const DomDateTime &other);
    void operator = (const DomDateTime&other);
};

class QDESIGNER_UILIB_EXPORT DomStringList {
public:
    DomStringList();
    ~DomStringList();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QStringList elementString() { return m_string; }
    void setElementString(const QStringList& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QStringList m_string;

    DomStringList(const DomStringList &other);
    void operator = (const DomStringList&other);
};

class QDESIGNER_UILIB_EXPORT DomResourcePixmap {
public:
    DomResourcePixmap();
    ~DomResourcePixmap();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeResource() { return m_has_attr_resource; }
    inline QString attributeResource() { return m_attr_resource; }
    inline void setAttributeResource(const QString& a) { m_attr_resource = a; m_has_attr_resource = true; }
    inline void clearAttributeResource() { m_has_attr_resource = false; }

    inline bool hasAttributeAlias() { return m_has_attr_alias; }
    inline QString attributeAlias() { return m_attr_alias; }
    inline void setAttributeAlias(const QString& a) { m_attr_alias = a; m_has_attr_alias = true; }
    inline void clearAttributeAlias() { m_has_attr_alias = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_resource;
    bool m_has_attr_resource;

    QString m_attr_alias;
    bool m_has_attr_alias;

    // child element data

    DomResourcePixmap(const DomResourcePixmap &other);
    void operator = (const DomResourcePixmap&other);
};

class QDESIGNER_UILIB_EXPORT DomString {
public:
    DomString();
    ~DomString();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeNotr() { return m_has_attr_notr; }
    inline QString attributeNotr() { return m_attr_notr; }
    inline void setAttributeNotr(const QString& a) { m_attr_notr = a; m_has_attr_notr = true; }
    inline void clearAttributeNotr() { m_has_attr_notr = false; }

    inline bool hasAttributeComment() { return m_has_attr_comment; }
    inline QString attributeComment() { return m_attr_comment; }
    inline void setAttributeComment(const QString& a) { m_attr_comment = a; m_has_attr_comment = true; }
    inline void clearAttributeComment() { m_has_attr_comment = false; }

    // child element accessors
private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_notr;
    bool m_has_attr_notr;

    QString m_attr_comment;
    bool m_has_attr_comment;

    // child element data

    DomString(const DomString &other);
    void operator = (const DomString&other);
};

class QDESIGNER_UILIB_EXPORT DomProperty {
public:
    DomProperty();
    ~DomProperty();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeName() { return m_has_attr_name; }
    inline QString attributeName() { return m_attr_name; }
    inline void setAttributeName(const QString& a) { m_attr_name = a; m_has_attr_name = true; }
    inline void clearAttributeName() { m_has_attr_name = false; }

    inline bool hasAttributeStdset() { return m_has_attr_stdset; }
    inline int attributeStdset() { return m_attr_stdset; }
    inline void setAttributeStdset(int a) { m_attr_stdset = a; m_has_attr_stdset = true; }
    inline void clearAttributeStdset() { m_has_attr_stdset = false; }

    // child element accessors
    enum Kind { Unknown = 0, Bool, Color, Cstring, Cursor, Enum, Font, IconSet, Pixmap, Palette, Point, Rect, Set, SizePolicy, Size, String, StringList, Number, Float, Double, Date, Time, DateTime };
    inline Kind kind() { return m_kind; }

    inline QString elementBool() { return m_bool; }
    void setElementBool(const QString& a);

    inline DomColor* elementColor() { return m_color; }
    void setElementColor(DomColor* a);

    inline QString elementCstring() { return m_cstring; }
    void setElementCstring(const QString& a);

    inline int elementCursor() { return m_cursor; }
    void setElementCursor(int a);

    inline QString elementEnum() { return m_enum; }
    void setElementEnum(const QString& a);

    inline DomFont* elementFont() { return m_font; }
    void setElementFont(DomFont* a);

    inline DomResourcePixmap* elementIconSet() { return m_iconSet; }
    void setElementIconSet(DomResourcePixmap* a);

    inline DomResourcePixmap* elementPixmap() { return m_pixmap; }
    void setElementPixmap(DomResourcePixmap* a);

    inline DomPalette* elementPalette() { return m_palette; }
    void setElementPalette(DomPalette* a);

    inline DomPoint* elementPoint() { return m_point; }
    void setElementPoint(DomPoint* a);

    inline DomRect* elementRect() { return m_rect; }
    void setElementRect(DomRect* a);

    inline QString elementSet() { return m_set; }
    void setElementSet(const QString& a);

    inline DomSizePolicy* elementSizePolicy() { return m_sizePolicy; }
    void setElementSizePolicy(DomSizePolicy* a);

    inline DomSize* elementSize() { return m_size; }
    void setElementSize(DomSize* a);

    inline DomString* elementString() { return m_string; }
    void setElementString(DomString* a);

    inline DomStringList* elementStringList() { return m_stringList; }
    void setElementStringList(DomStringList* a);

    inline int elementNumber() { return m_number; }
    void setElementNumber(int a);

    inline float elementFloat() { return m_float; }
    void setElementFloat(float a);

    inline double elementDouble() { return m_double; }
    void setElementDouble(double a);

    inline DomDate* elementDate() { return m_date; }
    void setElementDate(DomDate* a);

    inline DomTime* elementTime() { return m_time; }
    void setElementTime(DomTime* a);

    inline DomDateTime* elementDateTime() { return m_dateTime; }
    void setElementDateTime(DomDateTime* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_name;
    bool m_has_attr_name;

    int m_attr_stdset;
    bool m_has_attr_stdset;

    // child element data
    Kind m_kind;
    QString m_bool;
    DomColor* m_color;
    QString m_cstring;
    int m_cursor;
    QString m_enum;
    DomFont* m_font;
    DomResourcePixmap* m_iconSet;
    DomResourcePixmap* m_pixmap;
    DomPalette* m_palette;
    DomPoint* m_point;
    DomRect* m_rect;
    QString m_set;
    DomSizePolicy* m_sizePolicy;
    DomSize* m_size;
    DomString* m_string;
    DomStringList* m_stringList;
    int m_number;
    float m_float;
    double m_double;
    DomDate* m_date;
    DomTime* m_time;
    DomDateTime* m_dateTime;

    DomProperty(const DomProperty &other);
    void operator = (const DomProperty&other);
};

class QDESIGNER_UILIB_EXPORT DomConnections {
public:
    DomConnections();
    ~DomConnections();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomConnection*> elementConnection() { return m_connection; }
    void setElementConnection(const QList<DomConnection*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomConnection*> m_connection;

    DomConnections(const DomConnections &other);
    void operator = (const DomConnections&other);
};

class QDESIGNER_UILIB_EXPORT DomConnection {
public:
    DomConnection();
    ~DomConnection();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QString elementSender() { return m_sender; }
    void setElementSender(const QString& a);

    inline QString elementSignal() { return m_signal; }
    void setElementSignal(const QString& a);

    inline QString elementReceiver() { return m_receiver; }
    void setElementReceiver(const QString& a);

    inline QString elementSlot() { return m_slot; }
    void setElementSlot(const QString& a);

    inline DomConnectionHints* elementHints() { return m_hints; }
    void setElementHints(DomConnectionHints* a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QString m_sender;
    QString m_signal;
    QString m_receiver;
    QString m_slot;
    DomConnectionHints* m_hints;

    DomConnection(const DomConnection &other);
    void operator = (const DomConnection&other);
};

class QDESIGNER_UILIB_EXPORT DomConnectionHints {
public:
    DomConnectionHints();
    ~DomConnectionHints();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    // child element accessors
    inline QList<DomConnectionHint*> elementHint() { return m_hint; }
    void setElementHint(const QList<DomConnectionHint*>& a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    // child element data
    QList<DomConnectionHint*> m_hint;

    DomConnectionHints(const DomConnectionHints &other);
    void operator = (const DomConnectionHints&other);
};

class QDESIGNER_UILIB_EXPORT DomConnectionHint {
public:
    DomConnectionHint();
    ~DomConnectionHint();

    void read(const QDomElement &node);
    QDomElement write(QDomDocument &doc, const QString &tagName = QString());
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attribute accessors
    inline bool hasAttributeType() { return m_has_attr_type; }
    inline QString attributeType() { return m_attr_type; }
    inline void setAttributeType(const QString& a) { m_attr_type = a; m_has_attr_type = true; }
    inline void clearAttributeType() { m_has_attr_type = false; }

    // child element accessors
    inline int elementX() { return m_x; }
    void setElementX(int a);

    inline int elementY() { return m_y; }
    void setElementY(int a);

private:
    QString m_text;
    void clear(bool clear_all = true);

    // attribute data
    QString m_attr_type;
    bool m_has_attr_type;

    // child element data
    int m_x;
    int m_y;

    DomConnectionHint(const DomConnectionHint &other);
    void operator = (const DomConnectionHint&other);
};


#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_HEADER

#endif // UI4_H
