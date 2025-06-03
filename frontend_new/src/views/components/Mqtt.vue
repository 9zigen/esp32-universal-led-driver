<template>
  <div class="columns is-marginless is-multiline">
    <div class="column is-12 has-text-left">
      <!-- MQTT -->
      <span class="subtitle is-4 is-uppercase has-text-grey-light">MQTT</span>
      <div class="box">
        <!-- Server IP -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Server IP Address</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input-ip
                  v-model="state.services.mqtt_ip_address"
                  placeholder="IP Address"
                />
              </div>
            </div>
          </div>
        </div>
        <!-- Port -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Server Port</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input-number
                  v-model="state.services.mqtt_port"
                  class="input"
                  type="number"
                  placeholder="Server Port"
                />
              </div>
            </div>
          </div>
        </div>
        <!-- User -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">User</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input
                  v-model="state.services.mqtt_user"
                  class="input"
                  type="text"
                  placeholder="User"
                >
              </div>
            </div>
          </div>
        </div>
        <!-- Password -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Password</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input
                  v-model="state.services.mqtt_password"
                  class="input"
                  type="password"
                  placeholder="Password"
                >
              </div>
            </div>
          </div>
        </div>
        <!-- Enable -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">MQTT</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <toggle-switch
                  v-model:checked="state.services.enable_mqtt"
                  round
                />
              </div>
              <p class="help">
                Enable
              </p>
            </div>
          </div>
        </div>
        <!-- QoS -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">QoS</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <div class="select">
                  <select
                    v-model.number="state.services.mqtt_qos"
                    name="mqtt_qos"
                  >
                    <option
                      v-for="(option, index) in $mqttOptions"
                      v-bind:key="index"
                      v-bind:value="option.value"
                    >
                      {{ option.text }}
                    </option>
                  </select>
                </div>
              </div>
            </div>
          </div>
        </div>
        <!-- Interval -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Report interval</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input
                  v-model="state.services.mqtt_report_interval"
                  class="input"
                  type="number"
                  step="1"
                  placeholder="User"
                >
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- ThingsBoard -->
      <span class="subtitle is-4 is-uppercase has-text-grey-light">Things Board</span>
      <div class="box">
        <!-- Endpoint -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Endpoint</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input
                  v-model="state.thingsboard.endpoint"
                  class="input"
                  type="text"
                  placeholder="endpoint url"
                >
              </div>
            </div>
          </div>
        </div>
        <!-- Token -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Token</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <input
                  v-model="state.thingsboard.token"
                  class="input"
                  type="text"
                  placeholder="device token"
                >
              </div>
            </div>
          </div>
        </div>
        <!-- Enable -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">Enable</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <toggle-switch
                  v-model:checked="state.thingsboard.enable"
                  round
                />
              </div>
            </div>
          </div>
        </div>
        <!-- RPC -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">RPC</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <toggle-switch
                  v-model:checked="state.thingsboard.rpc"
                  round
                />
              </div>
            </div>
          </div>
        </div>
        <!-- MQTT QoS -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">MQTT QoS</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <div class="select">
                  <select
                    v-model.number="state.thingsboard.qos"
                    name="mqtt_qos"
                  >
                    <option
                      v-for="(option, index) in $mqttOptions"
                      v-bind:key="index"
                      v-bind:value="option.value"
                    >
                      {{ option.text }}
                    </option>
                  </select>
                </div>
              </div>
            </div>
          </div>
        </div>
        <!-- MQTT Retain -->
        <div class="field is-horizontal">
          <div class="field-label is-normal">
            <label class="label">MQTT Retain</label>
          </div>
          <div class="field-body">
            <div class="field">
              <div class="control">
                <toggle-switch
                  v-model:checked="state.thingsboard.retain"
                  round
                />
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div class="column">
      <!-- Buttons -->
      <div class="buttons is-centered">
        <span
          class="button is-primary"
          @click="saveMqtt"
        >
          <vue-feather type="check"/> Apply
        </span>
        <span
          class="button is-danger"
          @click="loadMqtt"
        >
          <vue-feather type="x"/> Cancel
        </span>
      </div>
    </div>
  </div>
</template>

<script>
import eventBus from '@/eventBus'
import { store } from "@/service/store";

export default {
  name: 'Mqtt',
  data () {
    return {
      state: store.state,
    }
  },
  methods: {
    async saveMqtt () {
      try {
        const success = await store.saveSettings(['thingsboard', 'services'])
        if (success) {
          eventBus.$emit('message', 'Saved', 'success')
        }
      } catch (e) {
        eventBus.$emit('message', store.state.errors.join('; '), 'danger')
      } finally {
        store.clearErrors();
      }
    },
    async loadMqtt () {
      await store.loadSettings()
    }
  }
}
</script>
