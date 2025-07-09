import { createRouter, createWebHistory } from 'vue-router'

const routes = [
    {
        path: '/',
        name: 'Login',
        component: () => import('../views/Login.vue'),  // 懒加载
        meta: {
            requiresAuth: false
        }
    },
    {
        path: '/admin',
        name: 'Admin',
        component: () => import('../views/Admin.vue'), // 改为懒加载
        meta: {
            requiresAuth: true,
            role: 'admin'
        }
    },
    {
        path: '/teacher',
        name: 'Teacher',
        component: () => import('../views/teacher.vue'), // 改为懒加载
        meta: {
            requiresAuth: true,
            role: 'teacher'
        }
    },
    {
        path: '/student',
        name: 'Student',
        component: () => import('../views/Student.vue'), // 改为懒加载
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
    console.log("路由守卫 - 目标路径:", to.path);
    console.log("路由守卫 - 存储的用���数据:", savedUser);

    if (to.meta.requiresAuth) {
        if (savedUser) {
            try {
                const userData = JSON.parse(savedUser)
                const userRole = userData.role
                const requiredRole = to.meta.role
                
                console.log("路由守卫 - 用户角色:", userRole);
                console.log("路由守卫 - 需要角色:", requiredRole);

                if (userRole === requiredRole) {
                    console.log("路由守卫 - 角色匹配，允许访问");
                    next()
                } else {
                    // 角色不匹配，重定向到对应页面
                    console.log("路由守卫 - 角色不匹配，重定向到:", `/${userRole}`);
                    next(`/${userRole}`)
                }
            } catch (error) {
                // 本地存储数据损坏，清除并重定向到登录
                console.error("路由守卫 - 解析用户数据错误:", error);
                localStorage.removeItem('rememberedUser')
                next('/')
            }
        } else {
            // 未登录，重定向到登录页面
            console.log("路由守卫 - 未登���，重定向到登录页面");
            next('/')
        }
    } else {
        // 如果已登录用户访问登录页面，重定向到对应角色页面
        if (to.path === '/' && savedUser) {
            try {
                const userData = JSON.parse(savedUser)
                console.log("路由守卫 - 已登录用户访问登录页面，重定向��:", `/${userData.role}`);
                next(`/${userData.role}`)
            } catch (error) {
                console.error("路由守卫 - 解析已登录用户数据错误:", error);
                localStorage.removeItem('rememberedUser')
                next()
            }
        } else {
            console.log("路由守卫 - 允许访问非受保护页面");
            next()
        }
    }
    console.log("路由导航:", { from: from.path, to: to.path, savedUser: !!savedUser });
})

export default router