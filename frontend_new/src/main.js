import { createApp } from 'vue'
import App from './App.vue'
import router from './router'

import GlobalOptions from '@/components/globalOptions'
import GlobalComponents from '@/components/globalComponents'

import './assets/style/custom.scss'

// Vue.config.productionTip = false

createApp(App)
    .use(router)
    .use(GlobalOptions)
    .use(GlobalComponents)
    .mount('#app')
