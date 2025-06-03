<template>
  <div class="container m-top-touch-navbar">
    <div class="columns is-marginless">
      <div class="column">
        <div class="content has-text-left">
          <h1 class="title">
            Settings
          </h1>
        </div>
      </div>
    </div>

    <div class="columns is-marginless">
      <div class="column">
        <!-- MAIN -->
        <div class="buttons has-addons">
          <span
            v-for="(tab, idx) in tabs"
            v-bind:key="tab"
            v-bind:class="['button', { 'is-primary': currentTab === tab }]"
            @click="selectTab(idx)"
          >
            <vue-feather v-bind:type="tabIcon(idx)"></vue-feather>
          </span>
        </div>
      </div>
    </div>

    <!-- Dynamic component -->
    <keep-alive>
      <component v-bind:is="currentTabComponent"/>
    </keep-alive>
  </div>
</template>

<script>
import TabMain from './components/Main'
import TabMqtt from './components/Mqtt'
import TabTime from './components/Time'
import TabLeds from './components/Leds'
import TabFan from './components/Fan'
import TabGpio from './components/Gpio'
import TabUser from './components/User'
import { store } from "@/service/store";

export default {
  name: 'Settings',
  components: {
    TabMain,
    TabMqtt,
    TabTime,
    TabLeds,
    TabFan,
    TabGpio,
    TabUser
  },
  data () {
    return {
      currentTab: 'Main',
      tabs: ['Main', 'Time', 'MQTT', 'Leds', 'Fan', 'Gpio', 'User'],
      icons: ['settings', 'clock', 'share-2', 'sun', 'wind', 'cpu', 'user'],
    }
  },
  computed: {
    currentTabComponent () {
      return `tab-${this.currentTab.toLowerCase()}`
    }
  },
  mounted () {
    this.loadData()
  },
  methods: {
    async loadData () {
      await store.loadSettings();
    },
    selectTab (id) {
      this.currentTab = this.tabs[id]
    },
    tabIcon (id) {
      return this.icons[id]
    }
  }
}
</script>
