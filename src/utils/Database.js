import { getDatabase, ref, get, set, update } from "firebase/database";
import { firebaseInstance } from "@/main.js";

const db = getDatabase(firebaseInstance);

class Database {

    static greet() {
        console.log('Hello from the Database class');
    }
    // Static function to get data from the database
    static async getData(path) {
        try {
            const statusref = ref(db, path);
            const snapshot = await get(statusref);
            return snapshot.val();
        } catch (error) {
            console.error('Error getting data:', error);
        }
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
            const data = await Database.getData('humidity');
            return data;
        } catch (error) {
            console.error('Error getting humidity data:', error);
        }
    }
    
    // Static function to get light data
    static async getLightData() {
        try {
            const data = await Database.getData('light');
            return data;
        } catch (error) {
            console.error('Error getting light data:', error);
        }
    }
    
    // Static function to get soil moisture data
    static async getSoilMoistureData() {
        try {
            const data = await Database.getData('soil_moisture');
            return data;
        } catch (error) {
            console.error('Error getting soil moisture data:', error);
        }
    }
}

export default Database;
