import { createEmscriptenAudioWorkletNode, setParam } from './index.js';

/** Path from which to import module. */
const WASM_MODULE_PATH = "./audio-passthrough.js";

/**
 * Global state data.
 */
const state = {
  audioNode: null,
  wasmModule: null,
  audioContext: null,
  userAudioStream: null,

  clear() {
    this.audioNode = null;
    this.wasmModule = null;
    this.audioContext = null;
    this.userAudioStream = null;
  }
}

/**
 * Activate 
 */
export async function toggleAudio() {

  const isProcessingOff = (state.audioContext == null);

  if (isProcessingOff) {
    const instantiateWasmModule = (await import(WASM_MODULE_PATH)).default;
    const wasmModule = await instantiateWasmModule();
    const audioContext = new AudioContext();
    const audioNode = await createEmscriptenAudioWorkletNode(audioContext, wasmModule);
    const userAudioStream = await navigator.mediaDevices.getUserMedia({ audio: true });
    const microphone = audioContext.createMediaStreamSource(userAudioStream);
    microphone.connect(audioNode).connect(audioContext.destination);
    state.audioNode = audioNode;
    state.wasmModule = wasmModule;
    state.audioContext = audioContext;
    state.userAudioStream = userAudioStream;
  } else {
    state.userAudioStream.getTracks().forEach(track => track.stop());
    await state.audioContext.close();
    state.clear();
  }

  return;
}

export const setGain = (value) => {
  setParam(state.audioNode, state.audioContext, 0, value);
}
