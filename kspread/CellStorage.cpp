/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "klocale.h"
#include "kpassivepopup.h"

#include "Damages.h"
#include "Doc.h"
#include "Map.h"
#include "RecalcManager.h"
#include "RectStorage.h"
#include "StyleStorage.h"

#include "CellStorage.h"

using namespace KSpread;

class KSpread::CellStorageUndoData
{
public:
    bool isEmpty() const
    {
        if ( !comments.isEmpty() )
            return false;
        if ( !conditions.isEmpty() )
            return false;
        if ( !formulas.isEmpty() )
            return false;
        if ( !fusions.isEmpty() )
            return false;
        if ( !links.isEmpty() )
            return false;
        if ( !matrices.isEmpty() )
            return false;
        if ( !styles.isEmpty() )
            return false;
        if ( !userInputs.isEmpty() )
            return false;
        if ( !validities.isEmpty() )
            return false;
        if ( !values.isEmpty() )
            return false;
        return true;
    }

    QList< QPair<QRectF,QString> >          comments;
    QList< QPair<QRectF,Conditions> >       conditions;
    QVector< QPair<QPoint,Formula> >        formulas;
    QList< QPair<QRectF,bool> >             fusions;
    QVector< QPair<QPoint,QString> >        links;
    QList< QPair<QRectF,bool> >             matrices;
    QList< QPair<QRectF,SharedSubStyle> >   styles;
    QVector< QPair<QPoint,QString> >        userInputs;
    QList< QPair<QRectF,Validity> >         validities;
    QVector< QPair<QPoint,Value> >          values;
};

class CellStorage::Private
{
public:
    Private( Sheet* sheet )
        : sheet( sheet )
        , commentStorage( new CommentStorage( sheet ) )
        , conditionsStorage( new ConditionsStorage( sheet ) )
        , formulaStorage( new FormulaStorage() )
        , fusionStorage( new FusionStorage( sheet ) )
        , linkStorage( new LinkStorage() )
        , matrixStorage( new MatrixStorage( sheet ) )
        , styleStorage( new StyleStorage( sheet ) )
        , userInputStorage( new UserInputStorage() )
        , validityStorage( new ValidityStorage( sheet ) )
        , valueStorage( new ValueStorage() )
        , undoData( 0 ) {}

    ~Private()
    {
        delete commentStorage;
        delete conditionsStorage;
        delete formulaStorage;
        delete fusionStorage;
        delete linkStorage;
        delete matrixStorage;
        delete styleStorage;
        delete userInputStorage;
        delete validityStorage;
        delete valueStorage;
    }

    Sheet*                  sheet;
    CommentStorage*         commentStorage;
    ConditionsStorage*      conditionsStorage;
    FormulaStorage*         formulaStorage;
    FusionStorage*          fusionStorage;
    LinkStorage*            linkStorage;
    MatrixStorage*          matrixStorage;
    StyleStorage*           styleStorage;
    UserInputStorage*       userInputStorage;
    ValidityStorage*        validityStorage;
    ValueStorage*           valueStorage;
    CellStorageUndoData*    undoData;
};

CellStorage::CellStorage( Sheet* sheet )
    : QObject( sheet )
    , d( new Private( sheet ) )
{
}

CellStorage::CellStorage( const CellStorage& other )
    : QObject( other.d->sheet )
    , d( new Private( *other.d ) )
{
}

CellStorage::~CellStorage()
{
    delete d;
}

void CellStorage::take( int col, int row )
{
    Formula oldFormula;
    QString oldLink;
    QString oldUserInput;
    Value oldValue;

    oldFormula = d->formulaStorage->take( col, row );
    oldLink = d->linkStorage->take( col, row );
    oldUserInput = d->userInputStorage->take( col, row );
    oldValue = d->valueStorage->take( col, row );

    // recording undo?
    if ( d->undoData )
    {
        d->undoData->formulas   << qMakePair( QPoint( col, row ), oldFormula );
        d->undoData->links      << qMakePair( QPoint( col, row ), oldLink );
        d->undoData->userInputs << qMakePair( QPoint( col, row ), oldUserInput );
        d->undoData->values     << qMakePair( QPoint( col, row ), oldValue );
    }
}

QString CellStorage::comment( int column, int row ) const
{
    return d->commentStorage->contains( QPoint( column, row ) );
}

void CellStorage::setComment( const Region& region, const QString& comment )
{
    // recording undo?
    if ( d->undoData )
        d->undoData->comments << d->commentStorage->undoData( region );

    d->commentStorage->insert( region, comment );
}

Conditions CellStorage::conditions( int column, int row ) const
{
    return d->conditionsStorage->contains( QPoint( column, row ) );
}

void CellStorage::setConditions( const Region& region, Conditions conditions )
{
    // recording undo?
    if ( d->undoData )
        d->undoData->conditions << d->conditionsStorage->undoData( region );

    d->conditionsStorage->insert( region, conditions );
}

Formula CellStorage::formula( int column, int row ) const
{
    if ( column == 0 || row == 0 )
        return Formula();
    return d->formulaStorage->lookup( column, row );
}

void CellStorage::setFormula( int column, int row, const Formula& formula )
{
    if ( column == 0 || row == 0 )
        return;

    Formula old;
    if ( formula.expression().isEmpty() )
        old = d->formulaStorage->take( column, row );
    else
        old = d->formulaStorage->insert( column, row, formula );

    // formula changed?
    if ( formula != old )
    {
        // trigger an update of the dependencies and a recalculation
        d->sheet->doc()->addDamage( new CellDamage( Cell( d->sheet, column, row ), CellDamage::Formula | CellDamage::Value ) );
        // recording undo?
        if ( d->undoData )
        {
            d->undoData->formulas << qMakePair( QPoint( column, row ), old );
            // Also store the old value, if there wasn't a formula before,
            // because the new value is calculated later by the damage
            // processing and is not recorded for undoing.
            if ( old == Formula() )
                d->undoData->values << qMakePair( QPoint( column, row ), value( column, row ) );
        }
    }
}

QString CellStorage::link( int column, int row ) const
{
    if ( column == 0 || row == 0 )
        return QString();
    return d->linkStorage->lookup( column, row );
}

void CellStorage::setLink( int column, int row, const QString& link )
{
    if ( column == 0 || row == 0 )
        return;

    QString old;
    if ( link.isEmpty() )
        old = d->linkStorage->take( column, row );
    else
        old = d->linkStorage->insert( column, row, link );

    // recording undo?
    if ( d->undoData && link != old )
        d->undoData->links << qMakePair( QPoint( column, row ), old );
}

Style CellStorage::style( int column, int row ) const
{
    return d->styleStorage->contains( QPoint( column, row ) );
}

Style CellStorage::style( const QRect& rect ) const
{
    return d->styleStorage->contains( rect );
}

void CellStorage::setStyle( const Region& region, const Style& style )
{
    // recording undo?
    if ( d->undoData )
        d->undoData->styles << d->styleStorage->undoData( region );

    d->styleStorage->insert( region, style );
}

QString CellStorage::userInput( int column, int row ) const
{
    if ( column == 0 || row == 0 )
        return QString();
    return d->userInputStorage->lookup( column, row );
}

void CellStorage::setUserInput( int column, int row, const QString& userInput )
{
    if ( column == 0 || row == 0 )
        return;

    QString old;
    if ( userInput.isEmpty() )
        old = d->userInputStorage->take( column, row );
    else
        old = d->userInputStorage->insert( column, row, userInput );

    // recording undo?
    if ( d->undoData && userInput != old )
        d->undoData->userInputs << qMakePair( QPoint( column, row ), old );
}

Validity CellStorage::validity( int column, int row ) const
{
    return d->validityStorage->contains( QPoint( column, row ) );
}

void CellStorage::setValidity( const Region& region, Validity validity )
{
    // recording undo?
    if ( d->undoData )
        d->undoData->validities << d->validityStorage->undoData( region );

    d->validityStorage->insert( region, validity );
}

Value CellStorage::value( int column, int row ) const
{
    if ( column == 0 || row == 0 )
        return Value();
    return d->valueStorage->lookup( column, row );
}

Value CellStorage::valueRegion( const Region& region ) const
{
    // create a subStorage with adjusted origin
    return Value( d->valueStorage->subStorage( region, false ) );
}

void CellStorage::setValue( int column, int row, const Value& value )
{
    if ( column == 0 || row == 0 )
        return;
    if ( isLocked( column, row ) )
    {
        KPassivePopup::message( i18n( "The cell is currently locked as element of a matrix." ), (QWidget*)0 );
        emit inform( i18n( "The cell is currently locked as element of a matrix." ) );
        return;
    }

    // release any lock
    unlockCells( column, row );

    Value old;
    if ( value.isEmpty() )
        old = d->valueStorage->take( column, row );
    else
        old = d->valueStorage->insert( column, row, value );

    // value changed?
    if ( value != old )
    {
        // Always trigger a repainting.
        CellDamage::Changes changes = CellDamage::Appearance;
        // Trigger a recalculation of the consuming cells, only if we are not
        // already in a recalculation process.
        if ( !d->sheet->map()->recalcManager()->isActive() )
            changes |= CellDamage::Value;
        d->sheet->doc()->addDamage( new CellDamage( Cell( d->sheet, column, row ), changes ) );
        // recording undo?
        if ( d->undoData )
            d->undoData->values << qMakePair( QPoint( column, row ), old );
    }
}

bool CellStorage::doesMergeCells( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return false;
    if ( pair.second == false )
        return false;
    // master cell?
    if ( pair.first.toRect().topLeft() != QPoint( column, row ) )
        return false;
    return true;
}

bool CellStorage::isPartOfMerged( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return false;
    if ( pair.second == false )
        return false;
    // master cell?
    if ( pair.first.toRect().topLeft() == QPoint( column, row ) )
        return false;
    return true;
}

void CellStorage::mergeCells( int column, int row, int numXCells, int numYCells )
{
    // Start by unmerging the cells that we merge right now
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( !pair.first.isNull() )
        d->fusionStorage->insert( Region( pair.first.toRect() ), false );
    // Merge the cells
    if ( numXCells != 0 || numYCells != 0 )
        d->fusionStorage->insert( Region( column, row, numXCells + 1, numYCells + 1 ), true );
}

Cell CellStorage::masterCell( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return Cell( d->sheet, column, row );
    if ( pair.second == false )
        return Cell( d->sheet, column, row );
    return Cell( d->sheet, pair.first.toRect().topLeft() );
}

int CellStorage::mergedXCells( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return 0;
    // Not the master cell?
    if ( pair.first.topLeft() != QPoint( column, row ) )
        return 0;
    return pair.first.toRect().width() - 1;
}

int CellStorage::mergedYCells( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->fusionStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return 0;
    // Not the master cell?
    if ( pair.first.topLeft() != QPoint( column, row ) )
        return 0;
    return pair.first.toRect().height() - 1;
}

bool CellStorage::locksCells( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->matrixStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return false;
    if ( pair.second == false )
        return false;
    // master cell?
    if ( pair.first.toRect().topLeft() != QPoint( column, row ) )
        return false;
    return true;
}

bool CellStorage::isLocked( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->matrixStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return false;
    if ( pair.second == false )
        return false;
    // master cell?
    if ( pair.first.toRect().topLeft() == QPoint( column, row ) )
        return false;
    return true;
}

void CellStorage::lockCells( const QRect& rect )
{
    kDebug() << k_funcinfo << endl;
    // Start by unlocking the cells that we lock right now
    const QPair<QRectF,bool> pair = d->matrixStorage->containedPair( rect.topLeft() ); // FIXME
    if ( !pair.first.isNull() )
        d->matrixStorage->insert( Region( pair.first.toRect() ), false );
    // Lock the cells
    if ( rect.width() > 1 || rect.height() > 1 )
        d->matrixStorage->insert( Region( rect ), true );
}

void CellStorage::unlockCells( int column, int row )
{
    kDebug() << k_funcinfo << endl;
    const QPair<QRectF,bool> pair = d->matrixStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return;
    if ( pair.second == false )
        return;
    if ( pair.first.toRect().topLeft() != QPoint( column, row ) )
        return;
    const QRect rect = pair.first.toRect();
    d->matrixStorage->insert( Region( rect ), false );
    // clear the values
    for ( int r = rect.top(); r <= rect.bottom(); ++r )
    {
        for ( int c = rect.left(); c <= rect.right(); ++c )
        {
            if ( r != rect.top() || c != rect.left() )
                setValue( c, r, Value() );
        }
    }
}

QRect CellStorage::lockedCells( int column, int row ) const
{
    const QPair<QRectF,bool> pair = d->matrixStorage->containedPair( QPoint( column, row ) );
    if ( pair.first.isNull() )
        return QRect( column, row, 1, 1 );
    if ( pair.second == false )
        return QRect( column, row, 1, 1 );
    if ( pair.first.toRect().topLeft() != QPoint( column, row ) )
        return QRect( column, row, 1, 1 );
    return pair.first.toRect();
}

void CellStorage::insertColumns( int position, int number )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->insertColumns( position, number );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->insertColumns( position, number );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->insertColumns( position, number );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->insertColumns( position, number );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->insertColumns( position, number );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->insertColumns( position, number );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->insertColumns( position, number );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->insertColumns( position, number );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->insertColumns( position, number );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->insertColumns( position, number );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::removeColumns( int position, int number )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->removeColumns( position, number );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->removeColumns( position, number );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->removeColumns( position, number );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->removeColumns( position, number );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->removeColumns( position, number );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->removeColumns( position, number );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->removeColumns( position, number );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->removeColumns( position, number );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->removeColumns( position, number );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->removeColumns( position, number );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::insertRows( int position, int number )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->insertRows( position, number );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->insertRows( position, number );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->insertRows( position, number );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->insertRows( position, number );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->insertRows( position, number );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->insertRows( position, number );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->insertRows( position, number );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->insertRows( position, number );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->insertRows( position, number );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->insertRows( position, number );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::removeRows( int position, int number )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->removeRows( position, number );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->removeRows( position, number );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->removeRows( position, number );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->removeRows( position, number );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->removeRows( position, number );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->removeRows( position, number );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->removeRows( position, number );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->removeRows( position, number );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->removeRows( position, number );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->removeRows( position, number );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::removeShiftLeft( const QRect& rect )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->removeShiftLeft( rect );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->removeShiftLeft( rect );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->removeShiftLeft( rect );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->removeShiftLeft( rect );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->removeShiftLeft( rect );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->removeShiftLeft( rect );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->removeShiftLeft( rect );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->removeShiftLeft( rect );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->removeShiftLeft( rect );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->removeShiftLeft( rect );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::insertShiftRight( const QRect& rect )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->insertShiftRight( rect );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->insertShiftRight( rect );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->insertShiftRight( rect );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->insertShiftRight( rect );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->insertShiftRight( rect );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->insertShiftRight( rect );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->insertShiftRight( rect );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->insertShiftRight( rect );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->insertShiftRight( rect );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->insertShiftRight( rect );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::removeShiftUp( const QRect& rect )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->removeShiftUp( rect );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->removeShiftUp( rect );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->removeShiftUp( rect );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->removeShiftUp( rect );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->removeShiftUp( rect );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->removeShiftUp( rect );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->removeShiftUp( rect );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->removeShiftUp( rect );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->removeShiftUp( rect );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->removeShiftUp( rect );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

void CellStorage::insertShiftDown( const QRect& rect )
{
    QList< QPair<QRectF,QString> > comments = d->commentStorage->insertShiftDown( rect );
    QList< QPair<QRectF,Conditions> > conditions = d->conditionsStorage->insertShiftDown( rect );
    QVector< QPair<QPoint,Formula> > formulas = d->formulaStorage->insertShiftDown( rect );
    QList< QPair<QRectF,bool> > fusions = d->fusionStorage->insertShiftDown( rect );
    QList< QPair<QRectF,bool> > matrices = d->matrixStorage->insertShiftDown( rect );
    QVector< QPair<QPoint,QString> > links = d->linkStorage->insertShiftDown( rect );
    QList< QPair<QRectF,SharedSubStyle> > styles = d->styleStorage->insertShiftDown( rect );
    QVector< QPair<QPoint,QString> > userInputs = d->userInputStorage->insertShiftDown( rect );
    QList< QPair<QRectF,Validity> > validities = d->validityStorage->insertShiftDown( rect );
    QVector< QPair<QPoint,Value> > values = d->valueStorage->insertShiftDown( rect );
    // recording undo?
    if ( d->undoData )
    {
        d->undoData->comments   << comments;
        d->undoData->conditions << conditions;
        d->undoData->formulas   << formulas;
        d->undoData->fusions    << fusions;
        d->undoData->links      << links;
        d->undoData->matrices   << matrices;
        d->undoData->styles     << styles;
        d->undoData->userInputs << userInputs;
        d->undoData->validities << validities;
        d->undoData->values     << values;
    }
}

Cell CellStorage::firstInColumn( int col ) const
{
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->firstInColumn( col, &tmpRow );
    newRow = tmpRow;
    d->valueStorage->firstInColumn( col, &tmpRow );
    if ( tmpRow )
        newRow = newRow ? qMin( newRow, tmpRow ) : tmpRow;
    if ( !tmpRow )
        return Cell();
    return Cell( d->sheet, col, newRow );
}

Cell CellStorage::firstInRow( int row ) const
{
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->firstInRow( row, &tmpCol );
    newCol = tmpCol;
    d->valueStorage->firstInRow( row, &tmpCol );
    if ( tmpCol )
        newCol = newCol ? qMin( newCol, tmpCol ) : tmpCol;
    if ( !tmpCol )
        return Cell();
    return Cell( d->sheet, newCol, row );
}

Cell CellStorage::lastInColumn( int col ) const
{
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->lastInColumn( col, &tmpRow );
    newRow = tmpRow;
    d->valueStorage->lastInColumn( col, &tmpRow );
    newRow = qMax( newRow, tmpRow );
    if ( !tmpRow )
        return Cell();
    return Cell( d->sheet, col, newRow );
}

Cell CellStorage::lastInRow( int row ) const
{
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->lastInRow( row, &tmpCol );
    newCol = tmpCol;
    d->valueStorage->lastInRow( row, &tmpCol );
    newCol = qMax( newCol, tmpCol );
    if ( !tmpCol )
        return Cell();
    return Cell( d->sheet, newCol, row );
}

Cell CellStorage::nextInColumn( int col, int row ) const
{
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->nextInColumn( col, row, &tmpRow );
    newRow = tmpRow;
    d->valueStorage->nextInColumn( col, row, &tmpRow );
    if ( tmpRow )
        newRow = newRow ? qMin( newRow, tmpRow ) : tmpRow;
    if ( !tmpRow )
        return Cell();
    return Cell( d->sheet, col, newRow );
}

Cell CellStorage::nextInRow( int col, int row ) const
{
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->nextInRow( col, row, &tmpCol );
    newCol = tmpCol;
    d->valueStorage->nextInRow( col, row, &tmpCol );
    if ( tmpCol )
        newCol = newCol ? qMin( newCol, tmpCol ) : tmpCol;
    if ( !tmpCol )
        return Cell();
    return Cell( d->sheet, newCol, row );
}

Cell CellStorage::prevInColumn( int col, int row ) const
{
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->prevInColumn( col, row, &tmpRow );
    newRow = tmpRow;
    d->valueStorage->prevInColumn( col, row, &tmpRow );
    newRow = qMax( newRow, tmpRow );
    if ( !tmpRow )
        return Cell();
    return Cell( d->sheet, col, newRow );
}

Cell CellStorage::prevInRow( int col, int row ) const
{
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->prevInRow( col, row, &tmpCol );
    newCol = tmpCol;
    d->valueStorage->prevInRow( col, row, &tmpCol );
    newCol = qMax( newCol, tmpCol );
    if ( !tmpCol )
        return Cell();
    return Cell( d->sheet, newCol, row );
}

int CellStorage::columns() const
{
    int max = 0;
    max = qMax( max, d->commentStorage->usedArea().right() );
    max = qMax( max, d->conditionsStorage->usedArea().right() );
    max = qMax( max, d->fusionStorage->usedArea().right() );
    max = qMax( max, d->styleStorage->usedArea().right() );
    max = qMax( max, d->validityStorage->usedArea().right() );
    max = qMax( max, d->formulaStorage->columns() );
    max = qMax( max, d->linkStorage->columns() );
    max = qMax( max, d->valueStorage->columns() );
    return max;
}

int CellStorage::rows() const
{
    int max = 0;
    max = qMax( max, d->commentStorage->usedArea().bottom() );
    max = qMax( max, d->conditionsStorage->usedArea().bottom() );
    max = qMax( max, d->fusionStorage->usedArea().bottom() );
    max = qMax( max, d->styleStorage->usedArea().bottom() );
    max = qMax( max, d->validityStorage->usedArea().bottom() );
    max = qMax( max, d->formulaStorage->rows() );
    max = qMax( max, d->linkStorage->rows() );
    max = qMax( max, d->valueStorage->rows() );
    return max;
}

CellStorage CellStorage::subStorage( const Region& region ) const
{
    CellStorage subStorage( d->sheet );
    *subStorage.d->formulaStorage = d->formulaStorage->subStorage( region );
    *subStorage.d->linkStorage = d->linkStorage->subStorage( region );
    *subStorage.d->valueStorage = d->valueStorage->subStorage( region );
    return subStorage;
}

const CommentStorage* CellStorage::commentStorage() const
{
    return d->commentStorage;
}

const ConditionsStorage* CellStorage::conditionsStorage() const
{
    return d->conditionsStorage;
}

const FormulaStorage* CellStorage::formulaStorage() const
{
    return d->formulaStorage;
}

const FusionStorage* CellStorage::fusionStorage() const
{
    return d->fusionStorage;
}

const LinkStorage* CellStorage::linkStorage() const
{
    return d->linkStorage;
}

StyleStorage* CellStorage::styleStorage() const
{
    return d->styleStorage;
}

const ValidityStorage* CellStorage::validityStorage() const
{
    return d->validityStorage;
}

const ValueStorage* CellStorage::valueStorage() const
{
    return d->valueStorage;
}

void CellStorage::startUndoRecording()
{
    // If undoData is not null, the recording wasn't stopped.
    // Should not happen, hence this assertion.
    Q_ASSERT( d->undoData == 0 );
    d->undoData = new CellStorageUndoData();
}

CellStorageUndoData* CellStorage::stopUndoRecording()
{
    // If undoData is null, the recording wasn't started.
    // Should not happen, hence this assertion.
    Q_ASSERT( d->undoData != 0 );
    CellStorageUndoData* undoData = d->undoData;
    d->undoData = 0;
    // do not store an object unnecessarily
    if ( undoData->isEmpty() )
    {
        delete undoData;
        undoData = 0;
    }
    return undoData;
}

void CellStorage::undo( CellStorageUndoData* data )
{
    if ( !data ) // nothing to do?
        return;
    for ( int i = 0; i < data->formulas.count(); ++i )
        setFormula( data->formulas[i].first.x(), data->formulas[i].first.y(), data->formulas[i].second );
    for ( int i = 0; i < data->values.count(); ++i )
        setValue( data->values[i].first.x(), data->values[i].first.y(), data->values[i].second );
    for ( int i = 0; i < data->userInputs.count(); ++i )
        setUserInput( data->userInputs[i].first.x(), data->userInputs[i].first.y(), data->userInputs[i].second );
    for ( int i = 0; i < data->links.count(); ++i )
        setLink( data->links[i].first.x(), data->links[i].first.y(), data->links[i].second );
    for ( int i = 0; i < data->fusions.count(); ++i )
        d->fusionStorage->insert( Region(data->fusions[i].first.toRect()), data->fusions[i].second );
    for ( int i = 0; i < data->styles.count(); ++i )
        d->styleStorage->insert( data->styles[i].first.toRect(), data->styles[i].second );
    for ( int i = 0; i < data->comments.count(); ++i )
        setComment( Region(data->comments[i].first.toRect()), data->comments[i].second );
    for ( int i = 0; i < data->conditions.count(); ++i )
        setConditions( Region(data->conditions[i].first.toRect()), data->conditions[i].second );
    for ( int i = 0; i < data->validities.count(); ++i )
        setValidity( Region(data->validities[i].first.toRect()), data->validities[i].second );
}

#include "CellStorage.moc"
