venvdir := virtualenv

all: venv test_results.txt

venv: ${venvdir}/bin/activate
${venvdir}/bin/activate: requirements.txt
	test -d ${venvdir} || python3 -m venv ${venvdir}
	${venvdir}/bin/pip install -Ur requirements.txt
	touch ${venvdir}/bin/activate
	touch source_me.sh
	echo "source ./virtualenv/bin/activate" > source_me.sh


test_results.txt: interconnect interconnect/* test test/*
	. ${venvdir}/bin/activate
	nosetests -s > $@  2>&1

.PHONY: all venv
