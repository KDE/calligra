/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

#ifndef _VTEXTTOOL_H_
#define _VTEXTTOOL_H_

#include "qframe.h"

#include "vcommand.h"
#include "vtool.h"
#include "vtext.h"

class KarbonView;
class QPushButton;
class KFontCombo;
class KIntNumInput;
class QCheckBox;
class QLineEdit;
class VTextTool;

class VTextOptionsWidget : public QFrame
{
	Q_OBJECT

	public:
		VTextOptionsWidget( VTextTool* tool, QWidget* parent = 0L );
		~VTextOptionsWidget();

		void setFont( const QFont& font );
		QFont font();
		void setText( const QString& text );
		QString text();
		void setPosition( VText::Position position );
		VText::Position position();

	public slots:
		void valueChanged( int );
		void accept();
		void textChanged( const QString& );
		void editBasePath();

	protected:
		KFontCombo*   m_fontCombo;
		QCheckBox*    m_boldCheck;
		QCheckBox*    m_italicCheck;
		KIntNumInput* m_fontSize;
		QLineEdit*    m_textEditor;
		QComboBox*    m_textPosition;
		QPushButton*  m_editBasePath;
		QPushButton*  m_convertToShapes;
		
		VTextTool*    m_tool;
}; // VTextOptionsWidget

class VTextTool : public VTool, public VVisitor
{
	public:
		VTextTool( KarbonView* view );
		~VTextTool();
		
		virtual QString name() { return i18n( "Text tool" ); }
		virtual QString contextHelp();
		virtual QWidget* optionsWidget() { return m_optionsWidget; }

		virtual void activate();
		virtual void deactivate();

		virtual void mouseButtonPress();
		virtual void mouseButtonRelease();
		virtual void mouseDrag();
		virtual void mouseDragRelease();
		virtual void textChanged();
		virtual void accept();
		virtual void cancel();
		virtual void editBasePath();

		virtual void visitVComposite( VComposite& composite );
		virtual void visitVDocument( VDocument& ) {}
		virtual void visitVGroup( VGroup& ) {}
		virtual void visitVLayer( VLayer& ) {}
		virtual void visitVPath( VPath& path );
		virtual void visitVText( VText& text );

	private:
		class VTextCmd : public VCommand
		{
			public:
				VTextCmd( VDocument* doc, KarbonView* view, const QString& name, VText* text );
				VTextCmd( VDocument* doc, KarbonView* view, const QString& name, VText* text,
						const QFont &newFont, const VPath& newBasePath, VText::Position newPosition, const QString& newText );
				virtual ~VTextCmd();

				virtual void execute();
				virtual void unexecute();
				virtual bool isExecuted() { return m_executed; }

			private:
				typedef struct 
				{
					QFont            oldFont;
					QFont            newFont;
					VPath            oldBasePath;
					VPath            newBasePath;
					VText::Position  oldPosition;
					VText::Position  newPosition;
					QString          oldText;
					QString          newText;
				} VTextModifPrivate;
				
				KarbonView*         m_view;
				VText*              m_text;
				bool                m_executed;
				VTextModifPrivate*  m_textModifications;
		}; // VTextCmd
		
		class VTextToCompositeCmd : public VCommand
		{
			public:
				VTextToCompositeCmd( VDocument* doc, const QString& name, VText* text );
				virtual ~VTextToCompositeCmd();

				virtual void execute() {}
				virtual void unexecute() {}
				virtual bool isExecuted() { return m_executed; }

			private:
				VText*   m_text;
				bool     m_executed;
		}; // VTextToCompositeCmd

		void drawPathCreation();
		void drawEditedText();
		
		VTextOptionsWidget* m_optionsWidget;
		KoPoint             m_last;
		VText*              m_text;
		VText*              m_editedText;
		bool                m_creating;
}; // VtextTool

#endif /* _VTEXTTOOL_H_ */
