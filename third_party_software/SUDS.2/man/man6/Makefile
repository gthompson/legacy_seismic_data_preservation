
LIBS=	$(SUDSHOME)/libsuds/$(SUDSEXEC)/libsuds.a -lm $(DBLIB)

all: domains codelist install

# compile domains.dat which is authorities codelist sorted by
#   code and meaning
domains: 
	@if test -d $(SUDSEXEC) ; then continue ; else mkdir $(SUDSEXEC) ; fi
	$(CCSUD) $(CSFLAGS) -o $(SUDSEXEC)/scan_auth scan_auth.c
	$(SUDSEXEC)/scan_auth
	sort domain.txt > domain.num
	sort domain.txt -f +0.12 > domain.nam
	$(CCSUD) $(CSFLAGS) -o $(SUDSEXEC)/comp_dom comp_dom.c
	$(SUDSEXEC)/comp_dom
	-rm domain.num domain.nam $(SUDSEXEC)/scan_auth $(SUDSEXEC)/comp_dom 

# Compile codelist.dat, a file of SUDS_CODE_LIST structures
codelist:
	$(CCSUD) $(CSFLAGS) -o $(SUDSEXEC)/comp_cod comp_cod.c \
		$(LIBS) $(CSLIBS)
	$(SUDSEXEC)/comp_cod pc_code_lists.6 code_lists.6

# Copy to SUDS_INCLUDE
install:
	cp domains.dat ../../include
	cp codelist.dat codelist.idx ../../include
