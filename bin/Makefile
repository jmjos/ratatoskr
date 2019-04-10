venvdir := virtualenv

all: venv

venv: ${venvdir}/bin/activate
${venvdir}/bin/activate: requirements.txt
	test -d ${venvdir} || python3 -m venv ${venvdir}
	${venvdir}/bin/pip3 install -Ur requirements.txt
	touch ${venvdir}/bin/activate
	touch source_me.sh
	echo "source ./virtualenv/bin/activate" > source_me.sh

clean:
	rm -rf virtualenv
	find -iname "*.pyc" -delete
	
clean-sim:
	rm -rf sim[0-9][0-9]
	rm -rf sim[0-9]

.PHONY: all venv

