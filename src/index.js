/**
 * Emscripten audioWorkletNode asynchronous factory.
 */
export function createEmscriptenAudioWorkletNode(audioContext, wasmModule) {
  return new Promise((resolve, reject) => {
    const audioContextHandle = wasmModule.emscriptenRegisterAudioObject(audioContext);
    var audioWorkletNode = null;
    let getNode = wasmModule.addFunction(async (audioWorkletNodeHandle) => {
      audioWorkletNode = wasmModule.emscriptenGetAudioObject(audioWorkletNodeHandle);
      if (audioWorkletNode) {
        resolve(audioWorkletNode);
      }
      else {
        reject("audioWorkletNode returned null.");
      }
    }, 'vi');
    wasmModule._create_node(audioContextHandle, audioContext.sampleRate, getNode);
  });
}

/**
 *
 */
export function setParam(node, context, key, value) {
  node.parameters.get(key).setValueAtTime(value, context.currentTime);
}
