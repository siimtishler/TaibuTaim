import {createRouter, createWebHashHistory} from 'vue-router';
import HomePage from './views/HomePage.vue';
import RegisterPage from './views/RegisterPage.vue';
import SigninPage from './views/SigninPage.vue';
import FeedPage from './views/FeedPage.vue';
import ErrorPage from './views/ErrorPage.vue';

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

export default router;