/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net> 
                 2009 Jeremias Epperlein <jeeree@web.de>

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

#ifndef KOFORMULATOOL_H
#define KOFORMULATOOL_H

#include <KoToolBase.h>

class KoFormulaShape;
class FormulaEditor;
class FormulaCommand;

struct TemplateAction {
    QAction *action;
    QString data;
};

/**
 * @short The flake tool for a formula
 * @author Martin Pfeiffer <hubipete@gmx.net>
 */
class KoFormulaTool : public KoToolBase {
    Q_OBJECT
public:
    /// The standard constructor
    explicit KoFormulaTool( KoCanvasBase *canvas );

    /// The standard destructor
    ~KoFormulaTool() override;

    /// reimplemented
    void paint( QPainter &painter, const KoViewConverter &converter ) override;

    /// reimplemented
    void mousePressEvent( KoPointerEvent *event ) override ;

    /// reimplemented
    void mouseDoubleClickEvent( KoPointerEvent *event ) override;

    /// reimplemented
    void mouseMoveEvent( KoPointerEvent *event ) override;

    /// reimplemented
    void mouseReleaseEvent( KoPointerEvent *event ) override;

    void keyPressEvent( QKeyEvent *event ) override;

    void keyReleaseEvent( QKeyEvent *event ) override;

    void remove( bool backSpace );

    /// @return The currently manipulated KoFormulaShape
    KoFormulaShape* shape();

    /// @return The currently active cursor
    FormulaEditor* formulaEditor();

    /// Reset the cursor
    void resetFormulaEditor();

public Q_SLOTS:
    /// Called when this tool instance is activated and fills m_formulaShape
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;

    /// Called when this tool instance is deactivated
    void deactivate() override;

    /// Insert the element tied to the given @p action
    void insert( const QString& action );

    void changeTable( QAction* action);
    
    void insertSymbol( const QString& symbol);

    /// Reposition the cursor according to the data change
    void updateCursor(FormulaCommand* command, bool undo);

    void saveFormula();

    void loadFormula();

    
protected:
    /// Create default option widget
    QWidget* createOptionWidget() override;

    void copy() const override;

    void deleteSelection() override;

    bool paste() override;

    QStringList supportedPasteMimeTypes() const override;
    
private:
    /// Repaint the cursor and selection
    void repaintCursor();

    /// Creates all the actions provided by the tool
    void setupActions();

    void addTemplateAction(const QString &caption, const QString &name, const QString &data, const char *iconName);

    /// The FormulaShape the tool is manipulating
    KoFormulaShape* m_formulaShape;

    /// The FormulaEditor the tool uses to move around in the formula
    FormulaEditor* m_formulaEditor;

    QList<FormulaEditor*> m_cursorList;

    std::vector<TemplateAction> m_templateActions;
};

#endif
