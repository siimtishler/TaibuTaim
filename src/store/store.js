import { createStore } from 'vuex';

export default createStore({
    state: {
        device: null,
        writableCharacteristic: null,
        readableCharacteristic: null
    },
    mutations: {
        setDevice(state,device) {
            state.device = device;
        },
        setWritableCharacteristic(state, writableCharacteristic) {
            state.writableCharacteristic = writableCharacteristic;
        },
        setReadableCharacteristic(state, readableCharacteristic) {
            state.readableCharacteristic = readableCharacteristic;
        },
        removeDevice(state) {
            state.device = null;
            state.readableCharacteristic = null;
            state.writableCharacteristic = null;
        }
    }
});