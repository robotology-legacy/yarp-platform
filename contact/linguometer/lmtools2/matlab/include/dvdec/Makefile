MEXFILES = dvopen.mexglx dvread.mexglx dvclose.mexglx
MEX = mex
MEX_LDADD = -ldv

all: $(MEXFILES)

$(MEXFILES): %.mexglx: %.c
	$(MEX) $< $(MEX_LDADD)

clean:
	rm -f $(MEXFILES)
