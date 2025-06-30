class LoginManager {
    constructor() {
        this.init();
    }

    init() {
        this.setupEventListeners();
        this.loadRememberedUser();
    }

    setupEventListeners() {
        const loginForm = document.getElementById('loginForm');
        const roleOptions = document.querySelectorAll('input[name="role"]');
        const rememberCheckbox = document.getElementById('rememberMe');

        // 表单提交事件
        loginForm.addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleLogin();
        });

        // 角色选择变化事件
        roleOptions.forEach(option => {
            option.addEventListener('change', () => {
                this.updateRoleSelection();
            });
        });

        // 记住我功能
        rememberCheckbox.addEventListener('change', (e) => {
            if (!e.target.checked) {
                this.clearRememberedUser();
            }
        });
    }

    updateRoleSelection() {
        const selectedRole = document.querySelector('input[name="role"]:checked').value;
        console.log('Selected role:', selectedRole);
        
        // 可以根据角色显示不同的提示信息
        this.showRoleInfo(selectedRole);
    }

    showRoleInfo(role) {
        // 移除之前的提示信息
        const existingMessage = document.querySelector('.role-info-message');
        if (existingMessage) {
            existingMessage.remove();
        }

        // 根据角色显示相应的提示
        const roleInfoMap = {
            admin: '管理员可以管理系统设置、用户权限和所有数据',
            teacher: '教师可以管理学生信息、成绩录入和课程安排',
            student: '学生可以查看个人信息、成绩和课程表'
        };

        if (roleInfoMap[role]) {
            const message = document.createElement('div');
            message.className = 'message info role-info-message';
            message.textContent = roleInfoMap[role];
            
            const roleSelector = document.querySelector('.role-selector');
            roleSelector.appendChild(message);
            
            // 3秒后自动消失
            setTimeout(() => {
                if (message && message.parentNode) {
                    message.remove();
                }
            }, 3000);
        }
    }

    async handleLogin() {
        const username = document.getElementById('username').value.trim();
        const password = document.getElementById('password').value.trim();
        const selectedRole = document.querySelector('input[name="role"]:checked').value;
        const rememberMe = document.getElementById('rememberMe').checked;

        // 基本验证
        if (!username || !password) {
            this.showMessage('请填写用户名和密码', 'error');
            return;
        }

        // 显示加载状态
        const loginBtn = document.querySelector('.login-btn');
        loginBtn.classList.add('loading');
        loginBtn.textContent = '登录中...';

        try {
            // 模拟登录延迟
            await this.delay(1000);

            // 验证登录（目前接受任何用户名密码）
            const loginResult = await this.validateLogin(username, password, selectedRole);

            if (loginResult.success) {
                // 保存登录信息
                if (rememberMe) {
                    this.saveRememberedUser(username, selectedRole);
                }

                // 保存登录状态
                this.saveLoginSession(username, selectedRole);

                // 显示成功消息
                this.showMessage('登录成功，正在跳转...', 'success');

                // 延迟跳转
                setTimeout(() => {
                    this.redirectToRolePage(selectedRole);
                }, 1000);

            } else {
                this.showMessage(loginResult.message || '用户名或密码错误', 'error');
            }

        } catch (error) {
            console.error('Login error:', error);
            this.showMessage('登录过程中出现错误，请稍后重试', 'error');
        } finally {
            // 恢复按钮状态
            loginBtn.classList.remove('loading');
            loginBtn.textContent = '登录';
        }
    }

    async validateLogin(username, password, role) {
        // 目前接受任何用户名和密码
        // 后续可以在这里添加真实的验证逻辑
        
        // 模拟网络请求
        return new Promise((resolve) => {
            setTimeout(() => {
                // 这里可以添加具体的验证逻辑
                // 例如：检查用户名格式、密码强度等
                
                resolve({
                    success: true,
                    user: {
                        username: username,
                        role: role,
                        displayName: username
                    }
                });
            }, 500);
        });
    }

    redirectToRolePage(role) {
        const rolePages = {
            admin: 'admin.html',
            teacher: 'teacher.html',
            student: 'student.html'
        };

        const targetPage = rolePages[role];
        if (targetPage) {
            // 检查页面是否存在（可选）
            this.checkPageExists(targetPage).then(exists => {
                if (exists || true) { // 暂时跳过存在性检查
                    window.location.href = targetPage;
                } else {
                    // 如果页面不存在，跳转到默认页面或显示错误
                    this.showMessage(`${role} 页面暂未创建，跳转到默认页面`, 'info');
                    setTimeout(() => {
                        window.location.href = 'index.html'; // 跳转到主页
                    }, 2000);
                }
            });
        } else {
            this.showMessage('未知的用户角色', 'error');
        }
    }

    async checkPageExists(url) {
        try {
            const response = await fetch(url, { method: 'HEAD' });
            return response.ok;
        } catch (error) {
            return false;
        }
    }

    saveLoginSession(username, role) {
        const sessionData = {
            username: username,
            role: role,
            loginTime: new Date().toISOString(),
            expiresAt: new Date(Date.now() + 24 * 60 * 60 * 1000).toISOString() // 24小时后过期
        };
        
        localStorage.setItem('userSession', JSON.stringify(sessionData));
        sessionStorage.setItem('currentUser', JSON.stringify({
            username: username,
            role: role
        }));
    }

    saveRememberedUser(username, role) {
        const rememberData = {
            username: username,
            role: role,
            savedAt: new Date().toISOString()
        };
        localStorage.setItem('rememberedUser', JSON.stringify(rememberData));
    }

    loadRememberedUser() {
        try {
            const remembered = localStorage.getItem('rememberedUser');
            if (remembered) {
                const data = JSON.parse(remembered);
                
                // 填充用户名
                document.getElementById('username').value = data.username;
                
                // 选择角色
                const roleOption = document.querySelector(`input[name="role"][value="${data.role}"]`);
                if (roleOption) {
                    roleOption.checked = true;
                    this.updateRoleSelection();
                }
                
                // 勾选记住我
                document.getElementById('rememberMe').checked = true;
            }
        } catch (error) {
            console.error('Error loading remembered user:', error);
        }
    }

    clearRememberedUser() {
        localStorage.removeItem('rememberedUser');
    }

    showMessage(text, type = 'info') {
        // 优化：避免频繁插入消息导致卡顿
        const existingMessage = document.querySelector('.login-message');
        if (existingMessage) {
            // 只更新内容和类型，不重复创建
            existingMessage.className = `message ${type} login-message`;
            existingMessage.textContent = text;
            // 重置消失计时器
            if (existingMessage._timeout) clearTimeout(existingMessage._timeout);
            existingMessage.style.opacity = '1';
            existingMessage.style.transform = 'none';
            existingMessage._timeout = setTimeout(() => {
                if (existingMessage && existingMessage.parentNode) {
                    existingMessage.style.opacity = '0';
                    existingMessage.style.transform = 'translateY(-10px)';
                    setTimeout(() => {
                        if (existingMessage && existingMessage.parentNode) {
                            existingMessage.remove();
                        }
                    }, 300);
                }
            }, 5000);
            return;
        }

        // 创建新消息
        const message = document.createElement('div');
        message.className = `message ${type} login-message`;
        message.textContent = text;

        // 插入到表单前面
        const form = document.getElementById('loginForm');
        form.parentNode.insertBefore(message, form);

        // 5秒后自动消失
        setTimeout(() => {
            if (message && message.parentNode) {
                message.style.opacity = '0';
                message.style.transform = 'translateY(-10px)';
                setTimeout(() => {
                    if (message && message.parentNode) {
                        message.remove();
                    }
                }, 300);
            }
        }, 5000);
    }

    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    // 静态方法：检查用户是否已登录
    static checkLoginStatus() {
        try {
            const session = localStorage.getItem('userSession');
            if (session) {
                const data = JSON.parse(session);
                const now = new Date();
                const expiresAt = new Date(data.expiresAt);
                
                if (now < expiresAt) {
                    return {
                        isLoggedIn: true,
                        user: {
                            username: data.username,
                            role: data.role
                        }
                    };
                } else {
                    // 会话已过期
                    localStorage.removeItem('userSession');
                    sessionStorage.removeItem('currentUser');
                }
            }
        } catch (error) {
            console.error('Error checking login status:', error);
        }
        
        return { isLoggedIn: false };
    }

    // 静态方法：登出
    static logout() {
        localStorage.removeItem('userSession');
        sessionStorage.removeItem('currentUser');
        window.location.href = 'login.html';
    }
}

// 页面加载完成后初始化登录管理器
document.addEventListener('DOMContentLoaded', () => {
    console.log('Login page loaded, initializing LoginManager...');
    
    // 自动聚焦到用户名输入框，兼容QtWeb和浏览器
    const usernameInput = document.getElementById('username');
    if (usernameInput) {
        usernameInput.focus();
    }

    // 检查是否已经登录
    const loginStatus = LoginManager.checkLoginStatus();
    if (loginStatus.isLoggedIn) {
        // 如果已经登录，询问是否要重新登录
        const shouldRelogin = confirm(`您已经以 ${loginStatus.user.role} 身份登录，是否要重新登录？`);
        if (!shouldRelogin) {
            // 跳转到对应的角色页面
            const rolePages = {
                admin: 'admin.html',
                teacher: 'teacher.html',
                student: 'student.html'
            };
            const targetPage = rolePages[loginStatus.user.role];
            if (targetPage) {
                window.location.href = targetPage;
                return;
            }
        }
    }
    
    // 初始化登录管理器
    window.loginManager = new LoginManager();
});

// 导出到全局作用域，方便其他页面使用
window.LoginManager = LoginManager;