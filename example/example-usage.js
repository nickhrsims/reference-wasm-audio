import { createEmscriptenAudioWorkletNode } from './index.js';

/** Path from which to import module. */
const WASM_MODULE_PATH = "./audio-passthrough.js";

/**
 * Global state data.
 */
const state = {
  wasmModule: null,
  audioContext: null,
  userAudioStream: null,

  clear() {
    this.wasmModule = null;
    this.audioContext = null;
    this.userAudioStream = null;
  }
}

/**
 * Activate 
 */
export async function toggleAudio() {
  if (!state.audioContext) {
    const instantiateWasmModule = (await import(WASM_MODULE_PATH)).default;
    const wasmModule = await instantiateWasmModule();
    const audioContext = new AudioContext();
    const node = await createEmscriptenAudioWorkletNode(audioContext, wasmModule);
    const userAudioStream = await navigator.mediaDevices.getUserMedia({ audio: true });
    const microphone = audioContext.createMediaStreamSource(userAudioStream);
    microphone.connect(node).connect(audioContext.destination);
    state.audioContext = audioContext;
    state.wasmModule = wasmModule;
    state.userAudioStream = userAudioStream;
  } else {
    state.userAudioStream.getTracks().forEach(track => track.stop());
    await state.audioContext.close();
    state.clear();
  }
}
