import { createRouter, createWebHistory } from 'vue-router'
import Admin from '../views/admin.vue'

const routes = [
    {
        path: '/admin',
        name: 'Admin',
        component: Admin,
        meta: {
            requiresAuth: true // 如果需要权限控制
        }
    }
]

const router = createRouter({
    history: createWebHistory(),
    routes
})

// 路由守卫（可选）
router.beforeEach((to, from, next) => {
    if (to.meta.requiresAuth) {
        // 这里可以添加权限验证逻辑
        const isAdmin = checkAdminPermission() // 自定义权限检查函数
        if (isAdmin) {
            next()
        } else {
            next('/')
        }
    } else {
        next()
    }
})

function checkAdminPermission() {
    // 实际项目中这里应该检查用户权限
    // 比如从 localStorage、Vuex 或 API 获取用户信息
    return true // 临时返回 true
}

export default router