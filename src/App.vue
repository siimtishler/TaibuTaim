<template>
	<nav-bar v-if="!isLoggedIn"></nav-bar>
	<nav-bar-after-login v-else></nav-bar-after-login>

	<!-- <button @click.prevent="loggedIn = !loggedIn">Log in</button> -->

	<router-view>

	</router-view>
</template>

<script setup>
import NavBar from './components/NavBar.vue';
import NavBarAfterLogin from './components/NavBarAfterLogin.vue';
import { ref, reactive, onMounted } from 'vue';
import { getAuth, onAuthStateChanged } from 'firebase/auth';

const isLoggedIn = ref(false);

let auth;
onMounted(() => {
	auth = getAuth();
	onAuthStateChanged(auth, (user) => {
		if (user) {
			isLoggedIn.value = true;
		} else {
			isLoggedIn.value = false;
		}
	})
})


</script>

<style>
body{
	width:100vw;
	height:100vh;
    background-image: url('assets/bg4.svg'); 
	background-size: cover;
	background-position: bottom;
	background-repeat: no-repeat;
}

</style>
