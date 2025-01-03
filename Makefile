OUTDIR := dist

SRC  := src/audio_passthrough.cpp
OUT  := ${OUTDIR}/audio-passthrough.js

LIBIN  := src/index.js
LIBOUT := ${OUTDIR}/index.js

all: ${OUTDIR}/ ${OUT}

## --- C++ Adapter
${OUT}: ${SRC} ${LIBOUT}
	@emcc                           \
		-s INITIAL_MEMORY=65536       \
		-s STACK_SIZE=32768           \
		-s ALLOW_TABLE_GROWTH         \
		-s EXPORTED_RUNTIME_METHODS=['addFunction','emscriptenRegisterAudioObject','emscriptenGetAudioObject'] \
		-s EXPORT_ES6                 \
		-s AUDIO_WORKLET              \
	  -s WASM_WORKERS               \
		-s MODULARIZE                 \
		-o ${OUT}                     \
	  ${SRC}

## --- JS Libs
${LIBOUT}: ${LIBIN}
	@cp src/$$(basename $@) ${OUTDIR}/$$(basename $@)

## --- Dist Dir
${OUTDIR}/:
	@mkdir ${OUTDIR}

## --- Auxiliary
clean:
	@if [ -d ${OUTDIR} ]; then rm -r ${OUTDIR}; fi


