-- MySQL dump 10.2
--
-- Host: localhost    Database: kugardb
---------------------------------------------------------
-- Server version	4.1.0-alpha-max-nt

--
-- Table structure for table 'salers'
--

DROP TABLE IF EXISTS salers;
CREATE TABLE salers (
  name varchar(100) NOT NULL default '',
  NumSaler int(10) unsigned NOT NULL auto_increment,
  PRIMARY KEY  (NumSaler)
) TYPE=MyISAM;

--
-- Dumping data for table 'salers'
--

/*!40000 ALTER TABLE salers DISABLE KEYS */;
LOCK TABLES salers WRITE;
INSERT INTO salers VALUES ('Red Hat',1),('Sun',2),('Microsoft',3),('Mandrake',4),('Suse',5);
UNLOCK TABLES;
/*!40000 ALTER TABLE salers ENABLE KEYS */;

--
-- Table structure for table 'software'
--

DROP TABLE IF EXISTS software;
CREATE TABLE software (
  NumSoftware int(10) unsigned NOT NULL auto_increment,
  NumSaler int(11) NOT NULL default '0',
  title varchar(100) NOT NULL default '',
  version varchar(10) NOT NULL default '',
  platform varchar(100) NOT NULL default '',
  copies int(11) NOT NULL default '0',
  PRIMARY KEY  (NumSoftware)
) TYPE=MyISAM;

--
-- Dumping data for table 'software'
--

/*!40000 ALTER TABLE software DISABLE KEYS */;
LOCK TABLES software WRITE;
INSERT INTO software VALUES (1,1,'Red hat linux','5.0','x86',1),(2,4,'Mandrake linux','9.1','x86',2),(3,4,'Mandrake linux','9.0','x86',3);
UNLOCK TABLES;
/*!40000 ALTER TABLE software ENABLE KEYS */;
