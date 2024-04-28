/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoSopranoTableModel_h__
#define __rdf_KoSopranoTableModel_h__

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <Soprano/Soprano>

class KoDocumentRdf;

/**
 * @short custom TableModel for editing a Soprano::Model
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoDocumentRdf
 *
 * Various columns are exposed to allow editing of not only
 * the triple itself but also the object type, data type,
 * and storage location of the triple.
 */
class KoSopranoTableModel : public QAbstractTableModel
{
    KoDocumentRdf *m_rdf;
    /**
     * Because we need to be able to lookup triples by
     * row number, a QList is created giving efficient index
     * lookup for statements. Note that Soprano::Statement
     * uses pimpl so we are not really copying statements here.
     */
    QList<Soprano::Statement> m_statementIndex;

    /**
     * True if the statement is stored in content.xml
     */
    bool isInlineRdf(const Soprano::Statement &st) const;

public:
    explicit KoSopranoTableModel(KoDocumentRdf *rdf);

    /**
     * Get the RDF model we this class is showing.
     * You should not delete the return value.
     */
    QSharedPointer<Soprano::Model> model() const;

    enum {
        ColIsValid = 0, // Is this triple valid
        ColSubj, // subject
        ColPred, // predicate
        ColObj, // object
        ColObjType, // string for type, eg, URI, Literal, Blank Node
        ColObjXsdType, // XSD type URI for object
        ColCtx, // Graph context for triple
        ColCount // NOT A COLUMN but the size.
    };

    /**
     * Convenience method, same as m_rdf->getPrefixMapping()
     */
    QString URItoPrefexedLocalname(const QString &uri) const;
    /**
     * Convenience method, same as m_rdf->getPrefixMapping()
     */
    QString PrefexedLocalnameToURI(const QString &pname);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * You MUST use this method if you want to change a Statement.
     *
     * Used by setData() to remove the old statement and replace it with the new 'n' one.
     * The internal m_statementIndex int->statement is updated
     * as well as the dataChanged signal emitted
     */
    bool setDataUpdateTriple(const QModelIndex &index, const Soprano::Statement &old, const Soprano::Statement &n);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /**
     * Add the statement 'st' to the model as the new last row.
     */
    int insertStatement(const Soprano::Statement &st);

    /**
     * Copy all the triples in srclist to be new rows in the model.
     * Note that the object value is modified to contain a unique
     * postfix so that the new triple copies can be inserted into
     * the Rdf model. It is a copy in a looser sense of the word.
     */
    QModelIndexList copyTriples(const QModelIndexList &srclist);

    /**
     * Delete all the triples in srclist from the model.
     */
    void deleteTriples(const QModelIndexList &srclist);

    /**
     * Find the statement at the given row.
     */
    Soprano::Statement statementAtIndex(const QModelIndex &mi) const;

    /**
     * The number of statements that have been partially or invalidly
     * edited.
     *
     * @see invalidStatementList()
     */
    int invalidStatementCount() const;

    /**
     * All the statements in the UI which are not valid Soprano::Statements
     *
     * @see invalidStatementCount()
     */
    QModelIndexList invalidStatementList() const;
};

#endif
