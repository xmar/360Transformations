MainSource = mainACM.tex
MainSourceICC = paper.tex
Sections = $(filter-out $MainSource, $(wildcard *.tex))
Bib = $(wildcard *.bib)
Plots = $(wildcard plots/*.tex)
PlotsPdf = $(patsubst %.tex,%.pdf,$(Plots))
Data = $(wildcard results/*.csv)
python_version_full = $(wordlist 2,4,$(subst ., ,$(shell python3 --version 2>&1)))
Python3 = python3.$(word 2,${python_version_full})

all: mainACM.pdf paper.pdf

mainACM.pdf: $(MainSource) $(Sections) $(Plots) $(Bib) $(Data)
	$(Python3) latexrun --latex-args="-shell-escape" $<
	#./latexrun --latex-args="-shell-escape" $<

paper.pdf: $(MainSourceICC) $(Sections) $(Plots) $(Bib) $(Data)
	$(Python3) latexrun --latex-args="-shell-escape" $<


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
