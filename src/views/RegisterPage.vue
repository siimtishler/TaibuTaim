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
                                <h3 class="mb-3">Loo kasutaja</h3>
                                <form>
                                    <div class="form-outline mb-4">
                                        <input 
                                            placeholder="E-mail" 
                                            type="email" 
                                            id="emailForm" 
                                            class="form-control form-control-lg"
                                            v-model="email" 
                                        />
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

                                    <div class="form-outline mb-4">
                                        <input 
                                            placeholder="Korda salasõna" 
                                            :type="passwordVisible ? 'text' : 'password'"
                                            id="confirmPasswordForm" 
                                            class="form-control form-control-lg "
                                            v-model="confirmPassword" 
                                        />
                                    </div>
                                    
                                    <div class="button-container mb-4">
                                        <button 
                                            class="btn btn-lg btn-block" 
                                            type="button" style="background-color: #CADBDE;">
                                            Registreeri
                                        </button>
                                    </div>
           
                                    <div class="d-flex mt-3 pt-2">
                                        <hr class="my-auto flex-grow-1">
                                        <div class="px-4" style="color: #393f81;">Kasutaja juba olemas?</div>
                                        <hr class="my-auto flex-grow-1">
                                    </div>
                                    <div class="button-container mt-4">
                                        <button 
                                            @click="$router.push('/signin')" 
                                            class="btn btn-lg btn-block" 
                                            type="button" 
                                            style="background-color:#D2DECB ;">Logi sisse
                                        </button>
                                    </div>
                                </form>
                            </div>
                        </div>
                    </div> 
                </div>
            </div>
        </div>
    </div>

</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue';
import { useRouter } from 'vue-router';
import { getAuth, createUserWithEmailAndPassword } from 'firebase/auth';

const router = useRouter();
const email = ref('');
const password = ref('');
const confirmPassword = ref('')

const passwordVisible = ref(false);

// Methods
const togglePasswordVisibility = () => {
    passwordVisible.value = !passwordVisible.value;
};

const register = () => {
    createUserWithEmailAndPassword(getAuth(), email.value, password.value)
        .then((data) => {
            console.log("Registered");
            router.push('/feed');
        })
        .catch((error) => {
            console.log(error.code);
            alert(error.message);
        });
};

</script>


<style scoped>

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

.btn {
    width: 45%;
    margin-right: 5px; 
    transition: all 0.05s ease; 
    border:none;
}

.btn:hover {
    opacity: 0.9;
}

.btn:active {
    opacity: 1;
    transform: translate(0,5%);
    box-shadow: 0 0 0;
}

</style>