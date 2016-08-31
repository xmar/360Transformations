MainSource = slidesViewportAdaptiveStreaming.tex
OutputPDF = $(MainSource:.tex=.pdf)
OutputPDFXavier = $(MainSource:.tex=_xavier.pdf)
OutputPDFGwendal = $(MainSource:.tex=_gwendal.pdf)
OutputPDFGwendal5G = $(MainSource:.tex=_gwendal_5G.pdf)
Sections = $(filter-out $MainSource, $(wildcard *.tex))
Plots = $(wildcard ../plots/*.tex)
PlotsPdf = $(patsubst %.tex,%.pdf,$(Plots))

.PHONY: xavier
xavier: $(OutputPDFXavier)

.PHONY: all
all: $(OutputPDFGwendal) $(OutputPDFXavier) $(OutputPDFGwendal5G) $(OutputPDFXavier)

.PHONY: gwendal
gwendal: $(OutputPDFGwendal)

.PHONY: gwendal5G
gwendal5G: $(OutputPDFGwendal5G)

.PHONY: FORCE
$(OutputPDF): $(MainSource) $(Sections) $(Plots) FORCE
	make cleanLn
	ln -s whoIsXavier.tex whoIsSlide.tex
	ln -s /dev/null extra5G.tex
	../latexrun --latex-args="-shell-escape" $<

$(OutputPDFXavier): $(MainSource) $(Sections) $(Plots) FORCE
	make cleanLn
	ln -s whoIsXavier.tex whoIsSlide.tex
	ln -s /dev/null extra5G.tex
	../latexrun --latex-args="-shell-escape" $<
	cp $(OutputPDF) $(OutputPDFXavier)

$(OutputPDFGwendal): $(MainSource) $(Sections) $(Plots) FORCE
	make cleanLn
	ln -s whoIsGwendal.tex whoIsSlide.tex
	ln -s /dev/null extra5G.tex
	#../latexrun --latex-args="-shell-escape" -O latex.out_gwendal $<
	../latexrun --latex-args="-shell-escape" $<
	cp $(OutputPDF) $(OutputPDFGwendal)

$(OutputPDFGwendal5G): $(MainSource) $(Sections) $(Plots) cleanLn FORCE
	make cleanLn
	ln -s whoIsGwendal.tex whoIsSlide.tex
	ln -s extra5GSlide.tex extra5G.tex
	#../latexrun --latex-args="-shell-escape" -O latex.out_gwendal_5G $<
	../latexrun --latex-args="-shell-escape" $<
	cp $(OutputPDF) $(OutputPDFGwendal5G)

.PHONY: clean
clean: cleanLn
	latexmk -CA
	-rm -rf latex.out*
	-rm -rf *.dpth *.log *.dep *.pdf *.vrb

.PHONY: cleanLn
cleanLn:
	-rm whoIsSlide.tex
	-rm extra5G.tex
