all:	clean validate_xmllint robocaremu-SRS-hu.pdf
epub:	clean validate_xmllint robocaremu-SRS-hu.epub

robocaremu-SRS-hu.pdf: robocaremu-SRS-hu.xml robocaremu-hu.xls
	dblatex robocaremu-SRS-hu.xml -p robocaremu-hu.xls

robocaremu-SRS-hu.epub: robocaremu-SRS-hu.xml
	dbtoepub -o robocaremu-SRS-hu.epub robocaremu-SRS-hu.xml


.PHONY: validate_jing
validate_jing:
	@if [ ! -e jing.jar ]; then \
		wget https://jing-trang.googlecode.com/files/jing-20091111.zip; \
		unzip jing-20091111.zip > /dev/null; \
		mv jing-20091111/bin/jing.jar jing.jar; \
		rm -rf jing-20091111*; \
	fi

	xmllint --xinclude robocaremu-SRS-hu.xml --output output.xml
	java -jar jing.jar -c http://docbook.org/xml/5.1CR3/rng/docbookxi.rnc output.xml
	@rm -f output.xml


.PHONY: validate_xmllint
validate_xmllint:
	xmllint --xinclude robocaremu-SRS-hu.xml --output output.xml
	xmllint --relaxng http://docbook.org/xml/5.1CR3/rng/docbookxi.rng output.xml --noout
	rm -f output.xml


.PHONY: clean
clean:
	rm -vf robocaremu-SRS-hu.pdf robocaremu-SRS-hu.epub

