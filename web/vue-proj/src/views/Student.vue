<template>
  <div class="container">
    <header class="header">
      <h2>个人信息中心</h2>
      <div class="header-controls">
        <button @click="exportToJson" class="btn btn-primary">导出到JSON</button>
        <button @click="logout" class="btn btn-secondary" style="margin-left: 10px;">退出登录</button>
      </div>
    </header>

    <main class="main-content">
      <div class="students-container">
        <div class="student-card">
          <!-- 学生基本信息 -->
          <div class="student-card-header">
            <div class="student-basic-info">
              <h2 class="student-name">{{ student.name || '未知姓名' }}</h2>
              <div class="student-id">学号: {{ student.id || 0 }}</div>
              <div class="status-badge status-active">在读</div>
            </div>
          </div>

          <div class="student-details">
            <!-- 课表 - 日历形式 -->
            <div class="info-section">
              <div class="section-header">
                <h3>课表</h3>
              </div>
              <div class="calendar-schedule" v-if="student.courses && student.courses.length">
                <div class="calendar-grid">
                  <div class="time-header">时间</div>
                  <div class="day-header" v-for="day in weekDays" :key="day.key">{{ day.label }}</div>
                  <template v-for="hour in timeSlots" :key="hour">
                    <div class="time-slot">{{ hour }}:00</div>
                    <div
                      v-for="day in weekDays"
                      :key="`${day.key}-${hour}`"
                      class="schedule-cell"
                      :class="{ 'has-course': getCourseAtTime(day.key, hour) }"
                    >
                      <div v-if="getCourseAtTime(day.key, hour)" class="course-block">
                        <div class="course-name">{{ getCourseAtTime(day.key, hour).courseName }}</div>
                        <div class="course-location">{{ getCourseAtTime(day.key, hour).location }}</div>
                      </div>
                    </div>
                  </template>
                </div>
              </div>
              <div class="no-data" v-else>暂无课表数据</div>
            </div>

            <!-- 成绩 - 柱状图 -->
            <div class="info-section">
              <div class="section-header">
                <h3>课程成绩</h3>
              </div>
              <div class="chart-container" v-if="student.courseScore && student.courseScore.length">
                <canvas ref="scoreChart"></canvas>
              </div>
              <div class="no-data" v-else>暂无成绩数据</div>
            </div>

            <!-- 个人基本信息 -->
            <div class="info-section">
              <div class="section-header">
                <h3>基本信息</h3>
              </div>
              <div class="basic-info-grid">
                <div class="info-item"><label>性别</label><span>{{ student.sex === 'Male' ? '男' : '女' }}</span></div>
                <div class="info-item"><label>年龄</label><span>{{ calculateAge(student.birthdate) }}岁</span></div>
                <div class="info-item"><label>入学年份</label><span>{{ student.enrollYear }}</span></div>
                <div class="info-item"><label>专业</label><span>{{ student.major }}</span></div>
                <div class="info-item"><label>班级</label><span>{{ student.class_ }}</span></div>
              </div>
            </div>

            <!-- 联系方式 -->
            <div class="info-section">
              <div class="section-header">
                <h3>联系方式</h3>
                <button @click="editSection('contact')" class="btn btn-icon">✏️</button>
              </div>
              <div class="contact-info-grid">
                <div class="info-item">
                  <label>电话</label>
                  <span>{{ student.contactInfo.phone || '无' }}</span>
                </div>
                <div class="info-item">
                  <label>邮箱</label>
                  <span>{{ student.contactInfo.email || '无' }}</span>
                </div>
              </div>
            </div>

            <!-- 地址信息 -->
            <div class="info-section">
              <div class="section-header">
                <h3>地址信息</h3>
                <button @click="editSection('address')" class="btn btn-icon">✏️</button>
              </div>
              <div class="contact-info-grid">
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

            <!-- 家庭成员 -->
            <div class="info-section">
              <div class="section-header">
                <h3>家庭成员</h3>
                <button @click="editSection('family')" class="btn btn-icon">✏️</button>
              </div>
              <div v-if="student.familyMembers && student.familyMembers.length" class="family-members">
                <div class="family-member" v-for="(member, idx) in student.familyMembers" :key="idx">
                  <div class="member-info">
                    <span class="member-name">{{ member.name }}</span>
                    <span class="member-relationship">{{ member.relationship }}</span>
                    <span class="member-contact">电话: {{ member.contactInfo.phone || '无' }}</span>
                    <span class="member-contact">邮箱: {{ member.contactInfo.email || '无' }}</span>
                  </div>
                </div>
              </div>
              <div class="no-data" v-else>暂无家庭成员信息</div>
            </div>
          </div>
        </div>
      </div>
    </main>

    <!-- 分栏编辑模态框 -->
    <div class="modal" v-if="isModalVisible">
      <div class="modal-content">
        <div class="modal-header">
          <h2>编辑{{ getSectionTitle(editingSection) }}</h2>
          <span @click="closeEditModal" class="close">&times;</span>
        </div>
        <form class="section-form" @submit.prevent="saveSectionData">
          <!-- 联系方式编辑 -->
          <div v-if="editingSection === 'contact'" class="form-section">
            <div class="form-grid">
              <div class="form-group">
                <label>电话号码</label>
                <input type="tel" v-model="editData.phone" placeholder="请输入手机号">
              </div>
              <div class="form-group">
                <label>邮箱地址</label>
                <input type="email" v-model="editData.email" placeholder="example@email.com">
              </div>
            </div>
          </div>

          <!-- 地址编辑 -->
          <div v-if="editingSection === 'address'" class="form-section">
            <div class="form-grid">
              <div class="form-group">
                <label>省份</label>
                <input type="text" v-model="editData.province" placeholder="如：北京市">
              </div>
              <div class="form-group">
                <label>城市</label>
                <input type="text" v-model="editData.city" placeholder="如：海淀区">
              </div>
            </div>
          </div>

          <!-- 家庭成员编辑 -->
          <div v-if="editingSection === 'family'" class="form-section">
            <div v-for="(member, index) in editData" :key="index" class="family-member-item">
              <div class="form-grid">
                <div class="form-group">
                  <label>姓名</label>
                  <input type="text" v-model="member.name" placeholder="家庭成员姓名">
                </div>
                <div class="form-group">
                  <label>关系</label>
                  <select v-model="member.relationship">
                    <option value="">请选择</option>
                    <option value="父亲">父亲</option>
                    <option value="母亲">母亲</option>
                    <option value="兄弟">兄弟</option>
                    <option value="姐妹">姐妹</option>
                    <option value="其他">其他</option>
                  </select>
                </div>
                <div class="form-group">
                  <label>联系电话</label>
                  <input type="tel" v-model="member.contactInfo.phone" placeholder="联系电话">
                </div>
                <div class="form-group">
                  <label>邮箱</label>
                  <input type="email" v-model="member.contactInfo.email" placeholder="邮箱地址">
                </div>
                <div class="form-group">
                  <button type="button" @click="removeFamilyMember(index)" class="btn btn-danger">删除</button>
                </div>
              </div>
            </div>
            <button type="button" @click="addFamilyMember" class="btn btn-secondary">添加家庭成员</button>
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

'''<script setup>
import { ref, reactive, onMounted, nextTick } from 'vue';
import { useRouter } from 'vue-router';

const router = useRouter();

const student = reactive({
  id: 0,
  name: '',
  sex: '',
  major: '',
  class_: 0,
  enrollYear: 0,
  birthdate: { year: 0, month: 0, day: 0 },
  contactInfo: {
    phone: '',
    email: '',
  },
  address: {
    province: '',
    city: '',
  },
  familyMembers: [],
  courses: [],
  courseScore: []
});

const isModalVisible = ref(false);
const editingSection = ref('');
const editData = ref({});
const scoreChart = ref(null);
const qtBridge = ref(null);

const waitForQtBridge = () => {
  return new Promise((resolve) => {
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

const createMockBridge = () => {
  let mockStudentData = {
    id: 20230101,
    name: 'Alice (Mock)',
    sex: 'Female',
    major: 'Computer Science',
    class_: 1,
    enrollYear: 2023,
    birthdate: { year: 2004, month: 9, day: 1 },
    contactInfo: {
      phone: '123-456-7890',
      email: 'alice@university.edu',
    },
    address: {
      province: 'California',
      city: 'Los Angeles',
    },
    familyMembers: [
      {
        name: '张三',
        relationship: '父亲',
        contactInfo: { phone: '13800000000', email: 'zhangsan@example.com' }
      },
    ],
    courses: [
      { courseID: 1, courseName: '数据结构', location: 'A101', schedule: [{ day: 'Monday', startTime: { hour: 8 }, endTime: { hour: 10 } }] },
      { courseID: 2, courseName: '操作系统', location: 'B202', schedule: [{ day: 'Wednesday', startTime: { hour: 10 }, endTime: { hour: 12 } }] }
    ],
    courseScore: [
      { course: '数据结构', score: 92, gpa: 4.0 },
      { course: '操作系统', score: 88, gpa: 3.7 }
    ]
  };
  return {
    get_student_by_id: (id) => {
      console.log(`MOCK: get_student_by_id(${id})`);
      return mockStudentData;
    },
    update_student: (updatedStudent) => {
      console.log('MOCK: update_student', updatedStudent);
      mockStudentData = Object.assign(mockStudentData, updatedStudent);
      return true;
    },
    log_message: (msg) => console.log(`MOCK LOG: ${msg}`),
  };
};


const loadStudentData = async () => {
  const userData = JSON.parse(localStorage.getItem('rememberedUser'));
  if (userData && userData.role === 'student') {
    try {
      if (!qtBridge.value) {
        await waitForQtBridge();
      }
      const studentId = parseInt(userData.username, 10);
      const result = await qtBridge.value.get_student_by_id(studentId);
      if (result) {
        Object.assign(student, result);
        // Ensure nested objects are reactive
        if (!student.contactInfo) student.contactInfo = {};
        if (!student.address) student.address = {};
        if (!student.familyMembers) student.familyMembers = [];
        if (!student.courses) student.courses = [];
        if (!student.courseScore) student.courseScore = [];
      }
      nextTick(() => {
        createScoreChart();
      });
    } catch (error) {
      console.error('Error loading student data:', error);
      if (qtBridge.value) {
        qtBridge.value.log_message('Error loading student data: ' + error.message);
      }
    }
  } else {
    router.replace('/login');
  }
};


const weekDays = [
  { key: 'Monday', label: '周一' },
  { key: 'Tuesday', label: '周二' },
  { key: 'Wednesday', label: '周三' },
  { key: 'Thursday', label: '周四' },
  { key: 'Friday', label: '周五' },
  { key: 'Saturday', label: '周六' },
  { key: 'Sunday', label: '周日' }
];

const timeSlots = [8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20];

const getCourseAtTime = (day, hour) => {
  if (!student.courses) return null;
  return student.courses.find(course => {
    return course.schedule.some(slot =>
      slot.day === day &&
      slot.startTime.hour <= hour &&
      slot.endTime.hour > hour
    );
  });
};

const editSection = (section) => {
  editingSection.value = section;
  switch(section) {
    case 'contact':
      editData.value = { ...student.contactInfo };
      break;
    case 'address':
      editData.value = { ...student.address };
      break;
    case 'family':
      editData.value = JSON.parse(JSON.stringify(student.familyMembers || []));
      break;
  }
  isModalVisible.value = true;
};

const getSectionTitle = (section) => {
  const titles = {
    contact: '联系方式',
    address: '地址信息',
    family: '家庭成员'
  };
  return titles[section] || '';
};

const saveSectionData = async () => {
  const updatedStudent = JSON.parse(JSON.stringify(student));
  switch(editingSection.value) {
    case 'contact':
       updatedStudent.contactInfo = { ...editData.value };
      break;
    case 'address':
       updatedStudent.address = { ...editData.value };
      break;
    case 'family':
      updatedStudent.familyMembers = JSON.parse(JSON.stringify(editData.value));
      break;
  }

  try {
    if (qtBridge.value) {
      await qtBridge.value.update_student_in_qjson(updatedStudent);
      Object.assign(student, updatedStudent); // Update local state after successful save
    }
  } catch (error) {
    console.error('Error saving student data:', error);
  }

  closeEditModal();
};

const closeEditModal = () => {
  isModalVisible.value = false;
  editingSection.value = '';
  editData.value = {};
};

const addFamilyMember = () => {
  if (!Array.isArray(editData.value)) {
     editData.value = [];
  }
  editData.value.push({
    name: '',
    relationship: '',
    contactInfo: { phone: '', email: '' }
  });
};

const removeFamilyMember = (idx) => {
  editData.value.splice(idx, 1);
};

const createScoreChart = () => {
  if (!scoreChart.value || !student.courseScore?.length) return;

  const ctx = scoreChart.value.getContext('2d');
  const labels = student.courseScore.map(item => item.course);
  const scores = student.courseScore.map(item => item.score);

  if (window.studentChart) {
    window.studentChart.destroy();
  }

  window.studentChart = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: labels,
      datasets: [{
        label: '成绩',
        data: scores,
        backgroundColor: [
          '#4f46e5',
          '#7c3aed',
          '#06b6d4',
          '#10b981',
          '#f59e0b',
          '#ef4444'
        ],
        borderColor: [
          '#4338ca',
          '#6d28d9',
          '#0891b2',
          '#059669',
          '#d97706',
          '#dc2626'
        ],
        borderWidth: 2,
        borderRadius: 6
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: {
          display: false
        }
      },
      scales: {
        y: {
          beginAtZero: true,
          max: 100,
          grid: {
            color: '#e5e7eb'
          }
        },
        x: {
          grid: {
            display: false
          }
        }
      }
    }
  });
};

const exportToJson = () => {
  const jsonData = JSON.stringify(student, null, 2);
  const blob = new Blob([jsonData], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `student_${student.id}_data.json`;
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
};

const calculateAge = (birthdate) => {
  if (!birthdate || !birthdate.year) return 0;
  const today = new Date();
  const birth = new Date(birthdate.year, birthdate.month - 1, birthdate.day);
  let age = today.getFullYear() - birth.getFullYear();
  const monthDiff = today.getMonth() - birth.getMonth();
  if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
    age--;
  }
  return age;
};

const logout = () => {
  localStorage.removeItem('rememberedUser');
  router.replace('/login');
};

onMounted(async () => {
  await waitForQtBridge();
  await loadStudentData();
  nextTick(() => {
    const script = document.createElement('script');
    script.src = 'https://cdn.jsdelivr.net/npm/chart.js';
    script.onload = () => {
      createScoreChart();
    };
    document.head.appendChild(script);
  });
});
</script>''

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
.family-members {
  display: flex;
  flex-direction: column;
  gap: 12px;
}
.family-member {
  display: flex;
  justify-content: flex-start;
  align-items: flex-start;
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
.family-member-item {
  background: #f8fafc;
  border: 2px solid #e5e7eb;
  border-radius: 12px;
  padding: 20px;
  margin-bottom: 15px;
  transition: all 0.3s ease;
}
.family-member-item:hover {
  border-color: #4f46e5;
  box-shadow: 0 4px 12px rgba(79, 70, 229, 0.1);
}
.family-member-item .form-grid {
  grid-template-columns: 1fr 1fr 1fr 1fr auto;
  align-items: end;
  gap: 6px;
}
.btn-danger {
  background: #ef4444;
  color: white;
  border: none;
  padding: 10px 15px;
  border-radius: 8px;
  cursor: pointer;
  font-size: 14px;
  font-weight: 600;
  transition: all 0.3s ease;
  min-width: 60px;
}
.btn-danger:hover {
  background: #dc2626;
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(239, 68, 68, 0.3);
}
.section-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 15px;
}

.section-header h3 {
  font-size: 16px;
  font-weight: 700;
  color: #1f2937;
  margin: 0;
  padding-bottom: 8px;
  border-bottom: 2px solid #e5e7eb;
}

.calendar-schedule {
  background: white;
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.05);
}

.calendar-grid {
  display: grid;
  grid-template-columns: 80px repeat(7, 1fr);
  gap: 1px;
  background: #e5e7eb;
}

.time-header, .day-header {
  background: #4f46e5;
  color: white;
  padding: 12px 8px;
  text-align: center;
  font-weight: 600;
  font-size: 14px;
}

.time-slot {
  background: #f8fafc;
  padding: 8px;
  text-align: center;
  font-size: 12px;
  font-weight: 500;
  color: #6b7280;
  border-right: 2px solid #e5e7eb;
}

.schedule-cell {
  background: white;
  min-height: 60px;
  position: relative;
  border: 1px solid transparent;
}

.schedule-cell.has-course {
  background: #eff6ff;
  border-color: #3b82f6;
}

.course-block {
  padding: 4px 6px;
  background: linear-gradient(135deg, #4f46e5, #7c3aed);
  color: white;
  border-radius: 4px;
  margin: 2px;
  text-align: center;
  font-size: 11px;
}

.course-name {
  font-weight: 600;
  margin-bottom: 2px;
}

.course-location {
  font-size: 10px;
  opacity: 0.9;
}

.chart-container {
  height: 300px;
  background: white;
  border-radius: 12px;
  padding: 20px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.05);
}

.section-form {
  max-width: 100%;
}

.form-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 15px;
  margin-bottom: 20px;
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

.form-group input, .form-group select {
  padding: 10px 12px;
  border: 1px solid #d1d5db;
  border-radius: 8px;
  font-size: 14px;
  transition: border-color 0.3s ease;
}

.form-group input:focus, .form-group select:focus {
  outline: none;
  border-color: #4f46e5;
  box-shadow: 0 0 0 3px rgba(79, 70, 229, 0.1);
}

.form-actions {
  display: flex;
  gap: 10px;
  justify-content: flex-end;
  margin-top: 20px;
  padding-top: 20px;
  border-top: 1px solid #e5e7eb;
}

/* 模态框样式 */
.modal {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.modal-content {
  background: white;
  border-radius: 12px;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
  max-width: 600px;
  width: 90%;
  max-height: 80vh;
  overflow-y: auto;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 20px 25px;
  border-bottom: 1px solid #e5e7eb;
  background: #f8fafc;
  border-radius: 12px 12px 0 0;
}

.modal-header h2 {
  font-size: 18px;
  font-weight: 700;
  color: #1f2937;
  margin: 0;
}

.close {
  font-size: 24px;
  font-weight: bold;
  color: #6b7280;
  cursor: pointer;
  width: 30px;
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 50%;
  transition: all 0.3s ease;
}

.close:hover {
  background: #e5e7eb;
  color: #374151;
}

.section-form {
  padding: 25px;
}

.form-section {
  margin-bottom: 20px;
}

@media (max-width: 768px) {
  .calendar-grid {
    grid-template-columns: 60px repeat(7, 1fr);
    font-size: 12px;
  }

  .time-header, .day-header {
    padding: 8px 4px;
    font-size: 12px;
  }

  .course-block {
    font-size: 10px;
    padding: 2px 4px;
  }

  .chart-container {
    height: 250px;
    padding: 15px;
  }
}
</style>
