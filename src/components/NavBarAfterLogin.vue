<template>
    <nav class="navbar navbar-expand-lg">
        <div class="container-fluid">
            <div class="row align-items-center justify-content-center w-100">
                <div class="col-auto d-flex align-items-center">
                    
                    <ul class="navbar-nav">
                        <li class="nav-item">
                            <img 
                            class="logo-img mr-2" 
                            src="../assets/site-logo.png" 
                            alt=""
                            @click="$router.push('/')
                        ">
                            <router-link class="navbar-brand mb-0" to="/">Taibu Taim</router-link>
                        </li>
                        
                        
                        <li class="nav-item">
                            <router-link class="nav-link" to="/">Kodu</router-link>
                        </li>
                        <li class="nav-item">
                            <router-link class="nav-link" to="/feed">Feed</router-link>
                        </li>
                        <li class="nav-item">
                            <router-link class="nav-link" to="/ble">BlueTooth connect</router-link>
                        </li>
                        <li class="nav-item">
                            <p
                                class="nav-link"
                                @click="handleSignOut">
                                Log Out
                            </p>
                        </li>
                        <!-- <li class="nav-item">
                            <i class="nav-link pi pi-sign-out logout-icon"></i>
                        </li> -->
                    </ul>
                </div>
            </div>
        </div>
    </nav>
</template>

<script setup>
import { getAuth, signOut } from 'firebase/auth';
import { useStore } from 'vuex';
import router from '@/routes';

const store = useStore();

const handleSignOut = () => {
    let auth = getAuth();
    let device = store.state.device;
    if(device) {
        if(device.gatt.connected) {
            device.gatt.disconnect();
            device = null;
            store.commit('removeDevice');
        }
    }
    signOut(auth).then(() =>{
        router.push("/");
    });
}


</script>

<style scoped>
.logo-img {
    width: 2.3rem;
    height: 2.3rem;
}

.logo-img:hover{
    cursor: pointer;
}

.navbar-brand {
    font-size: 1.5rem;
}

.navbar {
    font-family: 'Monsterrat';
    font-size: 1.2rem;
}

.nav-link{
    justify-content: center;
    text-align: center;
    &:hover {
        cursor:pointer;
        text-decoration: underline;
    }
}

.nav-item {
    padding-right: 1.5rem !important;
    &:hover {
        cursor:pointer;
    }
}

.logout-icon {
    text-align: center;
    justify-content: center;
}
</style>