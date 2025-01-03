OUT_DIR         := dist
EXAMPLE_OUT_DIR := example.dist

SRC  := src/audio_gain.cpp
OUT  := ${OUT_DIR}/audio-passthrough.js

LIB_IN  := src/index.js
LIB_OUT := ${OUT_DIR}/index.js

.PHONY: clean example

all: ${OUT_DIR}/ ${OUT}

## --- C++ Adapter
${OUT}: ${SRC} ${LIB_OUT}
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
${LIB_OUT}: ${LIB_IN}
	@cp src/$$(basename $@) ${OUT_DIR}/$$(basename $@)

## --- Dist Dir
${OUT_DIR}/:
	@mkdir ${OUT_DIR}

## --- Auxiliary
clean:
	@if [ -d ${OUT_DIR} ]; then rm -r ${OUT_DIR}; fi
	@if [ -d ${EXAMPLE_OUT_DIR} ]; then rm -r ${EXAMPLE_OUT_DIR}; fi

example: ${OUT}
	@if [ ! -d ${EXAMPLE_OUT_DIR} ]; then mkdir ${EXAMPLE_OUT_DIR}; fi
	@cp dist/* ${EXAMPLE_OUT_DIR}
	@cp example/* ${EXAMPLE_OUT_DIR}
	@python3 -m http.server -d ${EXAMPLE_OUT_DIR} 8000

