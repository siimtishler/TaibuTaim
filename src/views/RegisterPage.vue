<template>
    <div class="container py-5 my-5 h-100">
        <div class="row d-flex justify-content-center align-items-center h-100">
            <div class="col col-xl-10">
                <div class="card" style="border-radius: 1rem;">
                    <div class="row g-0">
                        <div class="col-md-6 col-lg-5 d-none d-md-block">
                            <img src="../assets/login2.jpg"
                                alt="login form" class="img-fluid" style="border-radius: 1rem 0 0 1rem;" />
                        </div>
                        <div class="col-md-6 col-lg-7 d-flex align-items-center">
                            <div class="card-body p-4 p-lg-5 m-lg-2 text-black text-center">
                                <form>

                                    <h5 class="h3 pb-3 mb-3 text-center" style="letter-spacing: 1px;">Sign in
                                    </h5>
                                    
                                    <div class="form-outline mb-4">
                                        
                                        <input placeholder="Email" type="email" id="emailForm" class="form-control form-control-lg"
                                            v-model="email" />
                                    </div>
                                    <div class="form-outline mb-4">
                                        <input placeholder="Password" type="password" id="passwordForm" class="form-control form-control-lg "
                                            v-model="password" />
                                    </div>
                                    
                                    <div class="button-container mb-4">
                                        <button class="btn btn-lg btn-block" type="button" style="background-color: #D2DECB;">Login</button>
                                    </div>
                                    <div class="button-container">
                                        <button class="btn btn-lg btn-block" type="button" style="background-color: #DEA49C;">
                                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" class="bi bi-google me-2" viewBox="1 0 14 19">
                                            <path d="M15.545 6.558a9.4 9.4 0 0 1 .139 1.626c0 2.434-.87 4.492-2.384 5.885h.002C11.978 15.292 10.158 16 8 16A8 8 0 1 1 8 0a7.7 7.7 0 0 1 5.352 2.082l-2.284 2.284A4.35 4.35 0 0 0 8 3.166c-2.087 0-3.86 1.408-4.492 3.304a4.8 4.8 0 0 0 0 3.063h.003c.635 1.893 2.405 3.301 4.492 3.301 1.078 0 2.004-.276 2.722-.764h-.003a3.7 3.7 0 0 0 1.599-2.431H8v-3.08z"/>
                                            </svg>
                                            Sign in with Google
                                        </button>
                                    </div>
                                    <div class="d-flex mt-4 pt-3">
                                        <hr class="my-auto flex-grow-1">
                                        <div class="px-4" style="color: #393f81;">Don't have an account?</div>
                                        <hr class="my-auto flex-grow-1">
                                    </div>
                                    <div class="button-container mt-3">
                                        <button class="btn btn-lg btn-block" type="button" style="background-color: #CADBDE;">Register</button>
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

const loginImageRef = ref(null);
const containerRef = ref(null);

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


onMounted(() => {
    adjustLoginImageHeight();
    window.addEventListener('resize', adjustLoginImageHeight);
});

onBeforeUnmount(() => {
    window.removeEventListener('resize', adjustLoginImageHeight);
});


const adjustLoginImageHeight = () => {
    const loginImage = loginImageRef.value;
    if (loginImage) {
        loginImage.style.height = `${containerRef.value.clientHeight}px`;
    }
};

const signInWithGoogle = () => {

};

</script>


<style scoped>

.button-container {
    display: flex;
    align-items: center;
    justify-content: center; /* Center the buttons horizontally */
}

.btn {
    width: 50%;
    margin-right: 5px; /* Add margin between buttons if needed */
}

/* .divider::before{
    content: '-';
    max-width: 100%;
    height: 1px;
}

.divider::after{
    height: 1px;
    content:'-';
    max-width: 100%;
}

.divider{
    display:flex;
    align-items:center;
    flex-wrap: wrap;
} */


</style>