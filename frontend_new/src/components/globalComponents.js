import Chart from './Ui/Chart'
import ScheduleChart from '@/components/Ui/ScheduleChart'
import ToggleSwitch from '@/components/Inputs/ToggleSwitch'
import Slider from '@/components/Inputs/InputSlider'
import InputIP from '@/components/Inputs/InputIP'
import InputNumber from './Inputs/InputNumber'
import VueFeather from 'vue-feather';

export default {
  install (app) {
    app.component('vue-chart', Chart)
    app.component('toggle-switch', ToggleSwitch)
    app.component('slider', Slider)
    app.component('input-ip', InputIP)
    app.component('input-number', InputNumber)
    app.component('schedule-chart', ScheduleChart)

    /* feather-icons */
    app.component(VueFeather.name, VueFeather);
  }
}
