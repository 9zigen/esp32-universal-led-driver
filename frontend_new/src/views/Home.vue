<template>
  <div class="container m-top-touch-navbar">
    <!-- Page Title -->
    <div class="tile is-ancestor is-marginless">
      <div class="tile is-parent">
        <div class="tile is-child text-left">
          <h1 class="title">
            Dashboard
          </h1>
        </div>
      </div>
    </div>

    <div class="tile is-ancestor is-marginless">
      <!-- Wrapper -->
      <div class="tile is-vertical">

        <div class="tile">
          <!-- left col -->
          <div class="tile is-vertical">
            <!-- Schedule chart -->
            <div class="tile is-vertical is-parent">
              <article class="tile is-child box">
                <div class="content">
                  <p class="subtitle is-4">
                    Light Schedule
                  </p>
                  <div class="content">
                    <schedule-chart />
                  </div>
                </div>
              </article>

              <article class="tile is-child">
                <article class="m-t-12">
                  <div class="buttons has-addons">
                    <span class="button is-light">{{ scheduleStatus }}</span>
                    <span
                      class="button"
                      @click="toggleSchedule()"
                    ><component v-bind:is="scheduleActionIcon()" /></span>
                  </div>
                </article>
              </article>
            </div>
          </div>
          <!-- Right col -->
          <div class="tile is-vertical">
            <!-- Light toggle -->
            <div class="tile is-parent">
              <article class="tile is-child box has-background-warning">
                <p class="subtitle is-4">
                  Light Channel control
                </p>
                <div class="columns is-vcentered">
                  <div class="column is-12">
                    <div
                      v-for="(led, index) in state.status.channels"
                      v-show="active[index]"
                      v-bind:key="index"
                      class="field is-horizontal"
                    >
                      <div class="field-body">
                        <div class="field">
                          <div class="control" v-if="state.status.channels">
                            <slider
                                v-bind:value="state.status.channels[index]"
                              min="0"
                              max="255"
                              v-bind:color="sliderColor(index)"
                              @change="setLight"
                            />
                          </div>
                        </div>
                      </div>
                    </div>
                    <div class="field is-horizontal">
                      <div class="field-body">
                        <div class="field">
                          <label>Brightness</label>
                          <div class="control" v-if="state.status.brightness">
                            <slider
                              v-bind:value="state.status.brightness"
                              min="0"
                              max="100"
                              @change="setLight"
                            />
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </article>
            </div>
            <!-- Light Channel status -->
            <div class="tile is-parent">
              <article class="tile is-child box">
                <p class="subtitle is-4">
                  Channels Power
                </p>
                <div class="tags are-medium">
                  <span
                    v-for="(led, index) in state.status.channels"
                    v-show="active[index]"
                    v-bind:key="index"
                    v-bind:style="{borderColor: sliderColor(index)}"
                    class="tag channel-power"
                  >
                    {{ ledPower(index) }}
                  </span>
                </div>
                <p><strong>Total: aprox. {{ totalPower() }} Watts</strong></p>
              </article>
            </div>
          </div>
        </div>

        <div class="tile">
          <div class="tile is-vertical">
            <!-- Hardware Status -->
            <div class="tile is-parent">
              <article class="tile is-child notification bg-notification is-primary">
                <p class="title">
                  Hardware
                </p>
                <p class="subtitle">
                  Device info
                </p>
                <div class="columns">
                  <div class="column has-text-left">
                    <ul>
                      <li>Firmware: {{ state.status.firmware }}</li>
                      <li>Hardware: {{ state.status.hardware }}</li>
                      <li>Free Heap: {{ state.status.freeHeap }}</li>
                      <li>WIFI: {{ state.status.wifiMode }}</li>
                      <li>IP Address: {{ state.status.ipAddress }}</li>
                      <li>MAC: {{ state.status.macAddress }}</li>
                    </ul>
                  </div>
                  <div class="column has-text-left">
                    <ul>
                      <li>Time: {{ state.status.localTime }}</li>
                      <li>Uptime: {{ state.status.upTime }}</li>
                      <li>Power IN: {{ state.status.vcc / 1000 }} volt.</li>
                      <li>Board Temperature: {{ state.status.board_temperature / 100 }} °C</li>
                      <li>MQTT Server: {{ mqttStatus }}</li>
                      <li>NTP: {{ ntpStatus }}</li>
                    </ul>
                  </div>
                </div>
                <div class="columns">
                  <div class="column has-text-centered">
                    <div class="field is-grouped">
                      <div class="control">
                        <a
                          class="button is-light"
                          @click="updateDevice"
                        >Update</a>
                        <p class="help has-text-centered">
                          Firmware
                        </p>
                      </div>
                      <div class="control">
                        <a
                          class="button is-warning"
                          @click="rebootDevice"
                        >Restart</a>
                        <p class="help has-text-centered">
                          Device
                        </p>
                      </div>
                      <div class="control">
                        <a
                          class="button is-danger"
                          @click="restoreDevice"
                        >Factory<span class="is-hidden-mobile"> Reset</span></a>
                        <p class="help has-text-centered">
                          Initial conf.
                        </p>
                      </div>
                    </div>
                  </div>
                </div>
              </article>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>

import eventBus from '@/eventBus'
import { http } from '@/service/http'
import { store } from "@/service/store";

export default {
  name: 'Home',
  data () {
    return {
      state: store.state,
      colors: [],
      power: [],
      active: [],
      refreshInterval: null,
      isLoading: true
    }
  },
  computed: {
    mqttStatus: function () {
      let status = "";
      if (this.state.status.length) {
        status = this.state.status.mqttService.enabled ? 'on' : 'off'
        status += this.state.status.mqttService.connected ? ', connected' : ', disconnected'
      }
      return status
    },
    ntpStatus: function () {
      let status = "";
      if (this.state.status.length) {
        status = this.state.status.ntpService.enabled ? 'on' : 'off'
        status += this.state.status.ntpService.sync ? ', sync`d' : ', not sync'
      }
      return status
    },
    scheduleStatus: function () {
      if (this.state.status.length) {
        return this.state.status.schedule_status ? 'schedule is running' : 'schedule stopped'
      }
      return ''
    }
  },
  mounted () {
    this.requestData()
  },
  methods: {
    async requestData () {
      eventBus.$emit('loading', true)
      try {
        await store.loadStatus();
        await store.loadSettings();

        /* Leds config */
        this.colors = this.state.leds.filter(led => led.state === 1).map((value) => value.color)
        this.power = this.state.leds.filter(led => led.state === 1).map((value) => value.power)
        this.active = this.state.leds.map((led) => led.state)

        console.log(this.state.status.brightness)

      } catch (e) {
        eventBus.$emit('message', store.state.errors.join('; '), 'danger')
      } finally {
        store.clearErrors();
      }
      eventBus.$emit('loading', false)
    },
    ledStatus (value) {
      if (value) { return `${parseInt(value)}%` } return `OFF ${parseInt(value)}%`
    },
    ledPower (index) {
      let string = ''
      const channels = this.state.status.channels

      if (!this.power[index]) {
        return 'POWER not SET '
      }

      if (channels[index]) {
        const percent = channels[index] / 255
        const power = this.power[index] * this.state.status.brightness / 100

        string += `${parseInt(percent * 100)}%`
        string += ` (${parseInt(percent * power)} Watts)`
        return string
      }

      return 'OFF '
    },
    totalPower () {
      return 0;
      // const active = this.active
      // const power = this.power
      // const brightness = this.state.status.brightness / 100
      // const channels = this.state.status.channels.filter((value, index) => active[index])
      // let totalPow = 0
      //
      // if (channels) {
      //   totalPow = channels.reduce((total, duty, index) => total + duty / 255 * power[index] * brightness, 0)
      // }
      //
      // return totalPow.toFixed(2)
    },
    async restoreDevice () {
      if (confirm('Do you really want to factory restore your device?')) {
        if (confirm('Sure?')) {
          try {
            let response = await http.get('/factory')
            if (response.data.success) {
              eventBus.$emit('message', 'Factory Restoring...', 'success')
            }
          } catch (e) {
            if (e.response) {
              eventBus.$emit('message', e.response.data.message, 'danger')
            } else {
              eventBus.$emit('message', 'unexpected error', 'danger')
            }
          }
        }
      }
    },
    async rebootDevice () {
      if (confirm('Do you really want to restart your device?')) {
        if (confirm('Sure?')) {
          try {
            let response = await http.get('/reboot')
            if (response.data.success) { eventBus.$emit('message', 'Rebooting...', 'success') }
          } catch (e) {
            if (e.response) {
              eventBus.$emit('message', e.response.data.message, 'danger')
            } else {
              eventBus.$emit('message', 'unexpected error', 'danger')
            }
          }
        }
      }
    },
    async updateDevice () {
      if (confirm('Do you really want to update firmware from cloud?')) {
        if (confirm('Sure?')) {
          try {
            let response = await http.get('/update')
            if (response.data.success) {
              eventBus.$emit('message', 'Update Firmware...', 'success')
            }
          } catch (e) {
            if (e.response) {
              eventBus.$emit('message', e.response.data.message, 'danger')
            } else {
              eventBus.$emit('message', 'unexpected error', 'danger')
            }
          }
        }
      }
    },
    async setLight () {
      if (this.state.status.channels.length > 0 && this.state.status.brightness !== undefined) {
        try {
          console.log(this.state.status)
          /* Set New Duty value */
          let response = await http.post('/api/light', {
            channels: this.state.status.channels,
            brightness: this.state.status.brightness })

          if (response.data.success) { eventBus.$emit('message', 'Applied...', 'success') }
        } catch (e) {
          if (e.response) {
            eventBus.$emit('message', e.response.data.message, 'danger')
          } else {
            eventBus.$emit('message', 'unexpected error', 'danger')
          }
        }
      }
    },
    async toggleSchedule () {
      try {
        const newStatus = !this.state.schedule_status
        let response = await http.post('/api/schedule/status', { status: newStatus })
        // eslint-disable-next-line camelcase
        if (response.data.success) { this.state.schedule_status = !!newStatus }
      } catch (e) {
        if (e.response) {
          eventBus.$emit('message', e.response.data.message, 'danger')
        } else {
          eventBus.$emit('message', 'unexpected error', 'danger')
        }
      }
    },
    scheduleActionIcon () {
      return this.state.schedule_status ? 'pause-icon' : 'play-icon'
    },
    sliderColor (index) {
      return this.colors[index]
    }
  }
}
</script>

<style lang="scss">
.tag.channel-power {
  border: dashed 1px;
  border-bottom: solid 0.25em;
  box-sizing: border-box;
  padding-left: 0.25em !important;
  padding-right: 0.25em !important;
}
</style>
