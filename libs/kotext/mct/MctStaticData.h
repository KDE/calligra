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

class MctStaticData
{
public:    

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

    static const XMLns XMLNS;
    static const XMLns NS_DC;
    static const XMLns NS_C;
    static const XMLns NS_OFFICE;

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

    // xml string constants (tags, attribute values, etc)
    static const QString ADDED;
    static const QString REMOVED;
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

    static int getTextPropAsInt(QString propertyname);
    static QString getTextPropetyString(int key, int type);

    static const QMap<MctChangeTypes, QString> CHANGE_AS_STRING;
    static const QMap<MctChangeTypes, QString> initCHANGE_AS_STRING();

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
    QString tempMctFile(QString url);
    void includeEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);
    void backupEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);
    void deleteEmbeddedFiles(QMap<QString, MctEmbObjProperties*> delemet);
    void exportGraphs();
    //determineType(URL, Context)
    //getTreeNodes(QList<DisplayValue*> display_values, TreeModel treeModel);
    void attachListeners(/*dialogStatvarsInstance*/);
    void fillUpWithSuppliers(/*doc, staticvars*/);
    void fillUpWithSupplier(/*container, delement, OdtURL*/);
    void fillUpWithSupplier2(/*container, delement*/);
    void stopSupport(QString url);

    // function2.py
    QList<QString> getDataFromPosTag(const QString &postag);
    MctPosition * getPosFromElementreeNode(const QDomElement &change, bool moved=false);
    MctPosition * getPosFromElementreeNodeNew(const QDomElement &change, bool moved=false);
    MctPosition * getPosFromElementreeNodeOld(const QDomElement &change, bool moved=false);
    void addPos2change(QDomElement *change, MctPosition *pos, bool moved=false, bool clearfirst=true);
    bool isTableChange(const MctChange *changeNode);

    FIXME_TextInterface * getTextInterface(FIXME_TextInterface *parent, MctPosition *position);
    QTextCursor * CreateCursorFromRecursivePos(KoTextDocument *doc, MctPosition *position);
    QTextCursor * CreateCursorFromPos( FIXME_TextInterface *xtext_interface, MctPosition *position);

    // function3.py
    QList<int> *findNewLines(QString string);
    MctChange * createRedoChangeNode(KoTextDocument *doc, MctChange *changeNode);
    void insertTextGraphicObjects(MctChange *changeNode, FIXME_Doc * doc/*,staticvars */,bool withprops=true, FIXME_TextInterface *xtext_interface=NULL);
    void instertTextTable(MctChange *changeNode/*,staticvars */, bool withprops=true, FIXME_TextInterface *xtext_interface=NULL);
    FIXME_PropertyDictionary * getNoneDefaultProps(MctEmbObjProperties *props);

    // own functions
    static void recursiveDirectoryCopy(const KArchiveDirectory *dir, const QString &path, KZip *destinationZip);
    static void textPropMapTester(int key);
    static bool hasMctSupport(QString odt);

    MctUndoGraph* getUndoGraph();
    void setUndoGraph(MctUndoGraph* graph);
    MctRedoGraph* getRedoGraph();
    void setRedoGraph(MctRedoGraph* graph);

    QString getFileURL();
    void setFileURL(const QString &url);

    QList<MctChange*>* getChanges();
    void setChanges(QList<MctChange*> *change);
    void clearChanges();

    void setKoDocument(KoDocument* doc);
    KoDocument* getKoDocument();

    QMap<ulong, ulong> * getFrameIndexes(const KoTextDocument *koTextDoc);    

    bool getMctState() const;
    void setMctState(bool state);

    QString getAddedShapeType();
    void setAddedShapeType(const QString &shapeType);

    // xml
    static QString attributeNS(const QString &attribute, const XMLns &ns);

private:
    MctStaticData();

    MctStaticData(const MctStaticData&);
    MctStaticData& operator=(const MctStaticData&);

    static MctStaticData* m_Instance;

    KoDocument *kodoc;

    QList<MctChange*> * changes;

    MctUndoGraph * undograph;
    MctRedoGraph * redograph;
    QString fileURL;
    bool mctState;
    QString addedShapeType;
};

#endif // MCTSTATICDATA_H

