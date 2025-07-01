<template>
  <div class="container">
    <header class="header">
      <h2>学生管理系统</h2>
      <div class="header-controls">
        <input type="text" v-model="searchTerm" placeholder="搜索学生..." class="search-input">
        <button @click="showStudentModal()" class="btn btn-primary">添加学生</button>
        <button @click="importData" class="btn btn-secondary">导入数据</button>
        <button @click="exportData" class="btn btn-secondary">导出数据</button>
      </div>
    </header>

    <main class="main-content">
      <div class="students-container" v-if="filteredStudents.length > 0">
        <!-- 学生卡片 -->
        <div v-for="student in filteredStudents" :key="student.id" class="student-card" :class="{ 'expanded': expandedCardId === student.id }">
          <div class="student-card-header">
            <div class="student-basic-info">
              <h2 class="student-name">{{ student.name || '未知姓名' }}</h2>
              <div class="student-id">学号: {{ student.id || 0 }}</div>
              <div class="status-badge" :class="getStatusClass(student.status || 'Active')">{{ getStatusText(student.status || 'Active') }}</div>
            </div>
            <div class="student-actions">
              <button class="btn-icon edit" @click="editStudent(student)" title="编辑">✏️</button>
              <button class="btn-icon delete" @click="deleteStudent(student.id)" title="删除">🗑️</button>
            </div>
            <button class="expand-toggle" @click="toggleCard(student.id)">
              {{ expandedCardId === student.id ? '收起' : '展开' }}
            </button>
          </div>
          <!-- 详细信息 -->
          <div class="student-details" v-if="expandedCardId === student.id">
            <!-- 详细信息渲染 -->
            <div class="info-section">
              <h3>基本信息</h3>
              <div class="info-grid">
                <div class="info-item"><label>性别</label><span>{{ student.sex === 'Male' ? '男' : '女' }}</span></div>
                <div class="info-item"><label>年龄</label><span>{{ calculateAge(student.birthdate) }}岁</span></div>
                <div class="info-item"><label>生日</label><span>{{ student.birthdate?.year }}-{{ student.birthdate?.month.toString().padStart(2, '0') }}-{{ student.birthdate?.day.toString().padStart(2, '0') }}</span></div>
                <div class="info-item"><label>入学年份</label><span>{{ student.admissionYear }}</span></div>
                <div class="info-item"><label>专业</label><span>{{ student.major }}</span></div>
              </div>
            </div>
            <!-- ... 其他信息区域 ... -->
          </div>
        </div>
      </div>
      <div v-else class="no-students">
        <h3>暂无学生数据</h3>
        <p>点击"添加学生"按钮开始添加学生信息</p>
      </div>
    </main>
  </div>

  <!-- 学生信息模态框 -->
  <div class="modal" v-if="isModalVisible">
    <div class="modal-content">
      <div class="modal-header">
        <h2 id="modalTitle">{{ modalTitle }}</h2>
        <span @click="closeStudentModal" class="close">&times;</span>
      </div>
      <form class="student-form" @submit.prevent="saveStudent">
        <!-- 表单内容... -->
        <div class="form-section">
          <h3>基本信息</h3>
          <div class="form-grid">
            <div class="form-group"><label for="studentId">学号</label><input type="number" v-model="editableStudent.id" required></div>
            <div class="form-group"><label for="studentName">姓名</label><input type="text" v-model="editableStudent.name" required></div>
            <!-- ... 其他表单字段绑定到 editableStudent -->
          </div>
        </div>

        <div class="form-actions">
          <button type="submit" class="btn btn-primary">保存</button>
          <button type="button" @click="closeStudentModal" class="btn btn-secondary">取消</button>
        </div>
      </form>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue';

// 学生数据和状态
const students = ref([]);
const currentEditingId = ref(null);
const qtBridge = ref(null);
const searchTerm = ref('');
const expandedCardId = ref(null);

// 模态框状态
const isModalVisible = ref(false);
const modalTitle = ref('添加学生信息');
const editableStudent = ref({});

// 模拟的 Qt Bridge，用于在浏览器中测试
const createMockBridge = () => ({
  get_students: () => {
    console.log('MOCK: get_students called');
    // 返回一些示例数据
    return [
      { id: 1, name: '张三', status: 'Active', sex: 'Male', birthdate: { year: 2002, month: 5, day: 10 }, admissionYear: 2020, major: '计算机科学' },
      { id: 2, name: '李四', status: 'Leave', sex: 'Female', birthdate: { year: 2001, month: 8, day: 22 }, admissionYear: 2019, major: '物理学' },
    ];
  },
  add_student: (student) => console.log('MOCK: add_student', student),
  update_student: (student) => console.log('MOCK: update_student', student),
  delete_student: (id) => console.log('MOCK: delete_student', id),
  log_message: (msg) => console.log(`MOCK LOG: ${msg}`),
  show_notification: (title, msg) => alert(`${title}: ${msg}`),
  open_file_dialog: () => console.log('MOCK: open_file_dialog'),
  save_file_dialog: () => console.log('MOCK: save_file_dialog'),
});

// 计算属性，用于过滤学生
const filteredStudents = computed(() => {
  if (!searchTerm.value) {
    return students.value;
  }
  const lowerCaseSearch = searchTerm.value.toLowerCase();
  return students.value.filter(student =>
      (student.name && student.name.toLowerCase().includes(lowerCaseSearch)) ||
      (student.id && student.id.toString().includes(lowerCaseSearch)) ||
      (student.major && student.major.toLowerCase().includes(lowerCaseSearch))
  );
});


const waitForQtBridge = () => {
  return new Promise((resolve) => {
    // 检查是否在 Qt 环境中
    if (typeof qt !== 'undefined' && qt.webChannelTransport) {
      new QWebChannel(qt.webChannelTransport, (channel) => {
        qtBridge.value = channel.objects.qtBridge;
        console.log('Qt Bridge connected');
        resolve();
      });
    } else {
      console.warn('Qt Bridge not available, using mock data');
      qtBridge.value = createMockBridge();
      resolve();
    }
  });
};

const loadStudents = async () => {
  try {
    if (!qtBridge.value) return;
    const result = await qtBridge.value.get_students();
    if (Array.isArray(result)) {
      students.value = result;
    } else {
      students.value = [];
    }
    qtBridge.value.log_message(`Students loaded: ${students.value.length} items`);
  } catch (error) {
    console.error('Error loading students:', error);
    students.value = [];
    qtBridge.value.log_message('Error loading students: ' + error.message);
  }
};

const showStudentModal = (student = null) => {
  if (student) {
    modalTitle.value = '编辑学生信息';
    currentEditingId.value = student.id;
    // 深拷贝一个副本以避免直接修改原始数据
    editableStudent.value = JSON.parse(JSON.stringify(student));
  } else {
    modalTitle.value = '添加学生信息';
    currentEditingId.value = null;
    // 提供一个空对象模板
    editableStudent.value = { id: Date.now(), name: '', status: 'Active', birthdate: {}, contact: {}, address: {}, courses: [], familyMembers: [], scores: {} };
  }
  isModalVisible.value = true;
};

const closeStudentModal = () => {
  isModalVisible.value = false;
};

const saveStudent = async () => {
  // 省略了表单验证逻辑
  try {
    if (currentEditingId.value) {
      await qtBridge.value.update_student(editableStudent.value);
    } else {
      await qtBridge.value.add_student(editableStudent.value);
    }
    qtBridge.value.show_notification('成功', '学生信息已保存');
    closeStudentModal();
    await loadStudents(); // 重新加载数据以刷新列表
  } catch (error) {
    console.error('Error saving student:', error);
    qtBridge.value.log_message('Error saving student: ' + error.message);
  }
};

const editStudent = (student) => {
  showStudentModal(student);
};

const deleteStudent = async (studentId) => {
  if (confirm('确定要删除这个学生吗？')) {
    try {
      await qtBridge.value.delete_student(studentId);
      qtBridge.value.show_notification('成功', '学生已删除');
      await loadStudents();
    } catch (error) {
      console.error('Error deleting student:', error);
      qtBridge.value.log_message('Error deleting student: ' + error.message);
    }
  }
};

const toggleCard = (id) => {
  expandedCardId.value = expandedCardId.value === id ? null : id;
};

// 辅助函数
const getStatusClass = (status) => ({ 'status-active': status === 'Active', 'status-leave': status === 'Leave', 'status-graduated': status === 'Graduated' });
const getStatusText = (status) => ({ 'Active': '在读', 'Leave': '休学', 'Graduated': '毕业' }[status] || '在读');
const calculateAge = (birthdate) => {
  if (!birthdate || !birthdate.year) return 0;
  const today = new Date();
  const birth = new Date(birthdate.year, (birthdate.month || 1) - 1, birthdate.day || 1);
  let age = today.getFullYear() - birth.getFullYear();
  const m = today.getMonth() - birth.getMonth();
  if (m < 0 || (m === 0 && today.getDate() < birth.getDate())) {
    age--;
  }
  return age;
};

const importData = () => {
  qtBridge.value?.open_file_dialog('导入学生数据', 'JSON Files (*.json)');
};

const exportData = () => {
  qtBridge.value?.save_file_dialog('导出学生数据', 'JSON Files (*.json)');
};

onMounted(async () => {
  await waitForQtBridge();
  await loadStudents();
});
</script>

<style scoped>
/* 样式保持不变 */
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  min-height: 100vh;
  color: #333;
}

.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

/* Header */
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

.header h1 {
  color: #4f46e5;
  font-size: 28px;
  font-weight: 700;
}

.header-controls {
  display: flex;
  gap: 15px;
}

/* Buttons */
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

/* Main Content */
.main-content {
  display: flex;
  flex-direction: column;
  gap: 30px;
}

/* Students Container - 三列布局 */
.students-container {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 20px;
}

/* Student Card - 紧凑设计 */
.student-card {
  background: white;
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.08);
  transition: all 0.3s ease;
  cursor: pointer;
}

.student-card:hover {
  transform: translateY(-3px);
  box-shadow: 0 15px 35px rgba(0, 0, 0, 0.12);
}

.student-card.expanded {
  grid-column: 1 / -1; /* 展开时占满整行 */
}

/* Student Card Header - 显示基本信息 */
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

.student-major {
  font-size: 14px;
  opacity: 0.8;
  margin-bottom: 12px;
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

/* 展开/折叠按钮 */
.expand-toggle {
  position: absolute;
  top: 15px;
  right: 15px;
  background: rgba(255, 255, 255, 0.2);
  border: none;
  color: white;
  width: 30px;
  height: 30px;
  border-radius: 50%;
  cursor: pointer;
  font-size: 14px;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.3s ease;
}

.expand-toggle:hover {
  background: rgba(255, 255, 255, 0.3);
}

.expand-toggle.expanded {
  transform: rotate(180deg);
}

/* 操作按钮区域 */
.student-actions {
  position: absolute;
  bottom: 15px;
  right: 15px;
  display: flex;
  gap: 8px;
  opacity: 0;
  transition: opacity 0.3s ease;
}

.student-card:hover .student-actions {
  opacity: 1;
}

.student-actions .btn-icon {
  color: white;
  background: rgba(255, 255, 255, 0.2);
  width: 32px;
  height: 32px;
  border-radius: 50%;
  padding: 0;
  display: flex;
  align-items: center;
  justify-content: center;
}

.student-actions .btn-icon:hover {
  background: rgba(255, 255, 255, 0.3);
}

/* Student Details - 折叠的详细信息 */
.student-details {
  display: none;
  padding: 25px;
  background: #f8fafc;
  border-top: 1px solid #e2e8f0;
}

.student-card.expanded .student-details {
  display: block;
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

.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
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

/* Courses */
.courses-container {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.course-tag {
  background: #e0e7ff;
  color: #3730a3;
  padding: 6px 12px;
  border-radius: 15px;
  font-size: 12px;
  font-weight: 600;
  border: 1px solid #c7d2fe;
  transition: all 0.3s ease;
}

.course-tag:hover {
  background: #c7d2fe;
  transform: translateY(-1px);
}

/* Scores */
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

.average-score {
  text-align: center;
  padding: 15px;
  background: linear-gradient(135deg, #10b981 0%, #059669 100%);
  color: white;
  border-radius: 8px;
  font-size: 16px;
}

/* Family Members */
.family-members {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.family-member {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  background: #fef3c7;
  border-radius: 8px;
  border-left: 4px solid #f59e0b;
}

.member-info {
  display: flex;
  flex-direction: column;
  gap: 3px;
}

.member-name {
  font-weight: 600;
  color: #1f2937;
  font-size: 14px;
}

.member-relationship {
  font-size: 12px;
  color: #92400e;
  background: white;
  padding: 2px 6px;
  border-radius: 8px;
  display: inline-block;
  width: fit-content;
}

.member-contact {
  color: #92400e;
  font-weight: 500;
  font-size: 13px;
}

/* 响应式设计 */
@media (max-width: 1024px) {
  .students-container {
    grid-template-columns: repeat(2, 1fr);
  }
}

@media (max-width: 768px) {
  .container {
    padding: 15px;
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

  .student-card.expanded {
    grid-column: 1;
  }

  .student-details {
    padding: 20px;
  }

  .info-grid {
    grid-template-columns: 1fr;
  }

  .score-item {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }

  .family-member {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
}

@media (max-width: 480px) {
  .header h1 {
    font-size: 24px;
  }

  .student-name {
    font-size: 16px;
  }

  .student-card-header {
    padding: 15px;
  }

  .header-controls {
    flex-direction: column;
    width: 100%;
  }

  .btn {
    width: 100%;
    text-align: center;
  }
}

/* 动画效果 */
@keyframes fadeInUp {
  from {
    opacity: 0;
    transform: translateY(20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.student-card {
  animation: fadeInUp 0.4s ease-out;
}

.student-details {
  animation: fadeInUp 0.3s ease-out;
}

/* 滚动条样式 */
::-webkit-scrollbar {
  width: 8px;
}

::-webkit-scrollbar-track {
  background: #f1f5f9;
}

::-webkit-scrollbar-thumb {
  background: #cbd5e1;
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: #94a3b8;
}

/* 模态框样式 */
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
  max-width: 800px;
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

/* 表单样式 */
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

.search-input {
  padding: 10px 15px;
  border: 2px solid rgba(255, 255, 255, 0.3);
  border-radius: 25px;
  background: rgba(255, 255, 255, 0.9);
  font-size: 14px;
  width: 250px;
  transition: all 0.3s ease;
}

.search-input:focus {
  outline: none;
  border-color: #4f46e5;
  background: white;
  box-shadow: 0 0 0 3px rgba(79, 70, 229, 0.1);
}

/* 动态字段容器 */
.dynamic-container {
  margin-bottom: 15px;
}

.course-field,
.family-field,
.score-field {
  display: grid;
  gap: 10px;
  padding: 15px;
  background: #f8fafc;
  border-radius: 8px;
  margin-bottom: 10px;
  border-left: 4px solid #4f46e5;
}

.course-field {
  grid-template-columns: 1fr auto;
  align-items: center;
}

.family-field {
  grid-template-columns: 1fr 1fr 1fr auto;
  align-items: center;
}

.score-field {
  grid-template-columns: 1fr auto auto auto;
  align-items: center;
}

.course-field input,
.family-field input,
.score-field input {
  padding: 8px 10px;
  border: 1px solid #d1d5db;
  border-radius: 6px;
  font-size: 14px;
}

.remove-btn {
  background: #ef4444;
  color: white;
  border: none;
  padding: 8px 12px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 12px;
  transition: background 0.3s ease;
}

.remove-btn:hover {
  background: #dc2626;
}

.btn-outline {
  background: transparent;
  color: #4f46e5;
  border: 2px solid #4f46e5;
}

.btn-outline:hover {
  background: #4f46e5;
  color: white;
}

/* 表单操作按钮 */
.form-actions {
  display: flex;
  gap: 15px;
  justify-content: flex-end;
  padding-top: 20px;
  border-top: 1px solid #e5e7eb;
}

/* 无数据状态 */
.no-students {
  text-align: center;
  padding: 60px 20px;
  color: #6b7280;
  grid-column: 1 / -1;
}

.no-students h3 {
  font-size: 24px;
  margin-bottom: 10px;
  color: #374151;
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

/* 状态徽章样式 */
.status-active {
  background: rgba(34, 197, 94, 0.3);
  color: #16a34a;
  border: 1px solid rgba(34, 197, 94, 0.4);
}

.status-leave {
  background: rgba(245, 158, 11, 0.3);
  color: #d97706;
  border: 1px solid rgba(245, 158, 11, 0.4);
}

.status-graduated {
  background: rgba(34, 197, 94, 0.3);
  color: #16a34a;
  border: 1px solid rgba(34, 197, 94, 0.4);
}
</style>