/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTSTATICDATA_H
#define MCTSTATICDATA_H

#include <QDomElement>
#include <QMap>
#include <QMutex>
#include <QString>
#include <QList>
#include <QTextCursor>
#include <QtGlobal>

class MctChange;
class MctPosition;
class MctEmbObjProperties;
class MctUndoGraph;
class MctRedoGraph;

class KZip;
class KArchiveDirectory;

class KoTextDocument;
class KoDocument;

// TODO
class FIXME_EmbElement;
class FIXME_URL;
class FIXME_TextInterface;
class FIXME_Doc;
class FIXME_PropertyDictionary;

enum MctChangeTypes : unsigned int;

using XMLns = QString;

/**
 * Contains all static const global variable and function
 */
class MctStaticData
{
public:    

    /// String constants
    static const QString EXPORTED_BY_SYSTEM;
    static const ulong UNDEFINED;

    static const QString UNDOTAG;
    static const QString REDOTAG;
    static const QString UNDOCHANGES;
    static const QString REDOCHANGES;
    // names of the files containing undo/redo operations
    static const QString UNDOFILE;
    static const QString REDOFILE;
    //static variables to parse position data from XML
    static const QString POSSEPARATOR;
    static const QString POSSTART;
    static const QString POSEND;
    static const QString MPOSSTART;
    static const QString MPOSEND;
    static const QString TABLE;
    static const QString CELL;
    static const QString MOVEDTABLE;
    static const QString MOVEDCELL;

    static const QString DATESEP;
    static const QString TIMESEP;
    static const QString DATETIMESEP;

    static const QString GRAPHICOBJ;
    static const QString GRAPHICOBJ_PATH;

    static const QChar REDOCHAR;

    /// XML namespaces
    static const XMLns XMLNS;
    static const XMLns NS_DC;
    static const XMLns NS_C;
    static const XMLns NS_OFFICE;

    /// XML string constants (tags, attribute values, etc)
    static const QString VERSION;
    static const QString STARTINGSTRING;
    static const QString AUTHOR;
    static const QString COMMENT;
    static const QString DATE;
    static const QString TYPE;
    static const QString ID;
    static const QString NAME;
    static const QString ROW;
    static const QString COL;
    static const QString URL;
    static const QString POSX;
    static const QString POSY;
    static const QString PREVPOSX;
    static const QString PREVPOSY;
    static const QString HEIGHT;
    static const QString WIDTH;
    static const QString ROTATION;
    static const QString PARENT;

    static const QString ADDED;     // undo uses "del" here (!)
    static const QString REMOVED;   // undo uses "add" here (!)
    static const QString MOVED;
    static const QString STRING;
    static const QString PARAGRAPH;
    static const QString STYLE;
    static const QString TEXTFRAME;
    static const QString TEXTGRAPHICOBJECT;
    static const QString EMBEDDEDOBJECT;
    static const QString ROWCHANGE;
    static const QString COLCHANGE;
    //static const QString TABLE;
    static const QString FORMATTAG;
    static const QString PROPERTIESTAG;
    static const QString TEXTTAG;
    static const QString PARAGRAPHTAG;
    static const QString LIST;
    static const QString UNDEFINEDTAG;

    static int currentRevision;    

    /// property string and values
    static const QSet<QString> TEXTGRAPHICOBJECTS_INTPROPS;
    static const QSet<QString> TEXTGRAPHICOBJECTS_STRUCTPROPS;
    static const QSet<QString> TEXTGRAPHICOBJECTS_FLOATPROPS;
    static const QSet<QString> TEXTGRAPHICOBJECTS_STRINGPROPS;
    static const QSet<QString> TEXTGRAPHICOBJECTS_BOOLEANPROPS;

    static const QSet<QString> TEXTTABLE_STRUCTPROPS;
    static const QSet<QString> TEXTTABLE_ENUMPROPS;

    static const QSet<QString> TEXT_PROPS_BOOLEAN;
    static const QSet<QString> TEXT_PROPS_INT;
    static const QSet<QString> TEXT_PROPS_DOUBLE;
    static const QSet<QString> TEXT_PROPS_STRING;

    static const QMap<int, QString> TEXT_PROPS_AS_STRING;
    static const QMap<int, QString> initTEXT_PROPS_AS_STRING();
    static const QMap<int, QString> TEXT_KO_CHAR_PROPS_AS_STRING;
    static const QMap<int, QString> initTEXT_KO_CHAR_PROPS_AS_STRING();
    static const QMap<int, QString> TEXT_KO_PAR_PROPS_AS_STRING;
    static const QMap<int, QString> initTEXT_KO_PAR_PROPS_AS_STRING();
    static const QMap<int, QString> TEXT_KO_LIST_PROPS_AS_STRING;
    static const QMap<int, QString> initTEXT_KO_LIST_PROPS_AS_STRING();

    /**
     * convert text property to integer
     *
     * @param propertyname name of the property (eg. from XML)
     * @return corresponding interger identifier
     */
    static int getTextPropAsInt(QString propertyname);

    /**
     * convert property type and id key into property name
     *
     * @param key property identifier
     * @param type property type id (text, list, etc) @todo enum class
     * @return property name as string
     */
    static QString getTextPropetyString(int key, int type);

    static const QMap<MctChangeTypes, QString> CHANGE_AS_STRING;    ///< \ref ChangeEntities classes as string
    static const QMap<MctChangeTypes, QString> initCHANGE_AS_STRING();

    /**
     * Text formatting property types
     *
     * @todo should make to enum class and use it correctly @ref MctChangeEntities
     */
    enum PropertyType{
        CharacterProperty,
        ParagraphProperty,
        ListProperty,
        UnknownProperty
    };

    static MctStaticData* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new MctStaticData();

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        if(m_Instance) {
            delete m_Instance;
            m_Instance = 0;
        }
        mutex.unlock();
    }    

    template <typename T> void reverseList(QList<T> **list);
    template <typename T> void reverseList2(QList<T> *list);

    // functions.py
    /**
     * create the temporary file path of the MCT tracket document
     *
     * @param url original URL of the document
     * @return the temporary file path
     */
    QString tempMctFile(QString url);

    /**
     * zips embedded files into the Odt when graph is exported
     *
     * @param delemet dictionary of embedded objects stored in staticvars
     */
    void includeEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);

    /**
     * backsup embedded files from the Odt when graph is exported
     * @param delemet dictionary of embedded objects stored in staticvars
     */
    void backupEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);

    /**
     * deletes embedded files corresponding to container delement
     * @param delemet dictionary of embedded objects stored in staticvars
     */
    void deleteEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);

    /// export undo and redo graphs
    void exportGraphs();

    // function2.py
    /**
     * get data from position tag and arrange it into a list
     * @param postag position tag
     * @return the list of data extracted from position tag
     */
    QList<QString> getDataFromPosTag(const QString &postag);

    /**
     * create position class from string extracted from the XML node
     * @param change XML node.
     * @param moved true if moved position is wanted, False otherwise
     * @return created position class
     */
    MctPosition * getPosFromElementreeNode(const QDomElement &change, bool moved=false);

    /**
     * create position class from string extracted from the XML node. --- new
     *
     * The index start from 1 in the xml, we need to fix that.
     * @param change the XML node.
     * @param moved true if moved position is wanted, False otherwise
     * @return created position class.
     */
    MctPosition * getPosFromElementreeNodeNew(const QDomElement &change, bool moved=false);

    /**
     * create position class from string extracted from the XML node. --- Obsolete
     *
     * @param change The XML node.
     * @param moved True if moved position is wanted, False otherwise
     * @return Returns with the created position class.
     *
     * @todo if obsolete why not removed?
     */
    MctPosition * getPosFromElementreeNodeOld(const QDomElement &change, bool moved=false);

    /**
     * recursively add position attribute to the node of the graph.
     *
     * The index start from 0 in Calligra, and start from 1 in the xml.
     * @param change change node in the XML
     * @param pos position
     * @param moved if true, moved position is affected.
     * @param clearfirst if true, position attributes in the XML node are cleared
     */
    void addPos2change(QDomElement *change, MctPosition *pos, bool moved=false, bool clearfirst=true);

    /**
     * decide, whether the chage node represents table change or not
     *
     * @param changeNode
     * @return Returns with True if the changenode is related to table change, False otherwise
     */
    bool isTableChange(const MctChange *changeNode);

    FIXME_TextInterface * getTextInterface(FIXME_TextInterface *parent, MctPosition *position);
    QTextCursor * CreateCursorFromRecursivePos(KoTextDocument *doc, MctPosition *position);
    QTextCursor * CreateCursorFromPos( FIXME_TextInterface *xtext_interface, MctPosition *position);

    // function3.py
    /**
     * gather indexes of newline characters in the parameter string
     *
     * @param string
     * @return list of indexes
     * @todo unused method
     */
    QList<int> *findNewLines(QString string);

    /**
     * creates change node for redo operation
     *
     * @param changeNode The undo changeNode.
     * @param doc
     * @return
     */
    MctChange * createRedoChangeNode(KoTextDocument *doc, MctChange *changeNode);

    /// @todo unused
    void insertTextGraphicObjects(MctChange *changeNode, FIXME_Doc * doc/*,staticvars */,bool withprops=true, FIXME_TextInterface *xtext_interface=NULL);
    /// @todo unused
    void instertTextTable(MctChange *changeNode/*,staticvars */, bool withprops=true, FIXME_TextInterface *xtext_interface=NULL);
    /// @todo unused
    FIXME_PropertyDictionary * getNoneDefaultProps(MctEmbObjProperties *props);

    // own functions
    /**
     * recursive copy of the given directory
     *
     * @param dir directory to copy
     * @param path destination
     * @param destinationZip output which contains the copied content
     */
    static void recursiveDirectoryCopy(const KArchiveDirectory *dir, const QString &path, KZip *destinationZip);

    /**
     * check whether the property type is supported or not
     *
     * @param key property type id @todo use enum class instead
     */
    static void textPropMapTester(int key);

    /**
     * check wether the given odt file has undo/redo.xml in the package
     * @param odt file to check
     * @return true if the odt has mct support, false otherwise
     * @todo unused function
     */
    static bool hasMctSupport(QString odt);

    /// getter
    MctUndoGraph* getUndoGraph();
    /// setter
    void setUndoGraph(MctUndoGraph* graph);
    /// getter
    MctRedoGraph* getRedoGraph();
    /// setter
    void setRedoGraph(MctRedoGraph* graph);
    /// getter
    QString getFileURL();
    /// setter
    void setFileURL(const QString &url);
    /// getter
    QList<MctChange*>* getChanges();
    /// setter
    void setChanges(QList<MctChange*> *change);
    /// clear the list of changes
    void clearChanges();
    /// getter
    KoDocument* getKoDocument();
    /// setter
    void setKoDocument(KoDocument* doc);

    /**
     * create a map to the frames of the document
     *
     * The created map contains from which block how much position correction is needed
     * @param koTextDoc document in action
     * @return map of frame indexes
     */
    QMap<ulong, ulong> * getFrameIndexes(const KoTextDocument *koTextDoc);    

    /// getter
    bool getMctState() const;
    /// setter
    void setMctState(bool state);
    /// getter @todo unused
    QString getAddedShapeType();
    void setAddedShapeType(const QString &shapeType);

    /**
     * extends XML attribute with namespace prefix
     *
     * @param attribute xml attribute
     * @param ns namespace prefix
     * @return "ns:attribute"
     */
    static QString attributeNS(const QString &attribute, const XMLns &ns);

private:
    /// constructor
    MctStaticData();

    MctStaticData(const MctStaticData&);
    MctStaticData& operator=(const MctStaticData&);

    static MctStaticData* m_Instance;

    KoDocument *kodoc;              ///< kde document handler class

    QList<MctChange*> * changes;    ///< list of changes

    MctUndoGraph * undograph;       ///< undo graph
    MctRedoGraph * redograph;       ///< redo graph
    QString fileURL;                ///< path to document in action
    bool mctState;                  ///< MCT status indicator
    QString addedShapeType;         ///< ?
};

#endif // MCTSTATICDATA_H

