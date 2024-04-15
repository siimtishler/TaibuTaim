import { createApp } from 'vue'
import App from './App.vue'
// import { initializeApp } from 'firebase/app';
// import fireBaseAPI from './fireBaseAPI.js';
// import getDatabase from "firebase/database";
import { getFirebase } from "./fireBaseAPI";
import './assets/styles.css'; // GLobal style file
import 'bootstrap/dist/css/bootstrap.css';
import 'primeicons/primeicons.css'
import 'primevue/resources/primevue.min.css'; // Import PrimeVue core CSS
// import 'firebase/auth'
// import 'firebase/database'
import store from './store/store'
import router from './routes.js';
import Toast from "vue-toastification";
import "vue-toastification/dist/index.css";

const { fireBaseApp } = getFirebase();
const app = createApp(App)

app.use(router)
app.use(store);
app.use(Toast, {
    transition: "Vue-Toastification__bounce",
    maxToasts: 3,
    newestOnTop: true,
    position: "top-right",
    timeout: 2500,
    closeOnClick: true,
    pauseOnFocusLoss: false,
    pauseOnHover: false,
    draggable: true,
    draggablePercent: 0.6,
    showCloseButtonOnHover: false,
    hideProgressBar: true,
    closeButton: "button",
    icon: true,
    rtl: false
  });
app.mount('#app');



