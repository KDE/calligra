/***************************************************************************
              mreportdetail.h  -  Kugar report detail section
              -------------------
    begin     : Mon Aug 23 1999                                           
    copyright : (C) 1999 by Mutiny Bay Software                         
    email     : info@mutinybaysoftware.com                                     
 ***************************************************************************/

#ifndef MREPORTDETAIL_H
#define MREPORTDETAIL_H

#include "mfieldobject.h"
#include "mreportsection.h"

/**Kugar report detail section
  *@author Mutiny Bay Software
  */

class MReportDetail : public MReportSection  {

public:
  /** Constructor */
	MReportDetail();
  /** Copy constructor */
  MReportDetail(const MReportDetail& mReportDetail);
  /** Assignment operator */
  MReportDetail operator=(const MReportDetail& mReportDetail);
  /** Destructor */
	virtual ~MReportDetail();

protected:
  /** Section's field collection */
  QList<MFieldObject> fields;

public:
  /** Draws the detail section to the specified painter & x/y-offsets */
  void draw(QPainter* p, int xoffset, int yoffset);
  /** Adds a new field object to the section's field collection */
  void addField(MFieldObject* field);
  /** Sets the data for the field at the specified index */
  void setFieldData(int idx, QString data);
  /** Returns the number of fields in the detail section */
  int getFieldCount();
  /** Returns the name of the bound field for field object at the given index */
  QString getFieldName(int idx);
	/** Frees all resources allocated by the report section */
  void clear();

private:
  /** Copies member data from one object to another.
    * Used by the copy constructor and assignment operator
    */
  void copy(const MReportDetail* mReportDetail);

};

#endif




















