import { http } from '@/service/http'
import { store } from '@/service/store'

export const authService = user => new Promise((resolve, reject) => {
  http({ url: '/auth', data: user, method: 'POST' })
    .then(resp => {
      const token = resp.data.token
      http.defaults.headers.common.Authorization = token
      store.setAuthenticated(token)
      resolve(resp)
    })
    .catch(err => {
      /* if the request fails, remove any possible user token if possible */
      localStorage.removeItem('user-token')
      reject(err)
    })
})
