<template>
    <div class="container text-center">
        <div class="mb-3">
            <button class="btn btn-primary btn-lg default-btn" @click="connectBlueTooth">{{ connect_btn_text }}
            </button>
        </div>
        <div class="wifi-data-form">
            <h5 class="mb-3">Sisesta Wi-Fi andmed</h5>
            <div class="row">
                <div class="col-sm-6 offset-sm-3 position-relative" style="padding:0; margin-bottom:0;">
                    <!-- Adjust the width using columns -->
                    <input :placeholder=ssid_placeholder type="text" id="ssidForm"
                        class="form-control form-control-lg text-center" 
                        v-model="ssid" 
                        :disabled="disconnected"
                        autocomplete="one-time-code"/>
                    <button 
                        class="eye-btn dropdown-toggle position-absolute top-50 end-0 translate-middle-y"
                        type="button" 
                        style="height: 100%;" 
                        aria-expanded="false" 
                        @click="toggleDropdown"
                        :disabled="disconnected">
                    </button>
                    <ul ref="dropdownMenu" class="dropdown-menu">
                        <li v-for="(item, index) in ssid_arr" :key="index" @click="selectSSID(item)"
                            class="dropdown-item">{{ item }}</li>
                    </ul>
                </div>
            </div>
            <div class="mb-4"></div>
            <div class="row">
                <div class="col-sm-6 offset-sm-3 position-relative " style="padding:0">
                    <!-- Adjust the width using columns -->
                    <input :placeholder=password_placeholder :type="passwordVisible ? 'text' : 'password'"
                        id="passwordForm" 
                        class="form-control form-control-lg text-center " 
                        v-model="password"
                        :disabled="disconnected" 
                        autocomplete="one-time-code" />
                    <button @click="togglePasswordVisibility"
                        class="eye-btn position-absolute top-50 end-0 translate-middle-y" style=" height: 100%;"
                        :disabled="disconnected">
                        <i :class="passwordVisible ? 'pi pi-eye-slash' : 'pi pi-eye'"></i>
                    </button>
                </div>
            </div>
            <div>
                <button class="btn btn-primary btn-lg default-btn mt-4" :disabled="isButtonDisabled"
                    @click="sendWiFiCredentials">{{ send_btn_text }}
                </button>
            </div>
        </div>
        <div class="error">
            {{ error_msg }}
        </div>
    </div>

</template>

<script setup>
import { ref, onMounted, computed, onBeforeUnmount } from 'vue';
import { useStore } from 'vuex';

import { useToast } from "vue-toastification";

const serviceUUID = "7f9b95e0-3f72-4f75-92d2-36c70cd39670"
const RX_UUID = "7f9b95e1-3f72-4f75-92d2-36c70cd39670"
const TX_UUID = "7f9b95e2-3f72-4f75-92d2-36c70cd39670"

const toast = useToast();

const ssid_placeholder = ref('Võrgu nimi');
const password_placeholder = ref('Parool');
const disconnected = ref(true);
const error_msg = ref('');
const connect_btn_text = ref('Ühenda seadmega')
const send_btn_text = ref('Saada');
const ssid = ref('');
const password = ref('');
const passwordVisible = ref('')

const disableSend = ref(false);

const ssid_arr = ref([]);
const dropdownMenu = ref(null);

const store = useStore();

let device
let writableCharacteristic;
let readableCharacteristic;
let readSSID = false;

const isButtonDisabled = computed(() => {
    return disconnected.value || disableSend.value;
});

onMounted(() => {
    const savedSSIDs = JSON.parse(localStorage.getItem('ssid_arr'));
    if (savedSSIDs != null) {
        ssid_arr.value = savedSSIDs;
    }
    device = store.state.device;
    writableCharacteristic = store.state.writableCharacteristic;
    readableCharacteristic = store.state.readableCharacteristic;
    if(device) {
        if(device.gatt.connected){
            disconnected.value = false;
            connect_btn_text.value = "Ühenda lahti"
        }
    }
});

const toggleDropdown = () => {
    if (dropdownMenu.value && ssid_arr.value.length !== 0) {
        dropdownMenu.value.style.display = dropdownMenu.value.style.display === 'block' ? 'none' : 'block';
    }
};

const togglePasswordVisibility = () => {
    passwordVisible.value = !passwordVisible.value;
};

const selectSSID = (ssidOption) => {
    ssid.value = ssidOption;
    dropdownMenu.value.style.display = 'none';
}

const setInitialFormValues = () => {
    ssid_placeholder.value = 'Võrgu nimi';
    password_placeholder.value = 'Parool';
    disconnected.value = 'true';
    error_msg.value = '';
    connect_btn_text.value = 'Ühenda seadmega';
    send_btn_text.value = 'Saada';
    disableSend.value = false;
    ssid.value = '';
    password.value = '';
    if (dropdownMenu.value) {
        dropdownMenu.value.style.display = 'none';
    }
}

const requestDevice = async () => {
    device = await navigator.bluetooth.requestDevice({
        // acceptAllDevices:true,
        optionalServices: [serviceUUID],
        filters:
            [{
                service: serviceUUID,
                name: "TaibuTaim"
            }]
    });

    device.addEventListener('gattserverdisconnected', () => {
        if (!device || !device.gatt.connected) {
            device = null;
            console.log("Seade yhendus lahti");
            store.commit('removeDevice');
            setInitialFormValues();
            connect_btn_text.value = 'Ühenda seadmega'
            console.log(connect_btn_text.value);
        }
    });

    store.commit('setDevice', device);
}

const connectDevice = async () => {

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(serviceUUID);

    writableCharacteristic = await service.getCharacteristic(RX_UUID);
    readableCharacteristic = await service.getCharacteristic(TX_UUID);

    if (readableCharacteristic.properties.notify) {
        readableCharacteristic.addEventListener('characteristicvaluechanged', readValue);
        await readableCharacteristic.startNotifications();
        console.log("Started notifications");
    }
    store.commit('setWritableCharacteristic', writableCharacteristic);
    store.commit('setReadableCharacteristic', readableCharacteristic);
    if (device.gatt.connected) return;
}


const connectBlueTooth = async () => {

    if (!navigator.bluetooth) return error_msg.value = "See brauser ei toeta BlueToothi"

    // If already connected, we disconnect
    if (device) {
        device.gatt.disconnect();
        device = null;
        store.commit('removeDevice');
        setInitialFormValues();
        writableCharacteristic = null;
        readableCharacteristic = null;
        return;
    }
    try {
        await requestDevice();
        connect_btn_text.value = "Ühendamine..."
        await connectDevice();
        connect_btn_text.value = "Ühenda lahti"
        toast.info("Bluetooth ühendatud");
        disconnected.value = false;
    } catch (error) {
        console.log("error:", error);
    }
}

const parseString = (event) => {
    let len = event.target.value.byteLength
    let rxStr = '';
    for (let i = 0; i < len; i++) {
        const byte = event.target.value.getUint8(i);
        rxStr += String.fromCharCode(byte);
    }
    return rxStr;
}

const readValue = (event) => {
    let rxStr = parseString(event)
    // console.log(rxStr);
    if (rxStr == "Success") {
        console.log("tra la la la")
        toast.success("WiFi ühendatud");
    }
    else if (rxStr == "Failed") {
        toast.error("Ei suudetud WiFi ühendust luua");
    }
    else if(readSSID && rxStr !== "</SSID_END>"){
        if (!Array.isArray(ssid_arr.value)) {
            ssid_arr.value = [];
        }
        ssid_arr.value.push(rxStr);
        console.log("SSID: ", rxStr);
    }
    else if (rxStr === "<SSID_START>") {
        readSSID = true;
        ssid_arr.value = [];
        localStorage.clear();
    }
    else if(rxStr == "</SSID_END>") {
        readSSID = false;
        console.log(ssid_arr.value);
        localStorage.setItem('ssid_arr', JSON.stringify(ssid_arr.value));
        if (dropdownMenu.value) {
            dropdownMenu.value.style.display = 'block';
        }
    }
}

const sendWiFiCredentials = async () => {
    
    let _ssid = ssid.value;
    let _password = password.value;
    let _ssid_len = _ssid.length;
    let _password_len = _password.length;
    let total_len = _password_len + _ssid_len + 2; // Add 2 for the length prefix bytes

    let encodedData = new Uint8Array(total_len);

    encodedData[0] = _ssid_len;

    // Copy SSID data after length byte
    for (let i = 0; i < _ssid_len; i++) {
        encodedData[i + 1] = _ssid.charCodeAt(i);
    }

    // Encode the length of password as byte after SSID data
    encodedData[_ssid_len + 1] = _password_len;

    // Copy password data after length byte
    for (let j = 0; j < _password_len; j++) {
        encodedData[_ssid_len + 2 + j] = _password.charCodeAt(j);
    }

    console.log(encodedData);

    if (!writableCharacteristic) {
        console.error('No writable characteristic found');
        return;
    }

    let encoder = new TextEncoder();

    try {
        send_btn_text.value = "Saadan...";
        disableSend.value = true;
        await writableCharacteristic.writeValue(encodedData);
        send_btn_text.value = "Saada";
        disableSend.value = false;
    }
    catch (e) {
        console.log(e);
        return;
    }
}

</script>

<style scoped>
.btn-primary {
    background-color: rgba(51, 80, 143, 0.87);
    border: none;

    &:hover {
        background-color: rgba(51, 80, 143, 0.87);
        opacity: 0.8;
    }
}

.btn:disabled {
    background-color: rgba(51, 80, 143, 0.87);
}


.dropdown-menu {
    display: none;
    position: absolute;
    width: 100%;
    /* top: 100%;  */
    max-height: 10rem;
    overflow-y: auto;
}

.dropdown-item {
    text-align: center;

    &:hover {
        cursor: pointer;
        background-color: rgb(210, 239, 243);

    }

    &:active {
        background-color: powderblue;
        color: black;
    }
}

.eye-btn {
    border: none;
    border-radius: 0 0.5rem 0.5rem 0;
    width: 3rem;
    transition: all 0.1s ease;
}

.eye-btn:hover {
    opacity: 0.8;
}

.container {
    font-family: 'Monsterrat';
}

.default-btn {
    width: 15rem;
}

.btn:hover {
    /* box-shadow: 0 0 10px rgba(0, 0, 0, 0.264);  */
    opacity: 0.9;
}

.btn:active {
    opacity: 1;
    /* transform: translate(0,5%); */
    box-shadow: 0 0 0;
}

input:disabled,
.btn:disabled {
    opacity: 0.5;
}
</style>