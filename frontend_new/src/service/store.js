import { reactive } from 'vue'
import {getSettings, setSettings, getBattery, getSensors, getStatus} from "@/service/apiService";

export const store = {
  state: reactive({
    isAuthenticated: !!localStorage.getItem('user-token'),
    services: {},
    leds: [],
    schedule: [],
    schedule_config: {
      rgb: false,
      use_sync: false,
      sync_master: false,
      sync_group: 0,
      gamma: 0,
      mode: 0,
      simple_mode_duration: 0,

    },
    status: {},
    time: {},
    cooling: {
      installed: true,
      start_temp: 45,
      target_temp: 50,
      max_temp: 70,
      tachometer: 0,
      pid_kp: 10,   /* proportional gain x100 value */
      pid_ki: 10,   /* integral gain x100 value */
      pid_kd: 10,   /* derivative gain x100 value */
      pid_max: 10,  /* maximum value of manipulated variable */
      pid_min: 10   /* minimum value of manipulated variable */
    },
    gpio: {},
    auth: {},
    errors: []
  }),

  setAuthenticated (token) {
    /* store the token in localstorage */
    localStorage.setItem('user-token', token)
    store.state.isAuthenticated = true
  },

  logout () {
    store.state.isAuthenticated = false
    localStorage.removeItem('user-token')
  },

  async loadSettings () {
    try {
      const response = await getSettings()
      if (response.status === 200) {
        Object.assign(this.state, {
          leds : response.data.leds,
          networks : response.data.networks,
          services : response.data.services,
          thingsboard : response.data.thingsboard,
          schedule_config : response.data.schedule_config,
          time : response.data.time,
          cooling : response.data.cooling,
          gpio : response.data.gpio,
          auth : response.data.auth
        })
      }
    } catch (e) {
      this.state.errors.push("Failed to load Settings");
      throw new Error("Failed to load Settings");
    }
  },

  async saveSettings(entity) {
    try {
      let message = {};

      if (entity === undefined) {
        message = {
          services: this.state.services,
          time: this.state.time,
        };
      } else {
        if (Array.isArray(entity)) {
          entity.forEach(i => {
            Object.keys(this.state).forEach(key => {
              if (key === i) {
                message[key] = this.state[key];
              }
            });
          });
        } else {
          Object.keys(this.state).forEach(key => {
            if (key === entity) {
              message[key] = this.state[key];
            }
          });
        }
      }

      const response = await setSettings(message)
      if (response.status !== 200) {
        this.state.errors.push("Failed to save Settings");
        return false
      } else {
        return true;
      }
    } catch (e) {
      this.state.errors.push("Failed to load Status");
      throw new Error("Failed to load Status");
    }
  },

  async loadStatus () {
    try {
      const response = await getStatus()
      if (response.status === 200) {
        Object.assign(this.state, {
          status : response.data.status
        })
      }
    } catch (e) {
      this.state.errors.push("Failed to load Status");
      throw new Error("Failed to load Status");
    }
  },

  async loadSensors () {
    try {
      const response = await getSensors()
      if (response.status === 200) {
        this.state.sensors = [...response.data.sensors]
      }
    } catch (e) {
      this.state.errors.push("Failed to load Sensors");
      throw new Error("Failed to load Status");
    }
  },

  async loadBattery () {
    try {
      const response = await getBattery()
      if (response.status === 200) {
        Object.assign(this.state, {
          battery : response.data.battery
        });
      }
    } catch (e) {
      this.state.errors.push("Failed to load Battery");
      throw new Error("Failed to load Battery");
    }
  },

  setSensors (payload) {
    this.state.sensors = JSON.parse(JSON.stringify(payload))
  },

  setSensorsValues (payload) {
    const sensors = JSON.parse(JSON.stringify(payload));

    if (this.state.sensors.length) {
      this.state.sensors.forEach((v, index) => {
          v.value = parseFloat(sensors[index].value);
        })
    } else {
      this.state.sensors = [...sensors];
    }
  },

  setBattery (payload) {
    this.state.battery = JSON.parse(JSON.stringify(payload))
  },

  setTime (payload) {
    this.state.time = JSON.parse(JSON.stringify(payload))
  },

  clearErrors() {
    this.state.errors = [];
  }
}
