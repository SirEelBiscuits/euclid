all: extern engine

package: all
	@mkdir -p pkg
	@cp engine/bin/* pkg
	@cp content/* pkg

extern:
	make -C extern

engine:
	make -C engine

clean:
	make -C extern clean
	make -C engine clean
	rm -r pkg

.PHONY: all package extern engine clean
