// kpropradio.h

#if !defined KPROPRADIO_H
#define KPROPRADIO_H

#include <qradiobutton.h>
#include <qlayout.h>
#include <qlist.h>
#include <qbuttongroup.h>
#include <kconfig.h>

/** This class provides a box with radio buttons.
 * You can add Radio buttons which will be aligned verticaly.
 * From the box you can exclusively select 1 button.
 * The number of the button can be read from getState().
 */
class KPropRadio : public QButtonGroup
{
	Q_OBJECT
	
public:
	/** Contructor of the box. The 'text' is the title of the box. */ 
	KPropRadio( QWidget *parent, const char *text, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. Deletes all the radiobuttons. */
	virtual ~KPropRadio();
	
	/** You can preset a radiobutton with this method. 
	  * button is the number of the button
	  */
	void setState( int button );

	/** Returns the number of the selected button. 
	  * Returns -1 if no button is selected.
	  */
	int getState();

	/** Add a radio button to the box. 'text' is the text for the 
	  * radio button. 'state' is the state of the button. If it's 
	  * TRUE, the button is selected, and all other buttons are 
	  * deselected.
	  */
	QRadioButton *addRadio( const char *text, bool state );

	/** set the group to modify */
	void setGroup( const char *group );

	/** Returns the group to be modified */
	const char *getGroup();

	/** set the key to modify */
	void setKey( const char * );

	/** returns the Key to be modified */
	const char *getKey();

	/** Set the KConfig object to operate upon. */
	void setKConfig( KConfig * );

	/** Returns the current KConfig object */
	KConfig *getKConfig();


public slots:
	/** Write the config key value. . */
	virtual void setConfig();

	/** Read the config key value into the widget. 
	  * . 
	  */
	virtual void getConfig();
	
protected:
	/** Handles a resize of the radio box. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** If the font changes for this widget, this method is called.
	  * It will call sizeUpdate() to resize the widget.
	  */ 
	virtual void fontChange( const QFont &oldfont );

	/** This method will set the minimum size for the box and all 
	  * contained widgets so that all buttons will fit.
	  */
	void sizeUpdate();

	QVBoxLayout *layout;
	QList<QRadioButton> *radioList;
	
	int State;
	KConfig *ConfigObject;
	QString Group, Key;

};

#endif //KPROPRADIO_H

