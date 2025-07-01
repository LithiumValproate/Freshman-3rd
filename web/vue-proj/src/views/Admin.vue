<template>
  <div class="container">
    <header class="header">
      <h2>学生管理系统</h2>
      <div class="header-controls">
        <input type="text" id="searchInput" placeholder="搜索学生..." class="search-input">
        <button id="addStudentBtn" class="btn btn-primary">添加学生</button>
        <button id="importBtn" class="btn btn-secondary">导入数据</button>
        <button id="exportBtn" class="btn btn-secondary">导出数据</button>
      </div>
    </header>

    <main class="main-content">
      <div id="studentsContainer" class="students-container">
        <!-- 学生列表将在这里动态生成 -->
      </div>
    </main>
  </div>

  <!-- 学生信息模态框 -->
  <div id="studentModal" class="modal">
    <div class="modal-content">
      <div class="modal-header">
        <h2 id="modalTitle">添加学生信息</h2>
        <span id="closeModal" class="close">&times;</span>
      </div>
      <form id="studentForm" class="student-form">
        <div class="form-section">
          <h3>基本信息</h3>
          <div class="form-grid">
            <div class="form-group">
              <label for="studentId">学号</label>
              <input type="number" id="studentId" required>
            </div>
            <div class="form-group">
              <label for="studentName">姓名</label>
              <input type="text" id="studentName" required>
            </div>
            <div class="form-group">
              <label for="studentSex">性别</label>
              <select id="studentSex" required>
                <option value="Male">男</option>
                <option value="Female">女</option>
              </select>
            </div>
            <div class="form-group">
              <label for="birthYear">出生年</label>
              <input type="number" id="birthYear" min="1900" max="2030" required>
            </div>
            <div class="form-group">
              <label for="birthMonth">出生月</label>
              <input type="number" id="birthMonth" min="1" max="12" required>
            </div>
            <div class="form-group">
              <label for="birthDay">出生日</label>
              <input type="number" id="birthDay" min="1" max="31" required>
            </div>
            <div class="form-group">
              <label for="admissionYear">入学年份</label>
              <input type="number" id="admissionYear" required>
            </div>
            <div class="form-group">
              <label for="major">专业</label>
              <input type="text" id="major" required>
            </div>
            <div class="form-group">
              <label for="status">状态</label>
              <select id="status" required>
                <option value="Active">在读</option>
                <option value="Leave">休学</option>
                <option value="Graduated">毕业</option>
              </select>
            </div>
          </div>
        </div>

        <div class="form-section">
          <h3>联系信息</h3>
          <div class="form-grid">
            <div class="form-group">
              <label for="phone">电话</label>
              <input type="tel" id="phone" required>
            </div>
            <div class="form-group">
              <label for="email">邮箱</label>
              <input type="email" id="email">
            </div>
            <div class="form-group">
              <label for="province">省份</label>
              <input type="text" id="province">
            </div>
            <div class="form-group">
              <label for="city">城市</label>
              <input type="text" id="city">
            </div>
          </div>
        </div>

        <div class="form-section">
          <h3>课程信息</h3>
          <div id="coursesContainer" class="dynamic-container"></div>
          <button type="button" id="addCourseBtn" class="btn btn-outline">添加课程</button>
        </div>

        <div class="form-section">
          <h3>成绩信息</h3>
          <div id="scoresContainer" class="dynamic-container"></div>
          <button type="button" id="addScoreBtn" class="btn btn-outline">添加成绩</button>
        </div>

        <div class="form-section">
          <h3>家庭成员</h3>
          <div id="familyContainer" class="dynamic-container"></div>
          <button type="button" id="addFamilyBtn" class="btn btn-outline">添加家庭成员</button>
        </div>

        <div class="form-actions">
          <button type="button" id="saveStudentBtn" class="btn btn-primary">保存</button>
          <button type="button" onclick="studentManager.closeModal()" class="btn btn-secondary">取消</button>
        </div>
      </form>
    </div>
  </div>
</template>

<script setup>
let studentsData = {
  students: [],
  currentEditingId: null,
  qtBridge: null
};

async function initStudentManager() {
  await waitForQtBridge();
  setupEventListeners();
  await loadStudents();
  renderStudents();
}

function waitForQtBridge() {
  return new Promise((resolve) => {
    if (typeof qt !== 'undefined' && qt.webChannelTransport) {
      new QWebChannel(qt.webChannelTransport, (channel) => {
        studentsData.qtBridge = channel.objects.qtBridge;
        console.log('Qt Bridge connected');
        resolve();
      });
    } else {
      console.warn('Qt Bridge not available, using mock data');
      studentsData.qtBridge = createMockBridge ? createMockBridge() : {};
      resolve();
    }
  });
}

function setupEventListeners() {
  // 添加学生按钮
  const addBtn = document.getElementById('addStudentBtn');
  if (addBtn) {
    addBtn.addEventListener('click', () => {
      showStudentModal();
    });
  }

  // 导入数据按钮
  const importBtn = document.getElementById('importBtn');
  if (importBtn) {
    importBtn.addEventListener('click', () => {
      importData();
    });
  }

  // 导出数据按钮
  const exportBtn = document.getElementById('exportBtn');
  if (exportBtn) {
    exportBtn.addEventListener('click', () => {
      exportData();
    });
  }

  // 搜索功能
  const searchInput = document.getElementById('searchInput');
  if (searchInput) {
    searchInput.addEventListener('input', (e) => {
      filterStudents(e.target.value);
    });
  }

  // 模态框关闭
  const closeModal = document.getElementById('closeModal');
  if (closeModal) {
    closeModal.addEventListener('click', () => {
      closeModal();
    });
  }

  // 保存学生信息
  const saveBtn = document.getElementById('saveStudentBtn');
  if (saveBtn) {
    saveBtn.addEventListener('click', () => {
      saveStudent();
    });
  }

  // 添加课程
  const addCourseBtn = document.getElementById('addCourseBtn');
  if (addCourseBtn) {
    addCourseBtn.addEventListener('click', () => {
      addCourseField();
    });
  }

  // 添加家庭成员
  const addFamilyBtn = document.getElementById('addFamilyBtn');
  if (addFamilyBtn) {
    addFamilyBtn.addEventListener('click', () => {
      addFamilyMemberField();
    });
  }

  // 添加成绩
  const addScoreBtn = document.getElementById('addScoreBtn');
  if (addScoreBtn) {
    addScoreBtn.addEventListener('click', () => {
      addScoreField();
    });
  }
}

async function loadStudents() {
  try {
    console.log('Loading students from backend...');
    const result = studentsData.qtBridge.get_students();
    console.log('Raw result from Qt:', result);

    // 确保结果是数组格式
    if (Array.isArray(result)) {
      studentsData.students = result;
    } else if (result && typeof result === 'object') {
      // 如果返回的是QJsonArray，需要转换
      if (result.length !== undefined) {
        // 类数组对象，转换为真正的数组
        studentsData.students = Array.from(result);
      } else {
        // 可能是单个对象，包装成数组
        studentsData.students = [result];
      }
    } else {
      // 如果没有数据或数据格式不正确，初始化为空数组
      console.warn('Invalid data format from backend, initializing empty array');
      studentsData.students = [];
    }

    console.log('Students loaded:', studentsData.students.length, 'items');
    studentsData.qtBridge.log_message(`Students loaded: ${studentsData.students.length} items`);
  } catch (error) {
    console.error('Error loading students:', error);
    studentsData.students = []; // 确保在错误情况下也是数组
    studentsData.qtBridge.log_message('Error loading students: ' + error.message);
  }
}

function renderStudents(studentsToRender = studentsData.students) {
  const container = document.getElementById('studentsContainer');
  if (!container) {
    console.error('Students container not found');
    return;
  }

  container.innerHTML = '';

  // 确保参数是数组
  if (!Array.isArray(studentsToRender)) {
    console.warn('studentsToRender is not an array, converting...');
    studentsToRender = Array.isArray(studentsData.students) ? studentsData.students : [];
  }

  if (studentsToRender.length === 0) {
    container.innerHTML = `
            <div class="no-students">
                <h3>暂无学生数据</h3>
                <p>点击"添加学生"按钮开始添加学生信息</p>
            </div>
        `;
    return;
  }
  studentsToRender.forEach(student => {
    try {
      const studentCard = createStudentCard(student);
      container.appendChild(studentCard);
    } catch (error) {
      console.error('Error creating student card:', error, student);
    }
  });
}

function createStudentCard(student) {
  const card = document.createElement('div');
  card.className = 'student-card';

  // 安全地获取学生数据，提供默认值
  const name = student.name || '未知姓名';
  const id = student.id || 0;
  const status = student.status || 'Active';

  card.innerHTML = `
        <div class="student-header">
            <div class="student-avatar">
                <img src="https://via.placeholder.com/80x80/4f46e5/ffffff?text=${encodeURIComponent(student.name ? student.name.charAt(0) : '学')}" alt="学生头像">
            </div>
            <div class="student-basic-info">
                <h2 class="student-name">${escapeHtml(student.name || '未知姓名')}</h2>
                <div class="student-id">学号: ${student.id || 0}</div>
                <div class="status-badge ${getStatusClass(student.status || 'Active')}">${getStatusText(student.status || 'Active')}</div>
            </div>
            <div class="student-actions">
                <button class="btn-icon edit" onclick="editStudent(${student.id})" title="编辑">✏️</button>
                <button class="btn-icon delete" onclick="deleteStudent(${student.id})" title="删除">🗑️</button>
            </div>
        </div>
        <div class="student-details">
            ${renderBasicInfo(student)}
            ${renderContactInfo(student)}
            ${renderCoursesInfo(student)}
            ${renderScoresInfo(student)}
            ${renderFamilyInfo(student)}
        </div>
    `;
  return card;
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

function renderBasicInfo(student) {
  const birthdate = student.birthdate || { year: 2000, month: 1, day: 1 };
  const sex = student.sex || 'Male';
  const admissionYear = student.admissionYear || new Date().getFullYear();
  const major = student.major || '未设置专业';

  return `
        <div class="info-section">
            <h3>基本信息</h3>
            <div class="info-grid">
                <div class="info-item">
                    <label>性别</label>
                    <span>${student.sex === 'Male' ? '男' : '女'}</span>
                </div>
                <div class="info-item">
                    <label>年龄</label>
                    <span>${calculateAge(student.birthdate || { year: 2000, month: 1, day: 1 })}岁</span>
                </div>
                <div class="info-item">
                    <label>生日</label>
                    <span>${(student.birthdate ? student.birthdate.year : 2000)}-${(student.birthdate ? student.birthdate.month : 1).toString().padStart(2, '0')}-${(student.birthdate ? student.birthdate.day : 1).toString().padStart(2, '0')}</span>
                </div>
                <div class="info-item">
                    <label>入学年份</label>
                    <span>${student.admissionYear || new Date().getFullYear()}</span>
                </div>
                <div class="info-item">
                    <label>专业</label>
                    <span>${escapeHtml(student.major || '未设置专业')}</span>
                </div>
            </div>
        </div>
    `;
}

function renderContactInfo(student) {
  const contact = student.contact || { phone: '', email: '' };
  const address = student.address || { province: '', city: '' };

  return `
        <div class="info-section">
            <h3>联系方式</h3>
            <div class="info-grid">
                <div class="info-item">
                    <label>电话</label>
                    <span>${escapeHtml(contact.phone || '未设置')}</span>
                </div>
                <div class="info-item">
                    <label>邮箱</label>
                    <span>${escapeHtml(contact.email || '未设置')}</span>
                </div>
                <div class="info-item">
                    <label>地址</label>
                    <span>${escapeHtml(address.province || '')} ${escapeHtml(address.city || '')}</span>
                </div>
            </div>
        </div>
    `;
}

function renderCoursesInfo(student) {
  const courses = student.courses || [];

  if (!Array.isArray(courses) || courses.length === 0) {
    return `
            <div class="info-section">
                <h3>课程信息</h3>
                <div class="courses-container">
                    <span class="no-data">暂无课程信息</span>
                </div>
            </div>
        `;
  }

  const courseTags = courses.map(course =>
      `<div class="course-tag">${escapeHtml(course)}</div>`
  ).join('');

  return `
        <div class="info-section">
            <h3>课程信息</h3>
            <div class="courses-container">
                ${courseTags}
            </div>
        </div>
    `;
}

function renderScoresInfo(student) {
  const scores = student.scores || {};

  if (!scores || typeof scores !== 'object' || Object.keys(scores).length === 0) {
    return `
            <div class="info-section">
                <h3>成绩信息</h3>
                <div class="scores-container">
                    <span class="no-data">暂无成绩信息</span>
                </div>
            </div>
        `;
  }

  const scoreItems = Object.entries(scores).map(([course, scoreData]) => {
    const score = scoreData.score || 0;
    const gpa = scoreData.gpa || 0;
    return `
            <div class="score-item">
                <div class="course-name">${escapeHtml(course)}</div>
                <div class="score-details">
                    <span class="score">${score}分</span>
                    <span class="gpa">GPA: ${gpa}</span>
                </div>
            </div>
        `;
  }).join('');

  const average = calculateAverage(scores);

  return `
        <div class="info-section">
            <h3>成绩信息</h3>
            <div class="scores-container">
                ${scoreItems}
            </div>
            <div class="average-score">
                <strong>平均分: ${average.toFixed(1)}</strong>
            </div>
        </div>
    `;
}

function renderFamilyInfo(student) {
  const familyMembers = student.familyMembers || [];

  if (!Array.isArray(familyMembers) || familyMembers.length === 0) {
    return `
            <div class="info-section">
                <h3>家庭成员</h3>
                <div class="family-members">
                    <span class="no-data">暂无家庭成员信息</span>
                </div>
            </div>
        `;
  }

  const familyItems = familyMembers.map(member => {
    const contactInfo = member.contactInfo || { phone: '' };
    return `
            <div class="family-member">
                <div class="member-info">
                    <span class="member-name">${escapeHtml(member.name || '')}</span>
                    <span class="member-relationship">${escapeHtml(member.relationship || '')}</span>
                </div>
                <div class="member-contact">
                    <span>${escapeHtml(contactInfo.phone || '')}</span>
                </div>
            </div>
        `;
  }).join('');

  return `
        <div class="info-section">
            <h3>家庭成员</h3>
            <div class="family-members">
                ${familyItems}
            </div>
        </div>
    `;
}

function showStudentModal(student = null) {
  const modal = document.getElementById('studentModal');
  const form = document.getElementById('studentForm');

  if (!modal || !form) {
    console.error('Modal or form not found');
    return;
  }

  studentsData.currentEditingId = student ? student.id : null;

  if (student) {
    populateForm(student);
    document.getElementById('modalTitle').textContent = '编辑学生信息';
  } else {
    form.reset();
    clearDynamicFields();
    document.getElementById('modalTitle').textContent = '添加学生信息';
  }

  modal.style.display = 'block';
}

function populateForm(student) {
  setInputValue('studentId', student.id);
  setInputValue('studentName', student.name);
  setInputValue('studentSex', student.sex);

  const birthdate = student.birthdate || {};
  setInputValue('birthYear', birthdate.year);
  setInputValue('birthMonth', birthdate.month);
  setInputValue('birthDay', birthdate.day);

  setInputValue('admissionYear', student.admissionYear);
  setInputValue('major', student.major);

  const contact = student.contact || {};
  setInputValue('phone', contact.phone);
  setInputValue('email', contact.email);

  const address = student.address || {};
  setInputValue('province', address.province);
  setInputValue('city', address.city);

  setInputValue('status', student.status);

  // 填充动态字段
  clearDynamicFields();

  // 填充课程
  if (Array.isArray(student.courses)) {
    student.courses.forEach(course => {
      addCourseField(course);
    });
  }

  // 填充家庭成员
  if (Array.isArray(student.familyMembers)) {
    student.familyMembers.forEach(member => {
      addFamilyMemberField(member);
    });
  }

  // 填充成绩
  if (student.scores && typeof student.scores === 'object') {
    Object.entries(student.scores).forEach(([course, scoreData]) => {
      addScoreField(course, scoreData.score, scoreData.gpa);
    });
  }
}

function setInputValue(id, value) {
  const element = document.getElementById(id);
  if (element && value !== undefined && value !== null) {
    element.value = value;
  }
}

function clearDynamicFields() {
  const containers = ['coursesContainer', 'familyContainer', 'scoresContainer'];
  containers.forEach(containerId => {
    const container = document.getElementById(containerId);
    if (container) {
      container.innerHTML = '';
    }
  });
}

function addCourseField(value = '') {
  const container = document.getElementById('coursesContainer');
  if (!container) return;
  const div = document.createElement('div');
  div.className = 'course-field';
  div.innerHTML = `
        <input type="text" class="course-input" value="${escapeHtml(value)}" placeholder="课程名称">
        <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
    `;
  container.appendChild(div);
}

function addFamilyMemberField(member = null) {
  const container = document.getElementById('familyContainer');
  if (!container) return;
  const div = document.createElement('div');
  div.className = 'family-field';
  const name = member ? member.name || '' : '';
  const relationship = member ? member.relationship || '' : '';
  const phone = member && member.contactInfo ? member.contactInfo.phone || '' : '';
  div.innerHTML = `
        <input type="text" class="family-name" value="${escapeHtml(name)}" placeholder="姓名">
        <input type="text" class="family-relationship" value="${escapeHtml(relationship)}" placeholder="关系">
        <input type="text" class="family-phone" value="${escapeHtml(phone)}" placeholder="电话">
        <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
    `;
  container.appendChild(div);
}

function addScoreField(course = '', score = '', gpa = '') {
  const container = document.getElementById('scoresContainer');
  if (!container) return;
  const div = document.createElement('div');
  div.className = 'score-field';
  div.innerHTML = `
        <input type="text" class="score-course" value="${escapeHtml(course)}" placeholder="课程名称">
        <input type="number" class="score-value" value="${score}" placeholder="分数" min="0" max="100" step="0.1">
        <input type="number" class="score-gpa" value="${gpa}" placeholder="GPA" min="0" max="4" step="0.1">
        <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
    `;
  container.appendChild(div);
}

function saveStudent() {
  const studentData = collectFormData();

  if (!validateStudentData(studentData)) {
    return;
  }

  try {
    if (studentsData.currentEditingId) {
      // 更新学生
      studentsData.qtBridge.update_student(studentData);
      const index = studentsData.students.findIndex(s => s.id === studentsData.currentEditingId);
      if (index !== -1) {
        studentsData.students[index] = studentData;
      }
    } else {
      // 添加新学生
      studentsData.qtBridge.add_student(studentData);
      // 确保students是数组
      if (!Array.isArray(studentsData.students)) {
        studentsData.students = [];
      }
      studentsData.students.push(studentData);
    }

    closeStudentModal();
    renderStudents();
    studentsData.qtBridge.show_notification('成功', studentsData.currentEditingId ? '学生信息已更新' : '学生信息已添加');
  } catch (error) {
    console.error('Error saving student:', error);
    studentsData.qtBridge.log_message('Error saving student: ' + error.message);
    alert('保存失败：' + error.message);
  }
}

function collectFormData() {
  const courses = Array.from(document.querySelectorAll('.course-input'))
      .map(input => input.value.trim())
      .filter(course => course);

  const familyMembers = Array.from(document.querySelectorAll('.family-field')).map(field => ({
    name: field.querySelector('.family-name').value.trim(),
    relationship: field.querySelector('.family-relationship').value.trim(),
    contactInfo: {
      phone: field.querySelector('.family-phone').value.trim(),
      email: ''
    }
  })).filter(member => member.name && member.relationship);

  const scores = {};
  Array.from(document.querySelectorAll('.score-field')).forEach(field => {
    const course = field.querySelector('.score-course').value.trim();
    const score = parseFloat(field.querySelector('.score-value').value) || 0;
    const gpa = parseFloat(field.querySelector('.score-gpa').value) || 0;
    if (course) {
      scores[course] = { score, gpa };
    }
  });

  return {
    id: parseInt(document.getElementById('studentId').value) || Date.now(),
    name: document.getElementById('studentName').value.trim(),
    sex: document.getElementById('studentSex').value,
    birthdate: {
      year: parseInt(document.getElementById('birthYear').value) || new Date().getFullYear() - 20,
      month: parseInt(document.getElementById('birthMonth').value) || 1,
      day: parseInt(document.getElementById('birthDay').value) || 1
    },
    admissionYear: parseInt(document.getElementById('admissionYear').value) || new Date().getFullYear(),
    major: document.getElementById('major').value.trim(),
    courses,
    contact: {
      phone: document.getElementById('phone').value.trim(),
      email: document.getElementById('email').value.trim()
    },
    address: {
      province: document.getElementById('province').value.trim(),
      city: document.getElementById('city').value.trim()
    },
    status: document.getElementById('status').value,
    familyMembers,
    scores
  };
}

function validateStudentData(data) {
  if (!data.name) {
    alert('请输入学生姓名');
    return false;
  }
  if (!data.major) {
    alert('请输入专业');
    return false;
  }
  if (!data.contact.phone) {
    alert('请输入电话号码');
    return false;
  }
  return true;
}

function editStudent(studentId) {
  if (!Array.isArray(studentsData.students)) {
    console.error('Students is not an array');
    return;
  }
  const student = studentsData.students.find(s => s.id === studentId);
  if (student) {
    showStudentModal(student);
  }
}

function deleteStudent(studentId) {
  if (confirm('确定要删除这个学生吗？')) {
    try {
      studentsData.qtBridge.delete_student(studentId);
      if (!Array.isArray(studentsData.students)) {
        studentsData.students = [];
      } else {
        studentsData.students = studentsData.students.filter(s => s.id !== studentId);
      }
      renderStudents();
      studentsData.qtBridge.show_notification('成功', '学生已删除');
    } catch (error) {
      console.error('Error deleting student:', error);
      studentsData.qtBridge.log_message('Error deleting student: ' + error.message);
    }
  }
}

function closeStudentModal() {
  const modal = document.getElementById('studentModal');
  if (modal) {
    modal.style.display = 'none';
  }
  studentsData.currentEditingId = null;
}

function filterStudents(searchTerm) {
  if (!Array.isArray(studentsData.students)) {
    console.error('Students is not an array');
    return;
  }
  const filtered = studentsData.students.filter(student =>
      (student.name && student.name.toLowerCase().includes(searchTerm.toLowerCase())) ||
      (student.id && student.id.toString().includes(searchTerm)) ||
      (student.major && student.major.toLowerCase().includes(searchTerm.toLowerCase()))
  );
  renderStudents(filtered);
}

function importData() {
  studentsData.qtBridge.open_file_dialog('导入学生数据', 'JSON Files (*.json)');
}

function exportData() {
  studentsData.qtBridge.save_file_dialog('导出学生数据', 'JSON Files (*.json)');
}

function getStatusClass(status) {
  switch(status) {
    case 'Active': return 'status-active';
    case 'Leave': return 'status-leave';
    case 'Graduated': return 'status-graduated';
    default: return 'status-active';
  }
}

function getStatusText(status) {
  switch(status) {
    case 'Active': return '在读';
    case 'Leave': return '休学';
    case 'Graduated': return '毕业';
    default: return '在读';
  }
}

function calculateAge(birthdate) {
  if (!birthdate || !birthdate.year) return 0;
  const today = new Date();
  const birth = new Date(birthdate.year, (birthdate.month || 1) - 1, birthdate.day || 1);
  let age = today.getFullYear() - birth.getFullYear();
  const monthDiff = today.getMonth() - birth.getMonth();
  if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
    age--;
  }
  return Math.max(0, age);
}

function calculateAverage(scores) {
  if (!scores || typeof scores !== 'object') return 0;
  const values = Object.values(scores).map(s => s.score || 0);
  return values.length > 0 ? values.reduce((a, b) => a + b, 0) / values.length : 0;
}

// 初始化应用
document.addEventListener('DOMContentLoaded', () => {
  console.log('DOM loaded, initializing StudentManager...');
  initStudentManager();

  // 将主要函数暴露到全局作用域
  window.editStudent = editStudent;
  window.deleteStudent = deleteStudent;
});

window.addEventListener('load', () => {
  // 确保函数可以被HTML调用
  window.editStudent = editStudent;
  window.deleteStudent = deleteStudent;
});
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
  display: none;
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