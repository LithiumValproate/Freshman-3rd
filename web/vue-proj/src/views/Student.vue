<template>
  <div class="container">
    <header class="header">
      <h2>个人信息中心</h2>
      <div class="header-controls">
        <button @click="showEditModal" class="btn btn-primary">编辑信息</button>
        <button @click="logout" class="btn btn-secondary" style="margin-left: 10px;">退出登录</button>
      </div>
    </header>

    <main class="main-content">
      <div class="students-container">
        <div class="student-card expanded">
          <div class="student-card-header">
            <div class="student-basic-info">
              <h2 class="student-name">{{ student.name || '未知姓名' }}</h2>
              <div class="student-id">学号: {{ student.id || 0 }}</div>
              <div class="status-badge status-active">在读</div>
            </div>
          </div>
          <div class="student-details" style="display:block;">
            <div class="info-section">
              <h3>基本信息</h3>
              <div class="basic-info-grid">
                <div class="info-item"><label>性别</label><span>{{ student.sex === 'Male' ? '男' : '女' }}</span></div>
                <div class="info-item"><label>入学年份</label><span>{{ student.enrollYear }}</span></div>
                <div class="info-item"><label>专业</label><span>{{ student.major }}</span></div>
                <div class="info-item"><label>班级</label><span>{{ student.class_ }}</span></div>
              </div>
              <div class="contact-info-grid">
                <div class="info-item contact-item">
                  <label>电话</label>
                  <span style="display: flex; align-items: center;">
                    {{ student.contactInfo.phone || '无' }}
                    <button
                        v-if="student.contactInfo.phone"
                        class="btn-icon"
                        style="margin-left: auto; margin-right: 1em;"
                        @click="copyToClipboard(student.contactInfo.phone, '电话')"
                        title="复制电话"
                    >📋</button>
                  </span>
                </div>
                <div class="info-item">
                  <label>邮箱</label>
                  <span style="display: flex; align-items: center;">
                    {{ student.contactInfo.email || '无' }}
                    <button
                        v-if="student.contactInfo.email"
                        class="btn-icon"
                        style="margin-left: auto; margin-right: 1em;"
                        @click="copyToClipboard(student.contactInfo.email, '邮箱')"
                        title="复制邮箱"
                    >📋</button>
                  </span>
                </div>
              </div>
              <div class="contact-info-grid" style="margin-top:20px;">
                <div class="info-item">
                  <label>省份</label>
                  <span>{{ student.address.province }}</span>
                </div>
                <div class="info-item">
                  <label>城市</label>
                  <span>{{ student.address.city }}</span>
                </div>
              </div>
            </div>
            <div class="info-section">
              <h3>课程成绩</h3>
              <div class="scores-container" v-if="student.courseScore && student.courseScore.length">
                <div class="score-item" v-for="c in student.courseScore" :key="c.course">
                  <span class="course-name">{{ c.course }}</span>
                  <div class="score-details">
                    <span class="score">{{ c.score }}</span>
                    <span class="gpa">GPA: {{ c.gpa }}</span>
                  </div>
                </div>
              </div>
              <div class="no-data" v-else>暂无成绩数据</div>
            </div>
            <div class="info-section">
              <h3>本周课表</h3>
              <div class="no-data">课表功能待实现</div>
            </div>
          </div>
        </div>
      </div>
    </main>

    <!-- 编辑信息模态框 -->
    <div class="modal" v-if="isModalVisible">
      <div class="modal-content">
        <div class="modal-header">
          <h2>编辑个人信息</h2>
          <span @click="closeEditModal" class="close">&times;</span>
        </div>
        <form class="student-form" @submit.prevent="saveStudent">
          <div class="form-section">
            <h3>基本信息</h3>
            <div class="form-grid">
              <div class="form-group">
                <label>学号</label>
                <input type="number" v-model="editableStudent.id" required>
              </div>
              <div class="form-group">
                <label>姓名</label>
                <input type="text" v-model="editableStudent.name" required>
              </div>
              <div class="form-group">
                <label>性别</label>
                <select v-model="editableStudent.sex" required>
                  <option value="">请选择</option>
                  <option value="Male">男</option>
                  <option value="Female">女</option>
                </select>
              </div>
              <div class="form-group">
                <label>入学年份</label>
                <input type="number" v-model="editableStudent.enrollYear" min="1900" max="2030" required>
              </div>
              <div class="form-group">
                <label>专业</label>
                <input type="text" v-model="editableStudent.major" required>
              </div>
              <div class="form-group">
                <label>班级</label>
                <input type="number" v-model="editableStudent.class_" required>
              </div>
            </div>
          </div>
          <div class="form-section">
            <h3>联系方式</h3>
            <div class="form-grid">
              <div class="form-group">
                <label>电话号码</label>
                <input type="tel" v-model="editableStudent.contactInfo.phone" placeholder="请输入手机号">
              </div>
              <div class="form-group">
                <label>邮箱地址</label>
                <input type="email" v-model="editableStudent.contactInfo.email" placeholder="example@email.com">
              </div>
            </div>
          </div>
          <div class="form-section">
            <h3>地址信息</h3>
            <div class="form-grid">
              <div class="form-group">
                <label>省份</label>
                <input type="text" v-model="editableStudent.address.province" placeholder="如：北京市">
              </div>
              <div class="form-group">
                <label>城市</label>
                <input type="text" v-model="editableStudent.address.city" placeholder="如：海淀区">
              </div>
            </div>
          </div>
          <div class="form-actions">
            <button type="submit" class="btn btn-primary">保存</button>
            <button type="button" @click="closeEditModal" class="btn btn-secondary">取消</button>
          </div>
        </form>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';

// Mock data based on the C++ structs
const student = reactive({
  id: 20230101,
  name: 'Alice',
  sex: 'Female',
  major: 'Computer Science',
  class_: 1,
  enrollYear: 2023,
  contactInfo: {
    phone: '123-456-7890',
    email: 'alice@university.edu',
  },
  address: {
    province: 'California',
    city: 'Los Angeles',
  },
  courseScore: [
    { course: 'Data Structures', score: 92, gpa: 4.0 },
    { course: 'Operating Systems', score: 88, gpa: 3.7 },
    { course: 'Computer Networks', score: 95, gpa: 4.0 },
    { course: 'Database Systems', score: 85, gpa: 3.3 },
  ],
});

const isModalVisible = ref(false);
const editableStudent = ref({});

const showEditModal = () => {
  editableStudent.value = JSON.parse(JSON.stringify(student));
  isModalVisible.value = true;
};
const closeEditModal = () => {
  isModalVisible.value = false;
};
const saveStudent = () => {
  Object.assign(student, JSON.parse(JSON.stringify(editableStudent.value)));
  closeEditModal();
};

const router = useRouter();
const logout = () => {
  localStorage.removeItem('rememberedUser');
  router.replace('/login');
};

const copyToClipboard = (text, label = '') => {
  if (!text) return;
  navigator.clipboard.writeText(text);
};
</script>

<style scoped>
.container {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}
.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: rgba(255, 255, 255, 0.95);
  padding: 20px 30px;
  border-radius: 15px;
  margin-bottom: 30px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
  backdrop-filter: blur(10px);
}
.header h2 {
  color: #4f46e5;
  font-size: 28px;
  font-weight: 700;
}
.header-controls {
  display: flex;
  gap: 15px;
}
.btn {
  padding: 12px 24px;
  border: none;
  border-radius: 8px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  text-decoration: none;
  display: inline-block;
}
.btn-primary {
  background: #4f46e5;
  color: white;
}
.btn-primary:hover {
  background: #4338ca;
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(79, 70, 229, 0.3);
}
.btn-secondary {
  background: #6b7280;
  color: white;
}
.btn-secondary:hover {
  background: #4b5563;
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(107, 114, 128, 0.3);
}
.btn-icon {
  background: none;
  border: none;
  font-size: 16px;
  cursor: pointer;
  padding: 8px 12px;
  border-radius: 6px;
  transition: all 0.3s ease;
  color: #4f46e5;
}
.btn-icon:hover {
  background: rgba(79, 70, 229, 0.1);
}
.main-content {
  display: flex;
  flex-direction: column;
  gap: 30px;
}
.students-container {
  display: grid;
  grid-template-columns: 1fr;
  gap: 20px;
}
.student-card {
  background: white;
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.08);
  transition: all 0.3s ease;
}
.student-card.expanded {
  grid-column: 1 / -1;
}
.student-card-header {
  background: linear-gradient(135deg, #4f46e5 0%, #7c3aed 100%);
  color: white;
  padding: 20px;
  position: relative;
}
.student-basic-info {
  text-align: center;
}
.student-name {
  font-size: 18px;
  font-weight: 700;
  margin-bottom: 8px;
}
.student-id {
  font-size: 14px;
  opacity: 0.9;
  margin-bottom: 8px;
}
.status-badge {
  display: inline-block;
  padding: 4px 12px;
  border-radius: 15px;
  font-size: 11px;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  background: rgba(255, 255, 255, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.3);
}
.status-active {
  background: rgba(34, 197, 94, 0.3);
  color: #16a34a;
  border: 1px solid rgba(34, 197, 94, 0.4);
}
.student-details {
  padding: 25px;
  background: #f8fafc;
  border-top: 1px solid #e2e8f0;
}
.info-section {
  margin-bottom: 25px;
}
.info-section:last-child {
  margin-bottom: 0;
}
.info-section h3 {
  font-size: 16px;
  font-weight: 700;
  color: #1f2937;
  margin-bottom: 15px;
  padding-bottom: 8px;
  border-bottom: 2px solid #e5e7eb;
}
.basic-info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 15px;
}
.info-item {
  display: flex;
  flex-direction: column;
  gap: 4px;
}
.info-item label {
  font-size: 12px;
  font-weight: 600;
  color: #6b7280;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}
.info-item span {
  font-size: 14px;
  color: #1f2937;
  font-weight: 500;
}
.contact-info-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 15px;
  margin-top: 20px;
}
.scores-container {
  display: flex;
  flex-direction: column;
  gap: 12px;
  margin-bottom: 15px;
}
.score-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  background: white;
  border-radius: 8px;
  border-left: 4px solid #4f46e5;
  transition: all 0.3s ease;
}
.score-item:hover {
  background: #f1f5f9;
  transform: translateX(3px);
}
.course-name {
  font-weight: 600;
  color: #1f2937;
  font-size: 14px;
}
.score-details {
  display: flex;
  gap: 12px;
  align-items: center;
}
.score {
  font-size: 16px;
  font-weight: 700;
  color: #059669;
}
.gpa {
  font-size: 12px;
  color: #6b7280;
  background: #f3f4f6;
  padding: 3px 6px;
  border-radius: 4px;
}
.no-data {
  color: #9ca3af;
  font-style: italic;
  padding: 20px;
  text-align: center;
  background: #f9fafb;
  border-radius: 8px;
  border: 2px dashed #d1d5db;
}
.modal {
  display: block;
  position: fixed;
  z-index: 1000;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  backdrop-filter: blur(5px);
}
.modal-content {
  background-color: white;
  margin: 2% auto;
  padding: 0;
  border-radius: 15px;
  width: 90%;
  max-width: 600px;
  max-height: 90vh;
  overflow-y: auto;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
  animation: modalSlideIn 0.3s ease-out;
}
@keyframes modalSlideIn {
  from {
    opacity: 0;
    transform: translateY(-50px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}
.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 25px 30px;
  background: linear-gradient(135deg, #4f46e5 0%, #7c3aed 100%);
  color: white;
  border-radius: 15px 15px 0 0;
}
.modal-header h2 {
  margin: 0;
  font-size: 24px;
  font-weight: 700;
}
.close {
  color: white;
  font-size: 28px;
  font-weight: bold;
  cursor: pointer;
  transition: opacity 0.3s ease;
  line-height: 1;
}
.close:hover {
  opacity: 0.7;
}
.student-form {
  padding: 30px;
}
.form-section {
  margin-bottom: 35px;
}
.form-section:last-of-type {
  margin-bottom: 20px;
}
.form-section h3 {
  font-size: 18px;
  font-weight: 700;
  color: #1f2937;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 2px solid #e5e7eb;
}
.form-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 20px;
}
.form-group {
  display: flex;
  flex-direction: column;
}
.form-group label {
  font-size: 14px;
  font-weight: 600;
  color: #374151;
  margin-bottom: 5px;
}
.form-group input,
.form-group select {
  padding: 10px 12px;
  border: 2px solid #e5e7eb;
  border-radius: 8px;
  font-size: 14px;
  transition: border-color 0.3s ease, box-shadow 0.3s ease;
}
.form-group input:focus,
.form-group select:focus {
  outline: none;
  border-color: #4f46e5;
  box-shadow: 0 0 0 3px rgba(79, 70, 229, 0.1);
}
.form-actions {
  display: flex;
  gap: 15px;
  justify-content: flex-end;
  padding-top: 20px;
  border-top: 1px solid #e5e7eb;
}
@media (max-width: 768px) {
  .container {
    padding: 10px;
  }
  .header {
    flex-direction: column;
    gap: 20px;
    text-align: center;
  }
  .students-container {
    grid-template-columns: 1fr;
    gap: 15px;
  }
  .student-details {
    padding: 15px;
  }
  .basic-info-grid {
    grid-template-columns: 1fr;
  }
}
</style>
