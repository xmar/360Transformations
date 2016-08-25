MainSource = slidesViewportAdaptiveStreaming.tex
OutputPDF = $(MainSource:.tex=.pdf)
Sections = $(filter-out $MainSource, $(wildcard *.tex))
Plots = $(wildcard ../plots/*.tex)
PlotsPdf = $(patsubst %.tex,%.pdf,$(Plots))

all: $(OutputPDF)

.PHONY: FORCE
$(OutputPDF): $(MainSource) $(Sections) $(Plots) FORCE
	../latexrun --latex-args="-shell-escape" $<

.PHONY: clean
clean:
	latexmk -CA
	rm -rf latex.out
	rm -rf *.dpth *.log *.dep *.pdf *.vrb
