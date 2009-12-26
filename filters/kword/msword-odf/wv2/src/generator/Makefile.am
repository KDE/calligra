# Some convenience stuff for the code generator...

generated: scanner converter

scanner:
	@cd $(srcdir) && perl generate.pl generator_wword6.htm Word95 ;\
	perl generate.pl generator_wword8.htm Word97 ;\
	cmp -s word95_generated.h ../word95_generated.h || cp word95_generated.h .. ;\
	cmp -s word95_generated.cpp ../word95_generated.cpp || cp word95_generated.cpp .. ;\
	cmp -s word97_generated.h ../word97_generated.h || cp word97_generated.h .. ;\
	cmp -s word97_generated.cpp ../word97_generated.cpp || cp word97_generated.cpp .. ;\
	cmp -s word95_test.cpp ../../tests/word95_test.cpp || cp word95_test.cpp ../../tests ;\
	cmp -s word97_test.cpp ../../tests/word97_test.cpp || cp word97_test.cpp ../../tests

converter:
	@cd $(srcdir) && perl converter.pl generator_wword6.htm generator_wword8.htm ;\
	cmp -s convert.cpp ../convert.cpp || cp convert.cpp .. ;\
	cmp -s convert.h ../convert.h || cp convert.h ..
