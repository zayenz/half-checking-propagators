.PHONY: FORCE warn clean

paper.pdf: FORCE
	latexrun half-checking.tex

warn:
	latexrun -Wall half-checking.tex

clean:
	latexrun --clean-all
