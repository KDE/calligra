// kpropcombo.h

#if !defined KPROPCOMBO_H
#define KPROPCOMBO_H

#include <qlabel.h>
#include <qcombobox.h>
#include <kconfig.h>


/** This class provides a label with a combo box.
  * You can get a pointer to the combo to configure it,
  * and the widget wil resize on font change.
  */
class KPropCombo : public QLabel
{
	Q_OBJECT
	
public:
	/** Constructor. 'text' contains the label text, 'perc' sets the 
	  * percentage of space reserved for the combo.
	  **/
	KPropCombo( QWidget *parent, const char *text, int perc, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. Deletes the combo. */
	virtual ~KPropCombo();
	
	/** Returns a pointer to the combo so you can configure it. */
	QComboBox *getCombo();
	
	/** Set the contents of the line edit widget.
	  * If no arguments are given, defaults to empty text.
	  */
	void setContents( const char * text=0 );

	/** Returns the current contents of the combo. */
	const char *getContents();

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
	/** Handles resizes for the combo. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();

	QComboBox *comboBox;
	int percentage;
	KConfig *ConfigObject;
	QString Group, Key;
};

#endif //KPROPCOMBO_H

