<template class="all">
    <div class="container outer mb-3">
        <div class="container measurements">
            <div class="single-measurement">
                <img :src="battSvg" style="height:100%;" alt="">
                {{ batteryPercentage }}%
            </div>
            <div class="single-measurement">
                <img :src="tempSvg" style="height:100%;" alt="">
                {{ temperature }}°C
            </div>
            <div class="single-measurement">
                <img :src="soilMoistureSvg" style="height:100%;" alt="">
                {{ soilMoisture }}%
            </div>
            <div class="single-measurement">
                <img :src="humiditySvg" style="height:100%;" alt="">
                {{ humidity }}%
            </div>
            <div class="single-measurement">
                <img :src="lightSvg" style="height:100%;" alt="">
                {{ lux }} lx
            </div>
        </div>
    </div>


    <div class="container outer">
        <div class="container charts">
            <div class="btn-container charts-btns">

                <input type="radio" class="btn-check" name="chartBtnRadio" id="btnradio1" autocomplete="off"
                    value="battery" v-model="selectedData">
                <label class="btn btn-light btn-sm" for="btnradio1">Aku</label>

                <input type="radio" class="btn-check" name="chartBtnRadio" id="btnradio2" autocomplete="off"
                    value="light" v-model="selectedData">
                <label class="btn btn-light btn-sm" for="btnradio2">Valgus</label>

                <input type="radio" class="btn-check" name="chartBtnRadio" id="btnradio3" autocomplete="off"
                    value="temperature" v-model="selectedData">
                <label class="btn btn-light btn-sm" for="btnradio3">Temperatuur</label>

                <input type="radio" class="btn-check" name="chartBtnRadio" id="btnradio4" autocomplete="off"
                    value="humidity" v-model="selectedData">
                <label class="btn btn-light btn-sm" for="btnradio4">Niiskus</label>

                <input type="radio" class="btn-check" name="chartBtnRadio" id="btnradio5" autocomplete="off"
                    value="soil" v-model="selectedData">
                <label class="btn btn-light btn-sm" for="btnradio5">Mulla niiskus</label>

            </div>

            <canvas id="myChart" width="150px" height="50px">
                Your browser does not support the canvas element.
            </canvas>

            <div class="btn-container time-btns d-flex justify-content-center">

                <div v-for="(item, idx) in Object.entries(timeBtns)" :key="idx">
                    <input type="radio" class="btn-check" name="btnradio" :id="`btntime${idx}`" autocomplete="off"
                        :value="item[1]" v-model="selectedTimeHr">
                    <label class="btn btn-light btn-sm" :for="`btntime${idx}`">{{ item[0] }}</label>
                </div>
            </div>
        </div>
    </div>

    <div class="container outer mt-3">
        <div class="container measurements">
            <div class="statistic-container">
                <div class="stat-name">
                    Avg:
                </div>
                <div class="statistic">
                    {{ avgValue }} {{ unit }}
                </div>
            </div>
            <div class="statistic-container">
                <div class="stat-name">
                    3h:
                </div>
                <div class="statistic" :class="{ 'positive': threeHrChange >= 0, 'negative': threeHrChange < 0 }">
                    {{ threeHrChange }} {{ unit }}
                </div>
            </div>
            <div class="statistic-container">
                <div class="stat-name">
                    6h:
                </div>
                <div class="statistic" :class="{ 'positive': sixHrChange >= 0, 'negative': sixHrChange < 0 }">
                    {{ sixHrChange }} {{ unit }}
                </div>
            </div>
            <div class="statistic-container">
                <div class="stat-name">
                    12h:
                </div>
                <div class="statistic" :class="{ 'positive': twelveHrChange >= 0, 'negative': twelveHrChange < 0 }">
                    {{ twelveHrChange }} {{ unit }}
                </div>
            </div>
            <div class="statistic-container">
                <div class="stat-name">
                    24h:
                </div>
                <div class="statistic" :class="{ 'positive': dayChange >= 0, 'negative': dayChange < 0 }">
                    {{ dayChange }} {{ unit }}
                </div>
            </div>
        </div>
    </div>




</template>

<script setup>
import { ref, onMounted, computed, watch } from 'vue';
import { Chart } from 'chart.js/auto';
import Database from '@/utils/Database.js';


const timeBtns = ref({
    '3h': 3,
    '6h': 6,
    '12h': 12,
    '24h': 24,
    '7d': 168,
    '30d': 720
});

let myChart = null;
let timestampData = {};
let ctx = null;

const allData = ref({});

const battSvg = ref('');
const tempSvg = ref(require('../assets/temp-icon.svg'));
const soilMoistureSvg = ref(require('../assets/soil-moisture-icon.svg'));
const humiditySvg = ref(require('../assets/moisture-icon.svg'));
const lightSvg = ref(require('../assets/light-icon.svg'));
    
const batteryPercentage = ref(0);
const temperature = ref(0);
const soilMoisture = ref(0);
const humidity = ref(0);
const lux = ref(0);

const avgValue = ref(0);
const threeHrChange = ref(0);
const sixHrChange = ref(0);
const twelveHrChange = ref(0);
const dayChange = ref(0);
const unit = ref('');

const selectedData = ref('battery');        // default dataset
const selectedTimeHr = ref(24);             // default Time
const selectedIntervalMinutes = ref(10);    // default Interval/frequency

const units = {
    battery: '%',
    light: 'lx',
    lux: 'lx',
    ambient: 'lx',
    white: 'lx',
    temperature: '°C',
    humidity: '%',
    soil: '%',
};


const dataSets = {
    battery: {title: 'Aku', label: '%', borderColor: 'rgb(0, 161, 0)', min: 0, max: 100},
    light: {title: 'Valgus', label: 'Valgus', borderColor: 'rgb(231, 87, 87)' },
    lux: {title: '', label: 'Lux', borderColor: 'rgb(231, 87, 87)' },
    ambient: {title: '', label: 'Hajus', borderColor: 'rgb(255, 205, 86)' },
    white: {title: '', label: 'Valge', borderColor: 'rgb(90, 110, 223)' },
    temperature: {title: 'Temperatuur', label: '°C', borderColor: 'rgb(255, 205, 86)', min: 0, max: 40},
    humidity: {title: 'Niiskus', label: '%', borderColor: 'rgb(54, 162, 235)', min: 0, max: 100},
    soil: {title: 'Mulla niiskus', label: '%', borderColor: 'rgb(54, 162, 235)', min: 0, max: 100},
};

onMounted(async() => {
    ctx = document.getElementById('myChart');

    [allData.value, timestampData] = await getAllData();

    await getDataWhenUpdates();


    // console.log( allData.value.battery[allData.value.battery.length - 1]);
    watch([selectedData, selectedTimeHr], ([newData, newTime]) => {

        updateChart(newData, newTime);
        updateStatistics(newData, newTime);
        
    }, {immediate : true});

});

const getDataWhenUpdates = async () => {
    Database.getDataWhenUpdates('measurements', async () => {
        [allData.value, timestampData] = await getAllData();
        updateChart(selectedData.value, selectedTimeHr.value);
    })
}
const calculateSoilPercentage = (soil) => {
    return Math.floor(100 * (1 - ((soil - 1800) / (3700 - 1800))));
}

const updateStatistics = (newData, newTime) => {
    let filteredData = [];
    let threeHrData = [];
    let sixHrData = [];
    let twelveHrData = [];
    let dayData = [];
    if (newData === 'light') {
        filteredData = getFilteredData(newData, newTime).map(light => light['lux']);
        threeHrData = getFilteredData(newData, 3).map(light => light['lux']);
        sixHrData = getFilteredData(newData, 6).map(light => light['lux']);
        twelveHrData = getFilteredData(newData, 12).map(light => light['lux']);
        dayData = getFilteredData(newData, 24).map(light => light['lux']);
    }
    else if (newData === 'soil') {
        filteredData = getFilteredData(newData, newTime).map(soil => calculateSoilPercentage(soil))
        threeHrData = getFilteredData(newData, 3).map(soil => calculateSoilPercentage(soil));
        sixHrData = getFilteredData(newData, 6).map(soil => calculateSoilPercentage(soil));
        twelveHrData = getFilteredData(newData, 12).map(soil => calculateSoilPercentage(soil));
        dayData = getFilteredData(newData, 24).map(soil => calculateSoilPercentage(soil));
    }
    else {
        filteredData = getFilteredData(newData, newTime);
        threeHrData = getFilteredData(newData, 3);
        sixHrData = getFilteredData(newData, 6);
        twelveHrData = getFilteredData(newData, 12);
        dayData = getFilteredData(newData, 24);
    }

    const avg = filteredData.reduce((acc, curr) => acc + Number(curr), 0) / filteredData.length;    
    avgValue.value = avg.toFixed(1);

    threeHrChange.value = (threeHrData[threeHrData.length - 1] - threeHrData[0]).toFixed(1);
    sixHrChange.value = (sixHrData[sixHrData.length - 1] - sixHrData[0]).toFixed(1);
    twelveHrChange.value = (twelveHrData[twelveHrData.length - 1] - twelveHrData[0]).toFixed(1);
    dayChange.value = (dayData[dayData.length - 1] - dayData[0]).toFixed(1);
    unit.value = units[newData];
}

const updateChart = async (newData, newTime) => {
    if (!myChart) {
        const cfg = {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: '',
                    data: [],
                    fill: true,
                    borderColor: '',
                    tension: 0.5
                }]
            },
        }
        Chart.defaults.font.family = 'Monsterrat';
        // Chart.defaults.font.weight = 'normal';
        Chart.defaults.color = 'Black';
        myChart = new Chart(ctx, cfg);
    }

    if (myChart) {

        myChart.data.datasets = [];
        const filteredData = getFilteredData(newData, newTime);
        const filteredTimestampData = getTimeStampData(newTime);

        if (newData === 'light') {
            ['lux', 'white', 'ambient'].forEach((key, index) => {
                myChart.data.datasets.push({
                    label: dataSets[key].label,
                    data: filteredData.map(light => light[key]),
                    fill: false,
                    borderColor: dataSets[key].borderColor,
                    backgroundColor: dataSets[key].borderColor,
                    tension: 0.2
                });
            });
        }
        else if (newData === 'soil') {
            let soilData = filteredData.map(soil => Math.floor(100 * (1 - ((soil - 1800) / (3700 - 1800)))));
            myChart.data.datasets.push({
                label: dataSets[newData].label,
                data: soilData,
                fill: false,
                borderColor: dataSets[newData].borderColor,
                backgroundColor: dataSets[newData].borderColor,
                tension: 0.2
            });
        }
        else {
            myChart.data.datasets.push({
                label: dataSets[newData].label,
                data: filteredData,
                fill: false,
                borderColor: dataSets[newData].borderColor,
                backgroundColor: dataSets[newData].borderColor,
                tension: 0.2
            });
        }
        // myChart.options.scales.y.max = 100;
        myChart.options.plugins.title.display = true;
        myChart.options.plugins.title.font.size = 25;
        myChart.options.plugins.title.font.weight = 'normal';
        myChart.options.plugins.title.text = dataSets[newData].title;
        myChart.options.plugins.legend.position = 'bottom';
        myChart.data.labels = filteredTimestampData
        myChart.update();
    }
};

const getFilteredData = (_data, _time) => {
    const currentTime = Date.now();
    const pastTime = currentTime - _time * 60 * 60 * 1000; // time 'time' hours ago

    const filtered = allData.value[_data].filter((data, index) => {
        const timestamp = new Date(timestampData[index]).getTime();
        if(_time == 24) {
            return timestamp >= pastTime && index % 2 === 0;
        }
        else if(_time >= 24 && _time <= 168) {
            return timestamp >= pastTime && index % 6 === 0;
        }
        else if(_time > 168) {
            return timestamp >= pastTime && index % 10 === 0;
        }
        else {
            return timestamp >= pastTime;
        }
    });
    return filtered;
}

const getTimeStampData = (_time) => {
    const currentTime = Date.now();
    const pastTime = currentTime - _time * 60 * 60 * 1000; // time 'time' hours ago

    const filtered = timestampData.filter((timestamp, index) => {
        const time = new Date(timestamp).getTime();
        if(_time == 24) {
            // console.log("Time1: ", _time);
            return time >= pastTime && index % 2 === 0;
        }
        else if(_time >= 24 && _time <= 168) {
            // console.log("Time2: ", _time);
            return time >= pastTime && index % 6 === 0;
        }
        else if(_time > 168) {
            // console.log("Time3: ", _time);
            return time >= pastTime && index % 10 === 0;
        }
        else {
            return time >= pastTime;
        }
    });
    if(_time > 24) {
        return filtered.map(timestamp => new Date(timestamp).toLocaleDateString());
    }
    return filtered.map(timestamp => new Date(timestamp).toTimeString().slice(0, 5));
}

const getAllData = async () => {
    const data = await Database.getData('measurements');
    allData.value = {
        battery: Object.values(data).map(measurement => measurement.battery.percentage),
        light: Object.values(data).map(measurement => measurement.light),
        soil: Object.values(data).map(measurement => measurement.soil.moisture),
        humidity: Object.values(data).map(measurement => measurement.humidity.humidity),
        temperature: Object.values(data).map(measurement => measurement.humidity.temperature),
    };

    batteryPercentage.value = Math.floor(allData.value.battery[allData.value.battery.length - 1]);
    temperature.value = Math.floor(allData.value.temperature[allData.value.temperature.length - 1]);
    soilMoisture.value = calculateSoilPercentage(allData.value.soil[allData.value.soil.length - 1]);
    humidity.value = Math.floor(allData.value.humidity[allData.value.humidity.length - 1]);
    lux.value = Math.floor(allData.value.light[allData.value.light.length - 1].lux);
    changeBatterySvg(batteryPercentage.value);

    timestampData = Object.values(data).map(measurement => new Date(measurement.timestamp).toLocaleString());

    return [allData.value, timestampData];
}

const changeBatterySvg = (percentage) => {
    // Sometimes MAX17048 gives more than 100% battery percentage
    if (percentage > 100) {
        batteryPercentage.value = 100;
    }

    if (percentage >= 0 && percentage <= 10) {
        battSvg.value = require('../assets/batteries/batt8.svg');
    }
    else if (percentage > 12.5 && percentage <= 15) {
        battSvg.value = require('../assets/batteries/batt7.svg');
    }
    else if (percentage > 15 && percentage <= 25) {
        battSvg.value = require('../assets/batteries/batt6.svg');
    }
    else if (percentage > 25 && percentage <= 42.5) {
        battSvg.value = require('../assets/batteries/batt5.svg');
    }
    else if (percentage > 42.5 && percentage <= 62.5) {
        battSvg.value = require('../assets/batteries/batt4.svg');
    }
    else if (percentage > 62.5 && percentage <= 75) {
        battSvg.value = require('../assets/batteries/batt3.svg');
    }
    else if (percentage > 75 && percentage <= 87.5) {
        battSvg.value = require('../assets/batteries/batt2.svg');
    }
    else if (percentage > 87.5) {
        battSvg.value = require('../assets/batteries/batt1.svg');
    }
}

</script>

<style scoped>

.charts {
    background-color: rgba(255, 255, 255, 0.788);
    border-radius: 1rem;
    font-family: "Monsterrat";
    display: flex;
    flex-direction: column;
    align-items: center;
}

.outer{
    padding:1rem;
    background-color: rgba(63, 83, 60, 0.836);
    border-radius: 1rem;
}

.measurements {
    font-family: "Monsterrat";
    display: flex;
    align-items: center;
    justify-content: space-between;
}


.single-measurement {
    height: 5rem;
    margin-right: 0.5rem;
    background-color: rgba(255, 255, 255, 0.788);
    color: rgb(65, 62, 62);
    border-radius: 0.5rem;
    padding: 1rem;
    font-size: 1.8rem;
    text-align: center;
}

.statistic-container {
    height: 7rem;
    width: 12rem;
    margin-right: 0.5rem;
    background-color: rgba(255, 255, 255, 0.788);
    color: rgb(65, 62, 62);
    border-radius: 0.5rem;
    padding: 0.3rem;
    font-size: 1.2rem;
    text-align: center;
}

.stat-name {
    /* border:1px solid; */
    display: flex;
    flex-direction: row;
    align-items: flex-start;
    /* width: 30%; */
    font-weight: bold;
}

.statistic {
    font-size: 1.8rem;
    /* border:1px solid; */
    /* font-weight: bold; */
}

.positive {
    color: rgb(0, 161, 0);
}

.negative {
    color: rgb(127, 25, 25);
}


.btn-sm {
    font-size: 0.9rem;
    margin: 0.5rem;
}

input[type="radio"] + label {
    background-color: rgb(250, 250, 250);
    border:none;
}

input[type="radio"]:checked + label {
    /* opacity: 0.8; */
    box-shadow:  0 0 8px rgba(0, 0, 0, 0.26);
    font-weight: bold;
    background-color: rgb(250, 250, 250);
}
</style>