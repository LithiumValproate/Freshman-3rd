import { createApp } from 'vue'
import App from './App.vue'
import router from './router' // 确保这是指向 router/index.js 的正确路径
import ElementPlus from 'element-plus' // Add this line
import 'element-plus/dist/index.css' // Add this line

// 创建应用实例
const app = createApp(App)

// 关键步骤：注册路由器
app.use(router)
app.use(ElementPlus) // Add this line

// 挂载应用
app.mount('#app')