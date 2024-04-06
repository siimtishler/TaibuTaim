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
                            <router-link class="nav-link" to="/signin">Logi sisse</router-link>
                        </li>
                        <li class="nav-item">
                            <router-link class="nav-link" to="/register">Registreeri</router-link>
                        </li>
                    </ul>
                </div>
            </div>
        </div>
    </nav>
</template>

<script setup>
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
}

.nav-item {
    padding-right: 1.5rem !important;
}
</style>