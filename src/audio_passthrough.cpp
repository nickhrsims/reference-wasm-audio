/**
 * helpful source:
 * <https://github.com/tomduncalf/emscripten-audio-worklet-example/blob/main/c%2B%2B/oscillator.cpp>
 */

#include <emscripten/webaudio.h>

// -------------------------------------------------------------
// Types, Constants, and Module Data
// -------------------------------------------------------------

/** JavaScript function handle (callback). */
typedef void (*js_cb_t)(int data);

/** AudioWorkletProcessor registration name. */
const char *AWP_NAME = "my-reference-processor";

/** Amount of stack space allocated to audio worklet thread. */
const int AUDIO_THREAD_STACKSIZE = 4096;

uint8_t audio_thread_stack[AUDIO_THREAD_STACKSIZE];

// -------------------------------------------------------------
// Implementation Prototypes
// -------------------------------------------------------------

extern "C" {
void create_node(EMSCRIPTEN_WEBAUDIO_T audio_context, uint32_t sample_rate,
                 js_cb_t handle_node);
}

void _create_node_step_1(EMSCRIPTEN_WEBAUDIO_T audio_context, EM_BOOL success,
                         void *handle_node);

void _create_node_step_2(EMSCRIPTEN_WEBAUDIO_T audio_context, EM_BOOL success,
                         void *handle_node);

EM_BOOL process(int input_count, const AudioSampleFrame *inputs,
                int output_count, AudioSampleFrame *outputs, int param_count,
                const AudioParamFrame *params, void *data);

// -------------------------------------------------------------
// Public API (JS-Exported Functions)
// -------------------------------------------------------------

extern "C" {

/**
 * Create AudioWorkletNode "Step 0".
 *
 * Starts a new audio worklet thread and calls step 1 handler function from
 * inside it.
 */
EMSCRIPTEN_KEEPALIVE void create_node(EMSCRIPTEN_WEBAUDIO_T audio_context,
                                      uint32_t sample_rate,
                                      js_cb_t handle_node) {
  emscripten_start_wasm_audio_worklet_thread_async(
      audio_context, audio_thread_stack, AUDIO_THREAD_STACKSIZE,
      _create_node_step_1, (void *)handle_node);
}
} // extern "C"

// -------------------------------------------------------------
// Node Construction Stages
// -------------------------------------------------------------

/**
 * Create and register AudioWorkletProcessor in dedicated audio thread.
 */
void _create_node_step_1(EMSCRIPTEN_WEBAUDIO_T audio_context, EM_BOOL success,
                         void *handle_node) {

  if (!success) {
    return;
  }

  WebAudioParamDescriptor paramDescriptors[] = {};

  WebAudioWorkletProcessorCreateOptions options{.name = AWP_NAME,
                                                .numAudioParams = 0,
                                                .audioParamDescriptors =
                                                    paramDescriptors};

  emscripten_create_wasm_audio_worklet_processor_async(
      audio_context, &options, _create_node_step_2, handle_node);
}

/**
 * Create AudioWorkletNode and invoke with handler callback.
 */
void _create_node_step_2(EMSCRIPTEN_WEBAUDIO_T audio_context, EM_BOOL success,
                         void *handle_node) {

  if (!success) {
    return;
  }

  int output_channel_counts[] = {1};
  EmscriptenAudioWorkletNodeCreateOptions options{
      .numberOfInputs = 1,
      .numberOfOutputs = 1,
      .outputChannelCounts = output_channel_counts,
  };

  EMSCRIPTEN_AUDIO_WORKLET_NODE_T awn =
      emscripten_create_wasm_audio_worklet_node(audio_context, AWP_NAME,
                                                &options, process, handle_node);

  ((js_cb_t)(handle_node))(awn);
}

/**
 * Process audio block.
 */
EM_BOOL process(int input_count, const AudioSampleFrame *inputs,
                int output_count, AudioSampleFrame *outputs, int param_count,
                const AudioParamFrame *params, void *data) {
  for (int index = 0; index < outputs[0].samplesPerChannel; ++index) {
    outputs[0].data[index] = inputs[0].data[index];
  }
  return true;
}
