<template>
    <div class="columns is-marginless is-multiline">
        <div class="column is-12 has-text-left">
            <!-- Fan -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Fan Controller</span>
            <div class="box">
                <!-- Installed -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Installed</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <toggle-switch
                                    v-model="state.cooling.installed"
                                    round
                                />
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Start Temperature -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Start Temperature</label>
                    </div>
                    <div class="field-body">
                        <div class="field has-addons">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.start_temp"
                                    class="input"
                                    maxlength="2"
                                    placeholder="Start Temperature"
                                />
                            </div>
                            <p class="control">
                                <a class="button is-static">
                                    °C
                                </a>
                            </p>
                        </div>
                    </div>
                </div>

                <!-- Target Temperature -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Target Temperature</label>
                    </div>
                    <div class="field-body">
                        <div class="field has-addons">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.target_temp"
                                    class="input"
                                    maxlength="2"
                                    placeholder="Target Temperature"
                                />
                            </div>
                            <p class="control">
                                <a class="button is-static">
                                    °C
                                </a>
                            </p>
                        </div>
                    </div>
                </div>

                <!-- Max Temperature -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Max Temperature</label>
                    </div>
                    <div class="field-body">
                        <div class="field has-addons">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.max_temp"
                                    class="input"
                                    maxlength="2"
                                    placeholder="Target Temperature"
                                />
                            </div>
                            <p class="control">
                                <a class="button is-static">
                                    °C
                                </a>
                            </p>
                        </div>
                    </div>
                </div>

                <!-- Tahometer -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">FAN speed</label>
                    </div>
                    <div class="field-body">
                        <div class="field has-addons">
                            <div class="control">
                                <span class="button is-static">{{ state.cooling.tachometer }}</span>
                            </div>
                            <div class="control">
                                <span class="button is-static">rpm</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Fan PID -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Fan PID</span>
            <div class="box">
                <!-- Proportional gain -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Proportional gain</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.pid_kp"
                                    class="input"
                                    maxlength="3"
                                    step="0.1"
                                    placeholder="proportional gain"
                                />
                            </div>
                        </div>
                    </div>
                </div>
                <!-- integral gain -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Integral gain</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.pid_ki"
                                    class="input"
                                    maxlength="3"
                                    step="0.1"
                                    placeholder="integral gain"
                                />
                            </div>
                        </div>
                    </div>
                </div>
                <!-- integral gain -->
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Derivative gain</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <input-number
                                    v-model.number="state.cooling.pid_kd"
                                    class="input"
                                    maxlength="3"
                                    step="0.1"
                                    placeholder="derivative gain"
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
            @click="saveFan"
        >
          <vue-feather type="check"/> Apply
        </span>
                <span
                    class="button is-danger"
                    @click="loadFan"
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
    name: 'Fan',
    data () {
        return {
            state: store.state,
            is_loading: false
        }
    },
    mounted () {
        this.loadFan()
    },
    methods: {
        async saveFan () {
            try {
                const success = await store.saveSettings('cooling')
                if (success) {
                    eventBus.$emit('message', 'Saved', 'success')
                }
            } catch (e) {
                if (e.response) {
                    eventBus.$emit('message', e.response.data.message, 'danger')
                } else {
                    eventBus.$emit('message', 'unexpected error', 'danger')
                }
            }
        },
        async loadFan () {
            eventBus.$emit('loadSettings')
        }
    }
}
</script>
