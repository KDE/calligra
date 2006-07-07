




########### install files ###############

INSTALL(FILES
	builtin.xml
	financial.xml
	trig.xml
	conversion.xml
	datetime.xml
	database.xml
	statistical.xml
	text.xml
	logic.xml
	math.xml
	information.xml
	engineering.xml
	reference.xml
    DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)





#original Makefile.am contents follow:

#
#scripts_DATA = builtin.xml financial.xml trig.xml conversion.xml datetime.xml \
#	database.xml statistical.xml text.xml logic.xml math.xml information.xml \
#	engineering.xml reference.xml
#scriptsdir = $(kde_datadir)/kspread/functions
#
