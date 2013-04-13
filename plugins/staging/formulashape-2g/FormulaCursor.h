/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#ifndef FORMULACURSOR_H
#define FORMULACURSOR_H
 
#include <QObject>
#include <QTimer>

class FormulaDocument;
class MmlNode;
class ChangeFormulaCommand;
class KUndo2Command;
 
class FormulaCursor : public QObject
{
  friend class ChangeFormulaCommand;
  friend class AlterFormulaCommand;
  
  Q_OBJECT
  
public:
    explicit FormulaCursor(FormulaDocument *document);
    ~FormulaCursor();
    
    void setNode(MmlNode *node);
    
    void nextNode();
    void previousNode();
    
    KUndo2Command *deleteNode();   
    //inserts text at the current cursor position
    KUndo2Command *insertText(const QString &text);
    KUndo2Command *deleteText();
    
    void activate();
    void deactivate();
    
public slots:
    void blinkCaret();
    
private:
    MmlNode *m_node;
    QTimer m_caretTimer;
    FormulaDocument *m_document;
    
    void init();
    MmlNode *nextNode(MmlNode *node);
    MmlNode *previousNode(MmlNode *node);
};
 
#endif //FORMULACURSOR_H