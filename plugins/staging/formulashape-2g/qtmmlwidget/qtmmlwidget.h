/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#ifndef QTMMLWIDGET_H
#define QTMMLWIDGET_H

#include <QtGui/QFrame>
#include <QtXml/QtXml>

class MmlDocument;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTMMLWIDGET_EXPORT) && !defined(QT_QTMMLWIDGET_IMPORT)
#    define QT_QTMMLWIDGET_EXPORT
#  elif defined(QT_QTMMLWIDGET_IMPORT)
#    if defined(QT_QTMMLWIDGET_EXPORT)
#      undef QT_QTMMLWIDGET_EXPORT
#    endif
#    define QT_QTMMLWIDGET_EXPORT __declspec(dllimport)
#  elif defined(QT_QTMMLWIDGET_EXPORT)
#    undef QT_QTMMLWIDGET_EXPORT
#    define QT_QTMMLWIDGET_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTMMLWIDGET_EXPORT
#endif

static bool           g_draw_frames			= false;
static const double   g_mfrac_spacing			= 0.1;
static const double   g_mroot_base_margin		= 0.1;
static const double   g_script_size_multiplier	    	= 0.7071; // sqrt(1/2)
static const int      g_min_font_point_size		= 8;
static const QChar    g_radical_char  	    	    	= QChar(0x1A, 0x22);
static const unsigned g_oper_spec_rows     	        = 9;


class MmlNode;

class QT_QTMMLWIDGET_EXPORT QtMmlWidget : public QFrame
{
    public:
	enum MmlFont { NormalFont, FrakturFont, SansSerifFont, ScriptFont,
				    MonospaceFont, DoublestruckFont };

	QtMmlWidget(QWidget *parent = 0);
	~QtMmlWidget();

	QString fontName(MmlFont type) const;
	void setFontName(MmlFont type, const QString &name);
	int baseFontPointSize() const;
	void setBaseFontPointSize(int size);

	bool setContent(const QString &text, QString *errorMsg = 0,
			    int *errorLine = 0, int *errorColumn = 0);
    QString content();
    
	void dump() const;
	virtual QSize sizeHint() const;

	void setDrawFrames(bool b);
	bool drawFrames() const;

	void clear();

    protected:
	virtual void paintEvent(QPaintEvent *e);

    private:
	MmlDocument *m_doc;
};


class QT_QTMMLWIDGET_EXPORT QtMmlDocument
{
public:
    QtMmlDocument();
    ~QtMmlDocument();
    void clear();

    bool setContent(QString text, QString *errorMsg = 0,
                    int *errorLine = 0, int *errorColumn = 0);
    QString content();
    
    void paint(QPainter *p, const QPoint &pos) const;
    void layout();
    
    QSize size() const;

    QString fontName(QtMmlWidget::MmlFont type) const;
    void setFontName(QtMmlWidget::MmlFont type, const QString &name);

    int baseFontPointSize() const;
    void setBaseFontPointSize(int size);
    
    MmlNode *rootNode();
    void deleteNode(MmlNode *node);
private:
    MmlDocument *m_doc;
};

struct Mml
{
    enum NodeType {
	    NoNode = 0, MiNode, MnNode, MfracNode, MrowNode, MsqrtNode,
	    MrootNode, MsupNode, MsubNode, MsubsupNode, MoNode,
	    MstyleNode, TextNode, MphantomNode, MfencedNode,
	    MtableNode, MtrNode, MtdNode, MoverNode, MunderNode,
	    MunderoverNode, MerrorNode, MtextNode, MpaddedNode,
        MspaceNode, MalignMarkNode, MannotationNode, UnknownNode
    };

    enum MathVariant {
	    NormalMV	    = 0x0000,
	    BoldMV  	    = 0x0001,
	    ItalicMV	    = 0x0002,
	    DoubleStruckMV  = 0x0004,
	    ScriptMV        = 0x0008,
	    FrakturMV       = 0x0010,
	    SansSerifMV     = 0x0020,
	    MonospaceMV     = 0x0040
    };

    enum FormType { PrefixForm, InfixForm, PostfixForm };
    enum ColAlign { ColAlignLeft, ColAlignCenter, ColAlignRight };
    enum RowAlign { RowAlignTop, RowAlignCenter, RowAlignBottom,
		    RowAlignAxis, RowAlignBaseline };
    enum FrameType { FrameNone, FrameSolid, FrameDashed };

    struct FrameSpacing {
	FrameSpacing(int hor = 0, int ver = 0)
	    : m_hor(hor), m_ver(ver) {}
	int m_hor, m_ver;
    };
};

struct OperSpec {
    enum StretchDir { NoStretch, HStretch, VStretch, HVStretch };

    const char *name;
    Mml::FormType form;
    const char *attributes[g_oper_spec_rows];
    StretchDir stretch_dir;
};

struct NodeSpec
{
    Mml::NodeType type;
    const char *tag;
    const char *type_str;
    int child_spec;
    const char *child_types;
    const char *attributes;

    enum ChildSpec {
	    ChildAny     = -1, // any number of children allowed
	    ChildIgnore  = -2, // do not build subexpression of children
	    ImplicitMrow = -3  // if more than one child, build mrow
    };
};

struct EntitySpec
{
    const char *name;
    const char *value;
};

typedef QMap<QString, QString> MmlAttributeMap;
class MmlNode;

class MmlDocument : public Mml
{
    public:
	MmlDocument();
	~MmlDocument();
	void clear();

	bool setContent(QString text, QString *errorMsg = 0,
			    int *errorLine = 0, int *errorColumn = 0);
    QString content();
    QString mmlContent(MmlNode *node);
    
	void paint(QPainter *p, const QPoint &pos) const;
	void dump() const;
	QSize size() const;
	void layout();

	QString fontName(QtMmlWidget::MmlFont type) const;
	void setFontName(QtMmlWidget::MmlFont type, const QString &name);

	int baseFontPointSize() const
	    { return m_base_font_point_size; }
	void setBaseFontPointSize(int size)
	    { m_base_font_point_size = size; }
	QColor foregroundColor() const
	    { return m_foreground_color; }
	void setForegroundColor(const QColor &color)
	    { m_foreground_color = color; }
	QColor backgroundColor() const
	    { return m_background_color; }
	void setBackgroundColor(const QColor &color)
	    { m_background_color = color; }
	MmlNode *rootNode() 
	    { return m_root_node; }
	
	void deleteNode(MmlNode *node);

    private:
	void _dump(const MmlNode *node, QString &indent) const;
	bool insertChild(MmlNode *parent, MmlNode *new_node, QString *errorMsg);

	MmlNode *domToMml(const QDomNode &dom_node, bool *ok, QString *errorMsg);
	MmlNode *createNode(NodeType type, const MmlAttributeMap &mml_attr,
				const QString &mml_value, QString *errorMsg);
	MmlNode *createImplicitMrowNode(const QDomNode &dom_node, bool *ok,
				    QString *errorMsg);

	void insertOperator(MmlNode *node, const QString &text);

	MmlNode *m_root_node;

	QString m_normal_font_name;
	QString m_fraktur_font_name;
	QString m_sans_serif_font_name;
	QString m_script_font_name;
	QString m_monospace_font_name;
	QString m_doublestruck_font_name;
	int m_base_font_point_size;
	QColor m_foreground_color;
	QColor m_background_color;
};

class FormulaCursor;

class MmlNode : public Mml
{
    friend class MmlDocument;
    friend class FormulaCursor;

    public:
	MmlNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map);
	virtual ~MmlNode();

	// Mml stuff
	NodeType nodeType() const
	    { return m_node_type; }

	virtual QString toStr() const;

	void setRelOrigin(const QPoint &rel_origin);
	QPoint relOrigin() const
	    { return m_rel_origin; }
	void stretchTo(const QRect &rect);
	bool isStretched() const
	    { return m_stretched; }
	QPoint devicePoint(const QPoint &p) const;

	QRect myRect() const
	    { return m_my_rect; }
	QRect parentRect() const;
	virtual QRect deviceRect() const;
	void updateMyRect();
	virtual void setMyRect(const QRect &rect)
	    { m_my_rect = rect; }

	virtual void stretch();
	virtual void layout();
	virtual void paint(QPainter *p);

	int basePos() const;
	int overlinePos() const;
	int underlinePos() const;
	int em() const;
	int ex() const;

	QString explicitAttribute(const QString &name, const QString &def = QString::null) const;
	QString inheritAttributeFromMrow(const QString &name, const QString &def = QString::null) const;

	virtual QFont font() const;
	virtual QColor color() const;
	virtual QColor background() const;
	virtual int scriptlevel(const MmlNode *child = 0) const;


	// Node stuff
	MmlDocument *document() const
	    { return m_document; }
	MmlNode *parent() const
	    { return m_parent; }
	MmlNode *firstChild() const
	    { return m_first_child; }
	MmlNode *nextSibling() const
	    { return m_next_sibling; }
	MmlNode *previousSibling() const
	    { return m_previous_sibling; }
	MmlNode *lastSibling() const;
	MmlNode *firstSibling() const;
	bool isLastSibling() const
	    { return m_next_sibling == 0; }
	bool isFirstSibling() const
	    { return m_previous_sibling == 0; }
	bool hasChildNodes() const
	    { return m_first_child != 0; }
	    
protected:
	virtual void layoutSymbol();
	virtual void paintSymbol(QPainter *p) const;
	virtual QRect symbolRect() const
	    { return QRect(0, 0, 0, 0); }

	MmlNode *parentWithExplicitAttribute(const QString &name, NodeType type = NoNode);
	int interpretSpacing(const QString &value, bool *ok) const;

    private:
	MmlAttributeMap m_attribute_map;
	bool m_stretched;
	QRect m_my_rect, m_parent_rect;
	QPoint m_rel_origin;

	NodeType m_node_type;
	MmlDocument *m_document;

	MmlNode *m_parent,
		*m_first_child,
		*m_next_sibling,
		*m_previous_sibling;
		
	bool m_showCursor;
};

#endif
