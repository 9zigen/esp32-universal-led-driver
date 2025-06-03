<template>
  <div class="container">
    <div class="tile is-ancestor is-marginless">
      <div class="tile is-vertical">
        <!-- top row -->
        <div class="tile">
          <!-- left col -->
          <div class="tile is-vertical is-4">
            <div class="tile is-parent">
              <article class="tile is-child has-text-left">
                <span class="subtitle is-4 is-uppercase has-text-grey-light">Local Time</span>
                <div class="box has-text-centered">
                  <div class="content">
                    <clock v-bind:time="timeString" />
                  </div>
                </div>
              </article>
            </div>
          </div>

          <!-- right col -->
          <div
            v-if="state.time"
            class="tile is-vertical"
          >
            <div class="tile is-parent">
              <article class="tile is-child has-text-left">
                <span class="subtitle is-4 is-uppercase has-text-grey-light">NTP</span>
                <div class="box">
                  <div class="field is-horizontal">
                    <div class="field-label is-normal">
                      <label class="label">Server Name</label>
                    </div>
                    <div class="field-body">
                      <div class="field">
                        <div class="control is-fullwidth">
                          <input
                            v-model="state.services.ntp_server"
                            class="input"
                            type="text"
                            placeholder="NTP Server Name"
                          />
                        </div>
                      </div>
                    </div>
                  </div>
                  <div class="field is-horizontal">
                    <div class="field-label is-normal">
                      <label class="label">Enable</label>
                    </div>
                    <div class="field-body">
                      <div class="field">
                        <div class="control">
                          <toggle-switch
                            v-model:checked="state.services.enable_ntp"
                            round
                          />
                        </div>
                      </div>
                    </div>
                  </div>
                  <div class="field is-horizontal">
                    <div class="field-label is-normal">
                      <label class="label">TimeZone</label>
                    </div>
                    <div class="field-body">
                      <div class="field">
                        <div class="control">
                          <div class="select is-fullwidth">
                            <select
                              v-model.number="state.services.utc_offset"
                            >
                              <option
                                v-for="(option, index) in $tzOptions"
                                v-bind:key="index"
                                v-bind:value="option.value"
                              >
                                {{ option.text }} ({{ option.value }})
                              </option>
                            </select>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                  <div class="field is-horizontal">
                    <div class="field-label is-normal">
                      <label class="label">DST</label>
                    </div>
                    <div class="field-body">
                      <div class="field">
                        <div class="control">
                          <toggle-switch
                            v-model:checked="state.services.ntp_dst"
                            round
                          />
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </article>
            </div>
          </div>
        </div>

        <!-- bottom row -->
        <div class="tile">
          <div class="tile is-vertical">
            <div class="tile is-parent">
              <div class="container">
                <div class="buttons is-centered">
                  <span
                    class="button is-primary"
                    @click="saveTime"
                  >
                    <vue-feather type="check"/> Apply
                  </span>
                  <span
                    class="button is-danger"
                    @click="loadTime"
                  >
                    <vue-feather type="x"/> Cancel
                  </span>
                </div>
              </div>
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
import { store } from "@/service/store"
import Clock from '../../components/Ui/Clock'

export default {
  name: 'Time',
  components: { Clock },
  data () {
    return {
      timeString: '',
      state: store.state,
      browserTime: {
        hour: '',
        minute: '',
        second: ''
      },
      date: new Date(),
      refreshInterval: null
    }
  },
  mounted () {
    /* update current time */
    this.refreshInterval = setInterval(() => {
      const dateTime = new Date()
      this.browserTime.hour = dateTime.getHours()
      this.browserTime.minute = dateTime.getMinutes()
      this.browserTime.second = dateTime.getSeconds()
      this.timeString = this.timeToString()
    }, 1000)
  },
  unmounted () {
    clearInterval(this.refreshInterval)
  },
  methods: {
    async rebootDevice () {
      try {
        const response = await http.get('/reboot')
        if (response.data.success) { eventBus.$emit('message', 'Rebooting...', 'success') }
      } catch (e) {
        if (e.response) {
          eventBus.$emit('message', e.response.data.message, 'danger')
        } else {
          eventBus.$emit('message', 'unexpected error', 'danger')
        }
      }
    },
    async saveTime () {
      try {
        const success = await store.saveSettings(['time', 'services'])
        if (success) {
          eventBus.$emit('message', 'Saved', 'success')
          setTimeout(() => {
            if (confirm('Reboot Led Controller to apply the time settings?')) {
              this.rebootDevice()
            }
          }, 3000)
        }
      } catch (e) {
        eventBus.$emit('message', store.state.errors.join('; '), 'danger')
      } finally {
        store.clearErrors();
      }
    },
    async loadTime () {
      await store.loadSettings()
    },
    timeToString () {
      return this.browserTime.hour + ':' + this.browserTime.minute + ':' + this.browserTime.second
    }
  }
}
</script>
