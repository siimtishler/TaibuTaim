<template>
	<nav-bar v-if="!loggedIn"></nav-bar>
	<nav-bar-after-login v-else></nav-bar-after-login>

	<button @click.prevent="loggedIn = !loggedIn">Log in</button>

	<router-view>

	</router-view>
</template>

<script setup>
import NavBar from './components/NavBar.vue';
import NavBarAfterLogin from './components/NavBarAfterLogin.vue';
import { ref, reactive, onBeforeMount } from 'vue';

const loggedIn = ref(false);




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
