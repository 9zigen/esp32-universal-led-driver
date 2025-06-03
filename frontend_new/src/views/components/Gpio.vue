<template>
    <div class="columns is-marginless is-multiline">
        <div class="column is-12 has-text-left">
            <!-- Gpio main function -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Gpio Main Functions</span>
            <div class="box">
                <div class="columns is-multiline">
                    <div
                        v-for="(gpio) in state.gpio"
                        v-bind:key="gpio.pin"
                        class="column is-12"
                    >
                        <!-- Gpio PIN -->
                        <div class="field is-horizontal">
                            <div class="field-label is-normal">
                                <label class="label">PIN {{gpio.pin}}</label>
                            </div>
                            <div class="field-body">
                                <div class="field">
                                    <div class="control">
                                        <div class="select">
                                            <select
                                                v-model.number="gpio.function"
                                                name="pin"
                                            >
                                                <option
                                                    v-for="(option, index) in gpioFunctions"
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
                    </div>
                </div>
            </div>
            <!-- Gpio alternative function -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Gpio Alternative Functions</span>
            <div class="box">
                <div class="columns is-multiline">
                    <div
                        v-for="(gpio) in state.gpio"
                        v-bind:key="gpio.pin"
                        class="column is-12"
                    >
                        <!-- Gpio PIN -->
                        <div class="field is-horizontal">
                            <div class="field-label is-normal">
                                <label class="label">PIN {{gpio.pin}}</label>
                            </div>
                            <div class="field-body">
                                <div class="field">
                                    <div class="control">
                                        <div class="select">
                                            <select
                                                v-model.number="gpio.alt_function"
                                                name="pin"
                                            >
                                                <option
                                                    v-for="(option, index) in $gpioFunctions"
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
                    </div>
                </div>
            </div>
        </div>

        <!-- Buttons -->
        <div class="column">
            <div class="buttons is-centered">
        <span
            class="button is-primary"
            @click="saveGpioConfig"
        >
          <vue-feather type="check"/> Apply
        </span>
                <span
                    class="button is-danger"
                    @click="reset"
                >
          <vue-feather type="x"/> Cancel
        </span>
            </div>
        </div>
    </div>
</template>

<script>
import eventBus from '@/eventBus'
import { store } from "@/service/store"

export default {
    name: 'Gpio',
    data () {
        return {
            state: store.state
        }
    },
    mounted () {
        // this.loadGpioOptions()
    },
    methods: {
        // async loadGpioOptions () {
        //   try {
        //     let response = await getGpio()
        //     if (response.data.gpioFunctions) {
        //       this.gpioFunctions = response.data.gpioFunctions
        //     }
        //   } catch (e) {
        //     if (e.response) {
        //       eventBus.$emit('message', e.response.data.message, 'danger')
        //     } else {
        //       eventBus.$emit('message', 'unexpected error', 'danger')
        //     }
        //   }
        // },
        async saveGpioConfig () {
            try {
                const success = await store.saveSettings('gpio')
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
        async reset () {
            eventBus.$emit('loadSettings')
        }
    }
}
</script>
