############################################################################
# slightly more user-friendly interface to a crufty latex makefile

# give the name (without extension) of the main tex file

MAIN = main

# give the name (without extension) of the main bibliography file

BMAIN = main


# generate and view dvi file
dvi:
	make figs f dd


# generate and view ps file
ps:
	make figs f gv

# generate and view pdf file
pdf:
	make figs f $(MAIN).pdf && xpdf $(MAIN).pdf


# repeat latex/bibtex a few times to make sure everything is happy
happy:
	make figs
	make bounce

final:
	make bounce
	make  $(MAIN).pdf
	cp  $(MAIN).pdf $(HOME)/d0107-exploratory-play.pdf


############################################################################
############################################################################
############################################################################
############################################################################
# now we delve into cruftiness

ifeq ($(shell uname),Linux)
GEOMETRY = -geometry +0+0
else
GEOMETRY =
endif


SUBMAIN = $(wildcard *.tex) $(wildcard *.sty)

all: $(MAIN).dvi

%.pssrc: %.prn
	cp $< $@

%.eps: %.jpg
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.gif
	convert $^ eps:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.png
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.epssrc
	cp $^ $@

%.eps: %.pssrc
ifeq ($(shell uname),Linux)
	@echo "On linux, so using straightforward eps generation method"
	ps2eps -f -C  -n -c $<
#	ps2eps --size=a0 -f -C  -n -c $<
	mv $<.eps $@
else
	@echo "***"
	@echo "*** using devious eps generation method, be careful"
	@echo "***"
	grep -q "Title: Microsoft PowerPoint" $< && ps2eps $< || ps2eps -f -C  -n -c $<
	( sed "s/statusdict/%%%statusdict/g" | sed "s/^(%%\[[^ ]/%%(%%\[/" | grep -v "1 dict dup" | grep -v "ImagingBBox null" | sed "s/: N 111 /: %%N 111 /"  | sed "s/: N 803 /: %%N 803 /" ) < $<.eps > $@ && rm $<.eps
endif

#%.eps: %.pssrc
#	ps2eps $<
#	( sed "s/statusdict/%%%statusdict/g" | sed "s/^(%%\[[^ ]/%%(%%\[/" | sed "s/: N 111 /: %%N 111 /" ) < $<.eps > $@
#	rm -f $^.eps

%.eps: %.pssrci
	ps2epsi $<
	sed "s/statusdict/%%%statusdict/g" < $<.epsi > $@
	rm -f $<.epsi
#	mv $^.epsi $@

%.bbl: %.bib
#	latex $*
	bibtex $*

#$(MAIN).bbl: $(BMAIN).bib
#	latex $(MAIN)
#	cp $(MAIN).aux $(BMAIN).aux
#	bibtex $(BMAIN)

%.pdf: %.ps
	ps2pdf $< $@
#	distill -v -compatlevel 2.1 -colordownsample on -graydownsample on -monodownsample on $^
#	distill -compatlevel 2.1 -colordownsample off -graydownsample off -monodownsample off $^

dd: $(MAIN).dvi
	xdvi $(GEOMETRY) $(MAIN).dvi

# need appropriate focus rules on win manager for this to work
#rep:
#	sh -c "while true do\
#	 ( make vision.dvi; cp vision.dvi no-focus.dvi; ( xdvi no-focus.dvi & ); echo Hi | tee rep; while -e rep do; ( make | grep latex && rm rep ); sleep 1; done; )  done"

figs: $(EPS)

d: $(MAIN).dvi
	xdvi -noghostscript $(GEOMETRY) $(MAIN).dvi
#	xdvi $(MAIN).dvi

gv: $(MAIN).ps
	gv $(GEOMETRY) $(MAIN).ps

p: $(MAIN).ps

$(MAIN).half.ps: $(MAIN).ps
	mpage -2 < $< > $@

p2: $(MAIN).half.ps

pdf2: $(MAIN).half.pdf

b: $(BMAIN).bbl

fb:
	rm -f $(BMAIN).bbl
	make $(BMAIN).bbl

f:
	latex $(MAIN)

wc:
	cat *.tex | wc

join:
	cat *.tex | egrep -v "^\%"

miss:
	@egrep "(Warning: ((Reference)|(Citation)))|(\.tex)" $(MAIN).log

bounce:
	make fb ; make f ; make fb ; make f ; make fb ; make f

$(MAIN).dvi: $(MAIN).tex $(BMAIN).bbl $(SUBMAIN)
	latex $(MAIN).tex

force:
	latex $(MAIN).tex

$(MAIN).ps: $(MAIN).dvi
#	dvips -t a4 -f $< > $@
	dvips -t letter -f $< > $@

short:
	cp defs-iros.tex defs.tex
	make bounce

long:
	cp defs-epi.tex defs.tex
	make bounce

commit:
	cvs commit -m "Makefile-generated commit"

c: commit

update:
	cvs update

u: update

# only works on a sun, with distill installed
pdf: $(MAIN).pdf

clean:
	rm -f $(MAIN).dvi $(MAIN).ps

