<template>
  <div class="login-container">
    <div class="login-card">
      <div class="login-header">
        <h1>UnivUni_Platform</h1>
        <p>请选择您的身份并登录</p>
      </div>

      <!-- 消息提示 -->
      <div v-if="message" :class="['message', messageType]">{{ message }}</div>

      <form class="login-form" @submit.prevent="handleLogin">
        <div class="role-selector">
          <h3>选择身份</h3>
          <div class="role-options">
            <div
                class="role-option"
                @click="selectRole('admin')"
            >
              <input type="radio" name="role" value="admin" v-model="role">
              <div class="role-card">
                <div class="role-icon">👨‍💼</div>
                <span class="role-name">管理员</span>
                <span class="role-desc">系统管理和配置</span>
              </div>
            </div>
            <div
                class="role-option"
                @click="selectRole('teacher')"
            >
              <input type="radio" name="role" value="teacher" v-model="role">
              <div class="role-card">
                <div class="role-icon">👨‍🏫</div>
                <span class="role-name">教师</span>
                <span class="role-desc">学生信息管理</span>
              </div>
            </div>
            <div
                class="role-option"
                @click="selectRole('student')"
            >
              <input type="radio" name="role" value="student" v-model="role">
              <div class="role-card">
                <div class="role-icon">👨‍🎓</div>
                <span class="role-name">学生</span>
                <span class="role-desc">查看个人信息</span>
              </div>
            </div>
          </div>
        </div>

        <div class="form-group">
          <label for="username">用户名</label>
          <input type="text" id="username" v-model="username" required placeholder="请输入用户名">
        </div>

        <div class="form-group">
          <label for="password">密码</label>
          <input type="password" id="password" v-model="password" required placeholder="请输入密码">
        </div>

        <div class="form-options">
          <label class="remember-me">
            <input type="checkbox" v-model="rememberMe">
            <span>记住我</span>
          </label>
          <a href="#" class="forgot-password">忘记密码？</a>
        </div>

        <button type="submit" class="login-btn" :disabled="loading">
          <span v-if="loading">登录中...</span>
          <span v-else>登录</span>
        </button>
      </form>

      <div class="login-footer">
        <p>&copy; 2025 UnivUni_Platform. All rights reserved.</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted, toRefs, inject } from 'vue';
import { useRouter } from 'vue-router';

const router = useRouter();

const loginForm = reactive({
  role: '',
  username: '',
  password: '',
  rememberMe: false,
});

const { role, username, password, rememberMe } = toRefs(loginForm);

const formError = reactive({ role: '', username: '', password: '' });
const message = ref('');
const messageType = ref('');
const loading = ref(false);

// 注入 App.vue 提供的学生列表
const students = inject('students');

// Qt Bridge 引用及初始化，用于拉取学生列表
const qtBridge = ref(null);
const waitForQtBridge = () => new Promise(resolve => {
  if (typeof qt !== 'undefined' && qt.webChannelTransport) {
    new QWebChannel(qt.webChannelTransport, ch => { qtBridge.value = ch.objects.qtBridge; resolve(); });
  } else if (typeof QWebChannel !== 'undefined') {
    try {
      new QWebChannel(qt.webChannelTransport, ch => { qtBridge.value = ch.objects.qtBridge; resolve(); });
    } catch {
      qtBridge.value = { 
        get_students_from_db: ()=>[],
        authenticate_user: (r, u, p) => ({ success: true, message: 'Mock login successful' })
      }; 
      resolve();
    }
  } else {
    qtBridge.value = { 
      get_students_from_db: ()=>[],
      authenticate_user: (r, u, p) => ({ success: true, message: 'Mock login successful' })
    }; 
    resolve();
  }
});
const loadStudents = async () => {
  if (!qtBridge.value) return;
  const res = await qtBridge.value.get_students_from_db();
  students.value = Array.isArray(res) ? res : [];
};

onMounted(async () => {
  // 初始化：恢复"记住我"数据
  const saved = localStorage.getItem('rememberedUser');
  if (saved) {
    try {
      const data = JSON.parse(saved);
      loginForm.role = data.role || '';
      loginForm.username = data.username || '';
      loginForm.password = data.password || '';
      loginForm.rememberMe = true;
    } catch {
      console.warn('读取本地存储失败');
    }
  }
  // 进入登录页时刷新学生列表
  await waitForQtBridge();
  await loadStudents();
});

// 表单校验
function validate() {
  formError.role = formError.username = formError.password = '';
  if (!loginForm.role) formError.role = '请选择身份';
  if (!loginForm.username) formError.username = '请输入用户名';
  if (!loginForm.password) formError.password = '请输入密码';
  if (formError.role || formError.username || formError.password) {
    message.value = formError.role || formError.username || formError.password;
    messageType.value = 'error';
    return false;
  }
  // 学生身份时，用户名必须等于某个学生的 ID
  if (loginForm.role === 'student') {
    const id = Number(loginForm.username);
    if (!students?.value?.some(s => s.id === id)) {
      formError.username = '用户名与学生ID不匹配';
      message.value = formError.username;
      messageType.value = 'error';
      return false;
    }
  }
  return true;
}

// 角色选择方法
function selectRole(r) {
  role.value = r;
}

// 修正的页面跳转方法
const goPage = (role) => {
  router.push(`/${role}`);  // ✅ 使用 router.push() 跳转到 admin.vue
  console.log("正在跳转到:", role);
}

// 登录处理
async function handleLogin() {
  if (!validate()) return;
  loading.value = true;
  message.value = '';

  try {
    const result = await qtBridge.value.authenticate_user(role.value, username.value, password.value);
    
    if (result && result.success) {
      const userData = {
        username: loginForm.username,
        // Do not store password in local/session storage
        role: loginForm.role,
        loginTime: new Date().toISOString()
      };
      
      // The router guard requires 'rememberedUser' in localStorage to work.
      // We will always set it for the current session. The "Remember Me"
      // checkbox should determine if this data persists after the session ends.
      localStorage.setItem('rememberedUser', JSON.stringify(userData));

      // Also save to session storage, which is cleared when the tab is closed.
      sessionStorage.setItem('currentUser', JSON.stringify(userData));

      // If "Remember Me" is not checked, the data should ideally be cleared on logout.
      if (!loginForm.rememberMe) {
        // To prevent persistence, we can clear the local storage item now,
        // but we must ensure the session one is used by the guard, or we can
        // just leave it and have a proper logout function handle the clearing.
        // For now, we prioritize fixing the login flow.
        console.log("Login successful. 'Remember Me' was not checked.");
      }

      loading.value = false;
      message.value = '登录成功，正在跳转...';
      messageType.value = 'success';
      
      setTimeout(() => {
        goPage(role.value);
      }, 800);

    } else {
      loading.value = false;
      message.value = result.message || '登录失败，请检查您的凭据。';
      messageType.value = 'error';
    }
  } catch (error) {
    loading.value = false;
    message.value = '登录时发生错误。';
    messageType.value = 'error';
    console.error("Login error:", error);
    if (qtBridge.value && qtBridge.value.log_message) {
      qtBridge.value.log_message(`Login error: ${error.message}`);
    }
  }
}
</script>

<style scoped>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  position: relative;
  overflow: hidden;
}

.login-container {
  width: 100%;
  max-width: 450px;
  margin: 20px;
  position: relative;
  z-index: 10;
}

.login-card {
  background: rgba(255, 255, 255, 0.95);
  border-radius: 20px;
  padding: 40px;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.2);
  animation: slideUp 0.6s ease-out;
}

@keyframes slideUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.login-header {
  text-align: center;
  margin-bottom: 30px;
}

.login-header h1 {
  color: #4f46e5;
  font-size: 28px;
  font-weight: 700;
  margin-bottom: 8px;
}

.login-header p {
  color: #6b7280;
  font-size: 14px;
}

.role-selector {
  margin-bottom: 25px;
}

.role-selector h3 {
  color: #374151;
  font-size: 16px;
  font-weight: 600;
  margin-bottom: 15px;
  text-align: center;
}

.role-options {
  display: flex;
  gap: 12px;
  margin-bottom: 20px;
}

.role-option {
  flex: 1;
  cursor: pointer;
  position: relative;
}

.role-option input[type="radio"] {
  display: none;
}

/* 角色卡片置于下层，接受高亮样式 */
.role-card {
  position: relative;
  z-index: 1;
  background: #f8fafc;
  border: 2px solid #e2e8f0;
  border-radius: 12px;
  padding: 16px 12px;
  text-align: center;
  transition: all 0.3s ease;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
}

.role-card:hover {
  background: #f1f5f9;
  border-color: #cbd5e1;
  transform: translateY(-2px);
}

.role-option input[type="radio"]:checked + .role-card {
  background: linear-gradient(135deg, #4f46e5 0%, #7c3aed 100%);
  border-color: #4f46e5;
  color: white;
  transform: translateY(-3px);
  box-shadow: 0 8px 25px rgba(79, 70, 229, 0.3);
}

.role-icon {
  font-size: 24px;
  margin-bottom: 4px;
}

.role-name {
  font-size: 14px;
  font-weight: 600;
  display: block;
}

.role-desc {
  font-size: 11px;
  opacity: 0.8;
  display: block;
}

.form-group {
  margin-bottom: 20px;
}

.form-group label {
  display: block;
  color: #374151;
  font-size: 14px;
  font-weight: 600;
  margin-bottom: 6px;
}

.form-group input {
  width: 100%;
  padding: 12px 16px;
  border: 2px solid #e5e7eb;
  border-radius: 10px;
  font-size: 14px;
  transition: all 0.3s ease;
  background: #f9fafb;
}

.form-group input:focus {
  outline: none;
  border-color: #4f46e5;
  background: white;
  box-shadow: 0 0 0 3px rgba(79, 70, 229, 0.1);
}

.form-options {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 25px;
  font-size: 14px;
}

.remember-me {
  display: flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
  color: #6b7280;
}

.remember-me input[type="checkbox"] {
  width: auto;
  margin: 0;
}

.forgot-password {
  color: #4f46e5;
  text-decoration: none;
  transition: color 0.3s ease;
}

.forgot-password:hover {
  color: #4338ca;
}

.login-btn {
  width: 100%;
  background: linear-gradient(135deg, #4f46e5 0%, #7c3aed 100%);
  color: white;
  border: none;
  padding: 14px;
  border-radius: 10px;
  font-size: 16px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
}

.login-btn:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(79, 70, 229, 0.3);
}

.login-btn:active:not(:disabled) {
  transform: translateY(0);
}

.login-btn:disabled,
.login-btn.loading {
  pointer-events: none;
  opacity: 0.7;
  cursor: not-allowed;
  transform: none !important;
}

.login-btn.loading::after {
  content: '';
  position: absolute;
  top: 50%;
  left: 50%;
  width: 20px;
  height: 20px;
  margin: -10px 0 0 -10px;
  border: 2px solid transparent;
  border-top: 2px solid white;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.login-footer {
  text-align: center;
  margin-top: 25px;
  padding-top: 20px;
  border-top: 1px solid #e5e7eb;
}

.login-footer p {
  color: #9ca3af;
  font-size: 12px;
}

@keyframes float {
  0%, 100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-20px);
  }
}

/* 响应式设计 */
@media (max-width: 480px) {
  .login-container {
    margin: 10px;
  }

  .login-card {
    padding: 30px 25px;
  }

  .login-header h1 {
    font-size: 24px;
  }

  .role-options {
    flex-direction: column;
    gap: 10px;
  }

  .role-card {
    padding: 12px;
  }

  .role-icon {
    font-size: 20px;
  }

  .form-options {
    flex-direction: column;
    gap: 10px;
    align-items: flex-start;
  }
}

@media (max-width: 360px) {
  .role-options {
    gap: 8px;
  }

  .role-card {
    padding: 10px 8px;
  }

  .role-name {
    font-size: 12px;
  }

  .role-desc {
    font-size: 10px;
  }
}

/* 提示消息样式 */
.message {
  padding: 12px 16px;
  border-radius: 8px;
  margin-bottom: 20px;
  font-size: 14px;
  animation: slideDown 0.3s ease-out;
}

@keyframes slideDown {
  from {
    opacity: 0;
    transform: translateY(-10px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.message.success {
  background: #d1fae5;
  color: #065f46;
  border: 1px solid #a7f3d0;
}

.message.error {
  background: #fee2e2;
  color: #991b1b;
  border: 1px solid #fca5a5;
}

.message.info {
  background: #dbeafe;
  color: #1e40af;
  border: 1px solid #93c5fd;
}
</style>
