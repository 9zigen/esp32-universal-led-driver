<template>
  <div class="container m-top-touch-navbar">
    <div class="columns is-marginless">
      <div class="column">
        <div class="content has-text-left">
          <h1 class="title">
            WiFi Settings
          </h1>
          <p class="subtitle is-small">
            You can add 2 networks.
          </p>
        </div>
      </div>
    </div>

    <!-- Wifi -->
    <div class="columns is-marginless">
      <div class="column">
        <transition-group
          name="list"
          appear
        >
          <div
            v-for="(network, index) in networks"
            v-bind:key="index"
            class="columns"
          >
            <div class="column">
              <div class="box">
                <div class="field has-text-left">
                  <!--  SSID -->
                  <label class="label">Network name (SSID) #{{ index }}</label>
                  <div class="field">
                    <div class="control">
                      <input
                        v-model="network.ssid"
                        class="input"
                        type="text"
                        placeholder="WiFi Network Name"
                      >
                    </div>
                  </div>
                  <!--  Password -->
                  <label class="label">Password</label>
                  <div class="field">
                    <div class="control">
                      <input
                        v-model="network.password"
                        class="input"
                        type="text"
                        placeholder="Password"
                      >
                    </div>
                  </div>
                  <!--  DHCP -->
                  <label class="label">Use DHCP</label>
                  <div class="field">
                    <div class="control">
                      <toggle-switch
                        v-model:checked="network.dhcp"
                        round
                      />
                    </div>
                  </div>
                  <!--  IP -->
                  <transition name="list">
                    <div
                      v-if="!network.dhcp"
                      class="columns is-centered"
                    >
                      <div class="column is-12">

                        <label class="label">Static IP:</label>
                        <div class="field has-addons">
                          <!-- IP Address -->
                          <div class="control">
                            <label>Address</label>
                            <input-ip
                              v-model="network.ip_address"
                              placeholder="192.168.1.200"
                            />
                          </div>
                          <!-- Mask -->
                          <div class="control ml-5 is-invisible-mobile">
                            <label>Mask</label>
                            <input-ip
                              v-model="network.mask"
                              placeholder="255.255.255.0"
                            />
                          </div>
                        </div>

                        <div class="field has-addons">
                          <!-- Mask -->
                          <div class="control is-invisible-tablet">
                            <label>Mask</label>
                            <input-ip
                              v-model="network.mask"
                              placeholder="255.255.255.0"
                            />
                          </div>
                        </div>

                        <div class="field has-addons">
                          <!-- Gateway -->
                          <div class="control">
                            <label>Gateway</label>
                            <input-ip
                              v-model="network.gateway"
                              placeholder="192.168.1.1"
                            />
                          </div>
                          <!-- DNS -->
                          <div class="field">
                            <div class="control ml-5 is-invisible-mobile">
                              <label>DNS</label>
                              <input-ip
                                v-model="network.dns"
                                placeholder="192.168.1.1"
                              />
                            </div>
                          </div>
                        </div>

                        <div class="field has-addons">
                          <!-- DNS -->
                          <div class="field">
                            <div class="control is-invisible-tablet">
                              <label>DNS</label>
                              <input-ip
                                v-model="network.dns"
                                placeholder="192.168.1.1"
                              />
                            </div>
                          </div>
                        </div>
                      </div>
                    </div>
                  </transition>


                  <div class="field-body">


                    <div class="field is-hidden-mobile">
                      <div class="control has-text-centered">
                        <span
                          class="button is-danger is-flex-mobile"
                          @click="deleteNetwork(index)"
                        >
                          <x-icon size="1.5x" /> Delete
                        </span>
                      </div>
                    </div>
                  </div>
                </div>



                <div class="field is-hidden-tablet">
                  <div class="control has-text-centered">
                    <span
                      class="button is-danger is-flex-mobile"
                      @click="deleteNetwork(index)"
                    >
                      <x-icon size="1.5x" /> Delete
                    </span>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </transition-group>
        <!-- End Networks -->
      </div>
    </div>

    <!-- Buttons -->
    <div class="buttons is-centered">
      <a
        class="button is-info"
        @click="addNetwork"
      ><plus-icon size="1.5x" />Add New</a>
      <a
        class="button is-primary"
        @click="saveNetwork"
      ><check-icon size="1.5x" /> Apply</a>
    </div>
  </div>
</template>

<script>

import { http } from '@/service/http'
import eventBus from '@/eventBus'

export default {
  name: 'Wifi',
  data () {
    return {
      networks: [],
      capacity: 2
    }
  },
  mounted () {
    this.loadNetwork()
  },
  methods: {
    addNetwork () {
      if (this.networks.length < this.capacity) {
        this.networks.push({
          ssid: '',
          password: '',
          ip_address: '192.168.1.100',
          mask: '255.255.255.0',
          gateway: '192.168.1.1',
          dns: '192.168.1.1',
          dhcp: true
        })
      } else {
        /* error - MAX Networks reached */
        eventBus.$emit('message', 'MAX Networks reached', 'danger')
      }
    },
    deleteNetwork (id) {
      this.networks.splice(id, 1)
    },
    async saveNetwork () {
      try {
        const response = await http.post('/api/settings', { networks: this.networks })
        if (response.data.success) {
          eventBus.$emit('message', 'Saved', 'success')
          setTimeout(() => {
            if (confirm('Reboot device to reconnect WiFi?')) {
              this.rebootDevice()
            }
          }, 3000)
        }
      } catch (e) {
        if (e.response) {
          eventBus.$emit('message', e.response.data.message, 'danger')
        } else {
          eventBus.$emit('message', 'unexpected error', 'danger')
        }
      }
    },
    async loadNetwork () {
      const response = await http.get('/api/settings')
      this.networks = response.data.networks
    },
    async rebootDevice () {
      try {
        const response = await http.get('/reboot')
        if (response.data.success) { eventBus.$emit('message', 'Rebooting...', 'success') }
      } catch (e) {
        eventBus.$emit('message', e, 'danger')
      }
    }
  }
}
</script>
