import { getDatabase, ref, get, set, update, onValue } from "firebase/database";
import { getFirebase } from "../fireBaseAPI";

const db = getFirebase().database;

class Database {
    static greet() {
        console.log(db);
    }
    // Static function to get data from the database
    static async getData(path) {
        console.log("Getting data");
        try {
            const statusref = ref(db, path);
            const snapshot = await get(statusref);
            return snapshot.val();
        } catch (error) {
            console.error('Error getting data:', error);
        }
    }

    static async getDataWhenUpdates(path, callback) {
        const dataRef = ref(db, path);
        onValue(dataRef, (snapshot) => {
            console.log("Getting new data");
            const data = snapshot.val();
            if (data) {
                callback();
            } else {
                console.log('No data available');
            }
        });
    }

    // Static function to set data in the database
    static async setData(path, data) {
        try {
            await set(ref(db, path), data);
            console.log('Data set successfully');
        } catch (error) {
            console.error('Error setting data:', error);
        }
    }

    // Static function to update data in the database
    static async updateData(path, data) {
        try {
            await update(ref(db, path), data);
            console.log('Data updated successfully');
        } catch (error) {
            console.error('Error updating data:', error);
        }
    }

    // Static function to remove data from the database
    static async removeData(path) {
        try {
            await set(ref(db, path), null);
            console.log('Data removed successfully');
        } catch (error) {
            console.error('Error removing data:', error);
        }
    }

    // Static function to get humidity data
    static async getHumidityData() {
        try {
            const data = await this.getData('measurements/humidity');
            return data;
        } catch (error) {
            console.error('Error getting humidity data:', error);
        }
    }
    
    // Static function to get light data
    static async getLightData() {
        try {
            const data = await this.getData('measurements/light');
            return data;
        } catch (error) {
            console.error('Error getting light data:', error);
        }
    }
    
    // Static function to get soil moisture data
    static async getSoilMoistureData() {
        try {
            const data = await this.getData('measurements/soil_moisture');
            return data;
        } catch (error) {
            console.error('Error getting soil moisture data:', error);
        }
    }

    // Static function to get soil moisture data
    static async getBatteryData() {
        try {
            const data = await this.getData('measurements/battery');
            return data;
        } catch (error) {
            console.error('Error getting soil battery data:', error);
        }
    }
}

export default Database;
