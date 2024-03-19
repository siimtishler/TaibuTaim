<template>

    <page class="page">

        <div class="card-container d-flex flex-wrap justify-content-between">
            <div class="card">
                <img class="card-img-top" src="../assets/site-logo.png" alt="Card image cap">
                <div class="card-body">
                    <p class="card-text">
                        Asetage toote terav ots mulla sisse
                    </p>
                </div>
            </div>

            <div class="card">
                <img class="card-img-top" src="../assets/site-logo.png" alt="Card image cap">
                <div class="card-body">
                    <p class="card-text">
                        Registreerige kasutajaks<br>Logige sisse
                    </p>
                </div>
            </div>


        </div>


        <b-card title="Card Title"
            img-src="https://demos.creative-tim.com/argon-dashboard-pro-bs4/assets/img/theme/img-1-1000x600.jpg"
            img-alt="Image" img-top tag="article" style="max-width: 20rem;" class="mb-2">
            <b-card-text>
                This is a wider card with supporting text below as a natural lead-in to additional content. This content
                is a little bit longer.
            </b-card-text>

            <small class="text-muted">Last updated 3 mins ago</small>
        </b-card>
    </page>



</template>

<script setup>

</script>

<style scoped>
.page {
    font-family: 'Monsterrat';
    font-size: 1rem;
}

.card {
    width: 12rem;
    align-items: center;
    margin: 0.5rem;
    flex: 0 0 calc(20% - 1rem);
    /* Adjust the width as needed */
    border-radius: 1rem;
}

.card-img-top {
    margin-top: 0.3rem;
    width: 4rem;
}

@media (max-width: 768px) {
    .card {
        flex: 0 0 calc(100% - 1rem);
        /* Change to full width for smaller screens */
    }
}
</style>

<!-- <div class="card">
    <img class="card-img-top" src="../assets/site-logo.png" alt="Card image cap">
    <div class="card-body">
        <p class="card-text">
            Ühendage seade BlueToothi veebilehel
        </p>
    </div>
</div>

<div class="card">
    <img class="card-img-top" src="../assets/site-logo.png" alt="Card image cap">
    <div class="card-body">
        <p class="card-text">
            Andke seadmele kodu WiFi nimi ja parool
        </p>
    </div>
</div>

<div class="card">
    <img class="card-img-top" src="../assets/site-logo.png" alt="Card image cap">
    <div class="card-body">
        <p class="card-text">
            Seade jälgima teie taime tervist
        </p>
    </div>
</div> -->