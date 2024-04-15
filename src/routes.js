import { createRouter, createWebHashHistory } from 'vue-router';
import HomePage from './views/HomePage.vue';
import SigninPage from './views/SigninPage.vue';
import RegisterPage from './views/RegisterPage.vue';
import FeedPage from './views/FeedPage.vue';
import ErrorPage from './views/ErrorPage.vue';
import BluetoothPage from './views/BluetoothPage.vue';
import PlantDetails from './views/PlantDetails.vue';

import { getAuth, onAuthStateChanged } from 'firebase/auth';

const router = createRouter({
    history: createWebHashHistory(),
    routes: [
        {
            path: '',
            component: HomePage,
        },
        {
            path: '/register',
            component: RegisterPage,
        },
        {
            path: '/feed',
            component: FeedPage,
            meta: {
                requiresAuth: true,
            }
        },
        {
            path: '/ble',
            component: BluetoothPage,
            meta: {
                requiresAuth: true,
            }
        },
        {
            path: '/details',
            component: PlantDetails,
            meta: {
                requiresAuth: true,
            }
        },
        {
            path: '/signin',
            component: SigninPage,
        },
        {
            path: '/:catchAll(.*)',
            component: ErrorPage,
        }
        
    ]
});

const getCurrentUser = () => {
    return new Promise((resolve, reject) => {
        const removeListener = onAuthStateChanged(
            getAuth(),
            (user) => {
                removeListener();
                resolve(user);
            },
            reject
        )
    });
};

router.beforeEach(async (to, from, next) => {
    if(to.matched.some((record) => record.meta.requiresAuth)){
        if(await getCurrentUser()) {
            next();
        } else {
            alert("You do not have access!");
            next("/");
        }
    } else {
        next();
    }
});

export default router;