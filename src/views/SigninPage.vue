<template>
    <div class="container py-5 my-5 h-100">
        <div class="row d-flex justify-content-center align-items-center h-100">
            <div class="col col-xl-10">
                <div class="card" style="border-radius: 1rem;">
                    <div class="row g-0">
                        <div class="col-md-6 col-lg-5 d-none d-md-block side-image">
                        </div>
                        <div class="col-md-6 col-lg-7 d-flex align-items-center">
                            <div class="card-body p-3 p-lg-5 m-lg-2 text-black text-center">
                                <h3 class="mb-3">Logige sisse</h3>
                                <form>
                                    <div class="form-outline mb-4">
                                        <input placeholder="E-mail" type="email" id="emailForm" class="form-control form-control-lg"
                                            v-model="email" />
                                    </div>
                                    <div class="form-outline mb-4 position-relative">
                                        <input 
                                            placeholder="Salasõna" 
                                            :type="passwordVisible ? 'text' : 'password'"
                                            id="passwordForm" 
                                            class="form-control form-control-lg "
                                            v-model="password"
                                        />
                                        <button @click="togglePasswordVisibility" 
                                            class="eye-btn position-absolute end-0 top-50 translate-middle-y"
                                            style="height: 100%;">
                                            <i :class="passwordVisible ? 'pi pi-eye-slash' : 'pi pi-eye'"></i>
                                        </button>
                                    </div>
                                    
                                    
                                    <div class="button-container mb-4">
                                        <button 
                                            class="btn btn-lg btn-block"
                                            type="button" 
                                            style="background-color: #D2DECB;"
                                            @click="signIn">Logi sisse</button>
                                    </div>
                                    <div class="button-container">
                                        <button 
                                            class="btn btn-lg btn-block" 
                                            type="button" 
                                            style="background-color: #DEA49C;"
                                            @click="signInWithGoogle">
                                            <i class="pi pi-google" style="width:8px; height:8px; margin-right: 0.5rem;"></i>
                                            <!-- <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" class="bi bi-google me-2" viewBox="1 0 14 19">
                                                <path d="M15.545 6.558a9.4 9.4 0 0 1 .139 1.626c0 2.434-.87 4.492-2.384 5.885h.002C11.978 15.292 10.158 16 8 16A8 8 0 1 1 8 0a7.7 7.7 0 0 1 5.352 2.082l-2.284 2.284A4.35 4.35 0 0 0 8 3.166c-2.087 0-3.86 1.408-4.492 3.304a4.8 4.8 0 0 0 0 3.063h.003c.635 1.893 2.405 3.301 4.492 3.301 1.078 0 2.004-.276 2.722-.764h-.003a3.7 3.7 0 0 0 1.599-2.431H8v-3.08z"/>
                                            </svg> -->
                                            oogle
                                        </button>
                                    </div>
                                    <div class="d-flex mt-3 pt-3">
                                        <hr class="my-auto flex-grow-1">
                                        <div class="px-4" style="color: #393f81;">Pole kasutajat?</div>
                                        <hr class="my-auto flex-grow-1">
                                    </div>
                                    <div class="button-container mt-4">
                                        <button 
                                            @click="$router.push('/register')" 
                                            class="btn btn-lg btn-block" 
                                            type="button" 
                                            style="background-color: #CADBDE;">Registreeri
                                        </button>
                                    </div>
                                </form>
                            </div>
                        </div>
                    </div> 
                </div>
                <div v-if="errMsg" class="alert alert-danger text-center mt-4" role="alert">{{ errMsg }}</div>
            </div>
        </div>
    </div>

</template>

<script setup>
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { GoogleAuthProvider, getAuth, signInWithEmailAndPassword, signInWithPopup } from 'firebase/auth';
import { useToast } from "vue-toastification";

const toast = useToast();

const router = useRouter();
const email = ref('');
const password = ref('');
const passwordVisible = ref(false);
const errMsg = ref('');


// Methods
const togglePasswordVisibility = () => {
    passwordVisible.value = !passwordVisible.value;
};

const signIn = () => {
    signInWithEmailAndPassword(getAuth(), email.value, password.value)
        .then((data) => {
            console.log("Signed in");
            toast.success("Sisse logitud");
            router.push('/ble');
        })
        .catch((error) => {
            console.log(error.code);
            switch(error.code){
                case "auth/invalid-email":
                    errMsg.value = "Email valesti sisestatud";
                    break;
                case "auth/user-not-found":
                    errMsg.value = "Kasutajat ei leitud";
                    break;
                case "auth/wrong-password":
                    errMsg.value = "Vale parool";
                    break;
                case "auth/missing-password":
                    errMsg.value = "Sisestage parool";
                    break;
                default:
                    errMsg.value = "Sellise Emaili ja parooliga kasutajat ei eksisteeri";
                    break;
            }
        });
}

const signInWithGoogle = () => {
    const provider = new GoogleAuthProvider();
    console.log(provider);
    signInWithPopup(getAuth(), provider)
        .then((result) => {
            console.log(result.user);
            toast.success("Sisse logitud");
            router.push('/ble');
        })
        .catch((error) => {
            errMsg.value = error;
        });
};

</script>

<style scoped>

.container {
    font-family: 'Monsterrat';
}

.alert {
    justify-content: center;
    left: 25%;
    width: 50%;
    border:none;
    color:black;
}

.side-image {
    content: '';
    background-image: url('../assets/login2.jpg');
    background-size: cover;
    background-position: bottom;
    background-repeat: no-repeat;
    border-radius: 1rem 0 0 1rem; /* Rounded corners */
}

.eye-btn {
    border:none;
    border-radius: 0 0.5rem 0.5rem 0;
    width:3rem;
    transition: all 0.1s ease;
}

.eye-btn:hover{
    opacity: 0.8;
}

.button-container {
    display: flex;
    align-items: center;
    justify-content: center; /* Center the buttons horizontally */
}

.btn {
    width: 45%;
    margin-right: 5px; /* Add margin between buttons if needed */
    transition: all 0.05s ease; /* Smooth transition for the box shadow */
    border:none;
}

.btn:hover {
    /* box-shadow: 0 0 10px rgba(0, 0, 0, 0.264);  */
    opacity: 0.9;
}

.btn:active {
    opacity: 1;
    transform: translate(0,5%);
    box-shadow: 0 0 0;
}

</style>