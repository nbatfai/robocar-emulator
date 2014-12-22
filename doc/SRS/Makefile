all:	clean validate_xmllint_emu validate_xmllint_cld robocaremu-SRS-hu.pdf robocarcld-SRS-hu.pdf
epub:	validate_xmllint_emu validate_xmllint_cld robocaremu-SRS-hu.epub robocarcld-SRS-hu.epub

robocaremu-SRS-hu.pdf: robocaremu-SRS-hu.xml robocaremu-hu.xls
	dblatex robocaremu-SRS-hu.xml -p robocaremu-hu.xls

robocaremu-SRS-hu.epub: robocaremu-SRS-hu.xml
	dbtoepub -o robocaremu-SRS-hu.epub robocaremu-SRS-hu.xml

robocarcld-SRS-hu.pdf: robocarcld-SRS-hu.xml robocaremu-hu.xls
	dblatex robocarcld-SRS-hu.xml -p robocaremu-hu.xls

robocarcld-SRS-hu.epub: robocarcld-SRS-hu.xml
	dbtoepub -o rrobocarcld-SRS-hu.epub robocarcld-SRS-hu.xml

.PHONY: validate_jing_emu
validate_jing_emu:
	@if [ ! -e jing.jar ]; then \
		wget https://jing-trang.googlecode.com/files/jing-20091111.zip; \
		unzip jing-20091111.zip > /dev/null; \
		mv jing-20091111/bin/jing.jar jing.jar; \
		rm -rf jing-20091111*; \
	fi

	xmllint --xinclude robocaremu-SRS-hu.xml --output output.xml
	java -jar jing.jar -c http://docbook.org/xml/5.1CR3/rng/docbookxi.rnc output.xml
	@rm -f output.xml


.PHONY: validate_xmllint_emu
validate_xmllint_emu:
	xmllint --xinclude robocaremu-SRS-hu.xml --output output.xml
	xmllint --relaxng http://docbook.org/xml/5.1CR3/rng/docbookxi.rng output.xml --noout
	rm -f output.xml

.PHONY: validate_jing_cld
validate_jing_cld:
	@if [ ! -e jing.jar ]; then \
		wget https://jing-trang.googlecode.com/files/jing-20091111.zip; \
		unzip jing-20091111.zip > /dev/null; \
		mv jing-20091111/bin/jing.jar jing.jar; \
		rm -rf jing-20091111*; \
	fi

	xmllint --xinclude robocarcld-SRS-hu.xml --output output.xml
	java -jar jing.jar -c http://docbook.org/xml/5.1CR3/rng/docbookxi.rnc output.xml
	@rm -f output.xml


.PHONY: validate_xmllint_cld
validate_xmllint_cld:
	xmllint --xinclude robocarcld-SRS-hu.xml --output output.xml
	xmllint --relaxng http://docbook.org/xml/5.1CR3/rng/docbookxi.rng output.xml --noout
	rm -f output.xml


.PHONY: clean
clean:
	rm -vf robocarcld-SRS-hu.pdf robocarcld-SRS-hu.epub
	rm -vf robocaremu-SRS-hu.pdf robocaremu-SRS-hu.epub

