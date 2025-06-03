<template>
    <div class="columns is-marginless is-multiline">
        <div class="column is-12 has-text-left">
            <!-- MAIN -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Identity</span>
            <div class="box">
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Name</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <input
                                    v-model="state.services.hostname"
                                    class="input"
                                    type="text"
                                    placeholder="Hostname"
                                >
                            </div>
                        </div>
                    </div>
                </div>

                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">OTA url</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control is-expanded">
                                <input
                                    v-model="state.services.ota_url"
                                    class="input"
                                    type="text"
                                    placeholder="OTA Url"
                                >
                            </div>
                        </div>
                    </div>
                </div>

                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Firmware</label>
                    </div>
                    <div class="field-body">
                        <div class="field has-addons">
                            <div class="file has-name is-fullwidth">
                                <label class="file-label">
                                    <input
                                        ref="file"
                                        class="file-input"
                                        type="file"
                                        @change="selectFile"
                                    >
                                    <span class="file-cta">
                    <span class="file-label">
                      Choose a file…
                    </span>
                  </span>
                                    <span
                                        v-if="selectedFiles"
                                        class="file-name is-hidden-mobile"
                                    >
                    <span
                        v-for="(file, id) in selectedFiles"
                        v-bind:key="id"
                    >
                      {{ file.name }} | {{ file.size / 1000 }} kb
                    </span>
                  </span>
                                </label>
                            </div>

                            <div class="control">
                <span
                    v-if="selectedFiles"
                    class="button is-warning"
                    @click="upload"
                >
                  Upload
                </span>
                            </div>
                        </div>
                    </div>
                </div>
                <progress
                    v-show="progress"
                    class="progress"
                    v-bind:value="progress"
                    max="100"
                />
            </div>

            <!-- Schedule Config -->
            <span class="subtitle is-4 is-uppercase has-text-grey-light">Schedule</span>
            <div class="box">
                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Mode</label>
                    </div>
                    <div class="field-body">
                        <div class="field is-narrow">
                            <div class="control">
                                <!--                <div class="select is-fullwidth">-->
                                <!--                  <select-->
                                <!--                    v-model="state.scheduleConfig.mode"-->
                                <!--                    name="mode"-->
                                <!--                  >-->
                                <!--                    <option-->
                                <!--                      v-for="(option, index) in $ledScheduleOptions"-->
                                <!--                      v-bind:key="index"-->
                                <!--                      v-bind:value="option.value"-->
                                <!--                    >-->
                                <!--                      {{ option.text }}-->
                                <!--                    </option>-->
                                <!--                  </select>-->
                                <!--                </div>-->
                            </div>
                        </div>
                    </div>
                </div>

                <div class="field is-horizontal" v-show="state.schedule_config.mode === 0">
                    <div class="field-label is-normal">
                        <label class="label">Transition duration</label>
                    </div>
                    <div class="field-body">
                        <div class="field is-narrow">
                            <div class="field has-addons is-narrow">
                                <div class="control">
                                    <input
                                        v-model.number="state.schedule_config.simple_mode_duration"
                                        class="input"
                                        type="number"
                                        placeholder="sync group"
                                    >
                                </div>
                                <p class="control">
                                    <a class="button is-static">
                                        Min.
                                    </a>
                                </p>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Gamma</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <div class="select is-narrow">
                                    <select
                                        v-model="state.schedule_config.gamma"
                                        name="gamma"
                                    >
                                        <option
                                            v-for="(option, index) in $ledGammaOptions"
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

                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">RGB Picker</label>
                    </div>
                    <div class="field-body">
                        <div class="field">
                            <div class="control">
                                <toggle-switch
                                    v-model="state.schedule_config.rgb"
                                    round
                                />
                            </div>
                        </div>
                    </div>
                </div>

                <div class="field is-horizontal">
                    <div class="field-label is-normal">
                        <label class="label">Sync controllers</label>
                    </div>
                    <div class="field-body">
                        <div class="field is-narrow">
                            <div class="control">
                                <toggle-switch
                                    v-model="state.schedule_config.use_sync"
                                    round
                                />
                            </div>
                        </div>
                    </div>
                </div>
                <div class="field is-horizontal" v-show="state.schedule_config.use_sync">
                    <div class="field-label is-normal">
                        <label class="label">Group ID</label>
                    </div>
                    <div class="field-body">
                        <div class="field is-narrow">
                            <div class="field has-addons is-narrow">
                                <div class="control">
                                    <input
                                        v-model.number="state.schedule_config.sync_group"
                                        class="input"
                                        type="number"
                                        placeholder="sync group"
                                    >
                                </div>
                                <p class="control">
                                    <a class="button is-static">
                                        0-255
                                    </a>
                                </p>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="field is-horizontal" v-show="state?.schedule_config.use_sync">
                    <div class="field-label is-normal">
                        <label class="label">Is Primary</label>
                    </div>
                    <div class="field-body">
                        <div class="field is-narrow">
                            <div class="control">
                                <toggle-switch
                                    v-model="state.schedule_config.sync_master"
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
            @click="saveMain"
        >
          <vue-feather type="check"/> Apply
        </span>
                <span
                    class="button is-danger"
                    @click="loadMain"
                >
          <vue-feather type="x"/> Cancel
        </span>
            </div>
        </div>
    </div>
</template>

<script>
import eventBus from '@/eventBus'
import { http } from '@/service/http'
import { store } from "@/service/store"
import UploadService from '@/service/uploadFileService'

export default {
    name: 'Services',
    data () {
        return {
            selectedFiles: undefined,
            currentFile: undefined,
            hostname: null,
            ota_url: null,
            progress: 0,
            fileInfos: [],
            state: store.state
        }
    },
    methods: {
        async saveMain () {
            try {
                const success = await store.saveSettings()
                if (success) {
                    eventBus.$emit('message', 'Saved', 'success')
                }
            } catch (e) {
                eventBus.$emit('message', store.state.errors.join('; '), 'danger')
            } finally {
                store.clearErrors();
            }
        },
        async loadMain () {
            await store.loadSettings()
        },
        async updateDevice () {
            try {
                const response = await http.get('/update')
                if (response.data.success) {
                    eventBus.$emit('message', 'Update Firmware...', 'success')
                    setTimeout(() => {
                        window.location.reload()
                    }, 1000)
                }
            } catch (e) {
                if (e.response) {
                    eventBus.$emit('message', e.response.data.message, 'danger')
                } else {
                    eventBus.$emit('message', 'unexpected error', 'danger')
                }
            }
        },
        selectFile () {
            this.selectedFiles = this.$refs.file.files
        },
        upload () {
            this.progress = 0
            this.currentFile = this.selectedFiles.item(0)
            UploadService.upload(this.currentFile, event => {
                this.progress = Math.round((100 * event.loaded) / event.total)
            })
                .then(response => {
                    if (response.data.success) {
                        eventBus.$emit('message', 'Firmware has been successfully updated', 'success')
                        this.progress = 0
                        setTimeout(() => {
                            eventBus.$emit('loading')
                            this.$router.replace('/login')
                        }, 10000)
                    } else {
                        eventBus.$emit('message', response.data, 'danger')
                    }
                })
                .catch(() => {
                    this.progress = 0
                    this.currentFile = undefined
                    eventBus.$emit('message', 'Could not upload the file!', 'danger')
                })

            this.selectedFiles = undefined
        }
    }
}
</script>