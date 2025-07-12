process.env.VUE_APP_VERSION = require('./package.json').version
const { defineConfig } = require('@vue/cli-service')
const webpack = require('webpack');

module.exports = defineConfig({
  configureWebpack: {
    plugins: [
      new webpack.DefinePlugin({
        // Vue CLI is in maintenance mode, and probably won't merge my PR to fix this in their tooling
        // https://github.com/vuejs/vue-cli/pull/7443
        __VUE_PROD_HYDRATION_MISMATCH_DETAILS__: 'false',
      })
    ],
  },
});

if (process.env.NODE_ENV === 'production') {
  module.exports = {
    pluginOptions: {
      'style-resources-loader': {
        preProcessor: 'scss',
        patterns: []
      }
    },

    chainWebpack: (config) => {
      config.optimization.delete('splitChunks')
    },

    // baseUrl: '/app',
    // assetsDir: undefined,
    // runtimeCompiler: undefined,
    productionSourceMap: false,
    // parallel: undefined,
    css: { extract: false },
    filenameHashing: false,

    // outputDir: '../data'
    // outputDir: undefined,

    // lintOnSave: undefined
  }
}
