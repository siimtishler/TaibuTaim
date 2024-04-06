<template>
    <div>
        <p>Humidity: {{ humidity.humidity }} Temperature: {{ humidity.temperature }}</p>
        <p>Light: Ambient: {{ light.ambient }} Lux: {{ light.lux }} White: {{ light.white }}</p>
        <p>Soil Moisture: {{ soil }}</p>
    </div>
</template>

<script setup>
import { ref, onMounted, computed, onBeforeUnmount } from 'vue';
import { useStore } from 'vuex';
// Remove the unused import statement
// import Database from '@/utils/Database.js';
// import { firebaseInstance } from '@/main.js';
// console.log(firebaseInstance);

const humidity = ref({});
const light = ref({});
const soil = ref(null);

// const test = Database.getData('humidity');
// console.log(test);

// // Use the Database.js functions here
// humidity.value = Database.getLightData();
// light.value = Database.getHumidityData();


// saveData();

</script>