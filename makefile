all: extern engine

package: all
	@mkdir -p pkg
	@cp engine/bin/* pkg
	@cp content/* pkg

extern:
	make -C extern

engine:
	make -C engine

.PHONY: all package extern engine
