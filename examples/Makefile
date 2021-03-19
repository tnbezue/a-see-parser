SUBDIRS = demo calculator calculator_ast

.PHONY: subdirs ${SUBDIRS}

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@

CLEANSUBDIRS = $(SUBDIRS:%=clean-%)

.PHONY: clean ${CLEANSUBDIRS}
clean: ${CLEANSUBDIRS}
${CLEANSUBDIRS}:
	${MAKE} -C $(@:clean-%=%) clean
