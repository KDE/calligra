int drPropTest()
{
	QValueList<QCString> names = driver->propertyNames();
	kDebug() << QString("%1 properties found:").arg(names.count()) << endl;
	for (QValueList<QCString>::ConstIterator it = names.constBegin(); it!=names.constEnd(); ++it) {
		kDebug() << " - " << (*it) << ":" 
			<< " caption=\"" << driver->propertyCaption(*it) << "\""
			<< " type=" << driver->propertyValue(*it).typeName() 
			<< " value=\""<<driver->propertyValue(*it).toString()<<"\"" << endl;
	}
//		QVariant propertyValue( const QCString& propName ) const;

//		QVariant propertyCaption( const QCString& propName ) const;

	return 0;
}

