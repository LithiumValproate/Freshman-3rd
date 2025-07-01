import { createApp } from 'vue'
import App from './App.vue'
import router from './router' // 确保这是指向 router/index.js 的正确路径

// 创建应用实例
const app = createApp(App)

// 关键步骤：注册路由器
app.use(router)

// 挂载应用
app.mount('#app')