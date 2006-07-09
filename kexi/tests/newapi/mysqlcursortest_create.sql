-- MySQL dump 9.08
--
-- Host: localhost    Database: test
---------------------------------------------------------
-- Server version	4.0.14

--
-- Table structure for table 'Applications'
--

CREATE TABLE Applications (
  id int(11) NOT NULL default '0',
  value varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Dumping data for table 'Applications'
--

INSERT INTO Applications VALUES (0,'A');
INSERT INTO Applications VALUES (1,'B');
INSERT INTO Applications VALUES (2,'C');
INSERT INTO Applications VALUES (3,'D');
INSERT INTO Applications VALUES (4,'E');

--
-- Table structure for table 'jw1'
--

CREATE TABLE jw1 (
  id int(11) NOT NULL default '0',
  txt varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

--
-- Dumping data for table 'jw1'
--


