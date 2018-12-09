doc.dvi: manual.tex Makefile local.bib
	pdftex $<
	bibtex manual
	pdftex $<
	pdftex $<

%.ps: %.dvi
	dvips -f -t a4 $< > $@

%.pdf: %.ps
	ps2pdf14 -sPAPERSIZE=letter $<

clean:
	rm -f *.aux *.dvi *.log *.blg *.bbl *.pdf *.ps *.toc
