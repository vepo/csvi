SUBDIRS = \
          src/lib \
          src/bin \
          tests

coverage-gcov:
	find . -name '*.c'	| xargs gcov
	@mkdir -p coverage
	mv *.gcov coverage/
