MainSource = mainACM.tex
Sections = $(filter-out $MainSource, $(wildcard *.tex))
Bib = $(wildcard *.bib)
Plots = $(wildcard plots/*.tex)
PlotsPdf = $(patsubst %.tex,%.pdf,$(Plots))
Data = $(wildcard plots/datas/ILP_VOD/*.csv) $(wildcard plots/datas/heuristic_vod_likeilp/*.csv)
python_version_full = $(wordlist 2,4,$(subst ., ,$(shell python3 --version 2>&1)))
Python3 = python3.$(word 2,${python_version_full})

all: mainACM.pdf

mainACM.pdf: $(MainSource) $(Sections) $(Plots) $(Bib)
	$(Python3) latexrun $<
	#./latexrun --latex-args="-shell-escape" $<

.PHONY: oldMake
oldMake:
	latexmk -pdf -pdflatex="pdflatex --shell-escape -interaction=nonstopmode" -use-make mainACM.pdf

.PHONY: checkWriting
checkWriting:
	$(Python3) checkWriting.py $(MainSource) $(Sections)

.PHONY: clean
clean:
	latexmk -CA
	rm -rf latex.out
	rm -rf *.dpth *.log *.dep *.pdf
