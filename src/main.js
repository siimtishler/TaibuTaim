import { createApp } from 'vue'
import App from './App.vue'
import { initializeApp } from 'firebase/app';
import fireBaseAPI from './fireBaseAPI.js';
import 'firebase/auth'
import 'firebase/database'
import '../node_modules/bootstrap/dist/css/bootstrap.css';
import router from './routes.js';

const fbApp = initializeApp(fireBaseAPI)

const app = createApp(App)

app.use(router)

app.mount('#app');
