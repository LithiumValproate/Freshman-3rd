import { createRouter, createWebHistory } from 'vue-router'
import Admin from '../views/Admin.vue'
import Teacher from '../views/teacher.vue'  // 需要创建
import Student from '../views/student.vue'  // 需要创建

const routes = [
    {
        path: '/',
        name: 'Login',
        component: () => import('../App.vue'),  // App.vue 就是登录页面
        meta: {
            requiresAuth: false
        }
    },
    {
        path: '/admin',
        name: 'Admin',
        component: Admin,
        meta: {
            requiresAuth: true,
            role: 'admin'
        }
    },
    {
        path: '/teacher',
        name: 'Teacher',
        component: Teacher,
        meta: {
            requiresAuth: true,
            role: 'teacher'
        }
    },
    {
        path: '/student',
        name: 'Student',
        component: Student,
        meta: {
            requiresAuth: true,
            role: 'student'
        }
    },
    {
        // 404 页面重定向到登录
        path: '/:pathMatch(.*)*',
        redirect: '/'
    }
]

const router = createRouter({
    history: createWebHistory(),
    routes
})

// 改进的路由守卫
router.beforeEach((to, from, next) => {
    const savedUser = localStorage.getItem('rememberedUser')

    if (to.meta.requiresAuth) {
        if (savedUser) {
            try {
                const userData = JSON.parse(savedUser)
                const userRole = userData.role
                const requiredRole = to.meta.role

                if (userRole === requiredRole) {
                    next()
                } else {
                    // 角色不匹配，重定向到对应页面
                    next(`/${userRole}`)
                }
            } catch (error) {
                // 本地存储数据损坏，清除并重定向到登录
                localStorage.removeItem('rememberedUser')
                next('/')
            }
        } else {
            // 未登录，重定向到登录页面
            next('/')
        }
    } else {
        // 如果已登录用户访问登录页面，重定向到对应角色页面
        if (to.path === '/' && savedUser) {
            try {
                const userData = JSON.parse(savedUser)
                next(`/${userData.role}`)
            } catch (error) {
                localStorage.removeItem('rememberedUser')
                next()
            }
        } else {
            next()
        }
    }
})

export default router