const studentManager = {
    students: [],
    currentEditingId: null,
    qtBridge: null,

    async init() {
        await this.waitForQtBridge();
        this.setupEventListeners();
        await this.loadStudents();
        this.renderStudents();
    },

    waitForQtBridge() {
        return new Promise((resolve) => {
            if (typeof qt !== 'undefined' && qt.webChannelTransport) {
                new QWebChannel(qt.webChannelTransport, (channel) => {
                    studentManager.qtBridge = channel.objects.qtBridge;
                    console.log('Qt Bridge connected');
                    resolve();
                });
            } else {
                console.warn('Qt Bridge not available, using mock data');
                studentManager.qtBridge = studentManager.createMockBridge ? studentManager.createMockBridge() : {};
                resolve();
            }
        });
    },

    setupEventListeners() {
        // 添加学生按钮
        const addBtn = document.getElementById('addStudentBtn');
        if (addBtn) {
            addBtn.addEventListener('click', () => {
                studentManager.showStudentModal();
            });
        }

        // 导入数据按钮
        const importBtn = document.getElementById('importBtn');
        if (importBtn) {
            importBtn.addEventListener('click', () => {
                studentManager.importData();
            });
        }

        // 导出数据按钮
        const exportBtn = document.getElementById('exportBtn');
        if (exportBtn) {
            exportBtn.addEventListener('click', () => {
                studentManager.exportData();
            });
        }

        // 搜索功能
        const searchInput = document.getElementById('searchInput');
        if (searchInput) {
            searchInput.addEventListener('input', (e) => {
                studentManager.filterStudents(e.target.value);
            });
        }

        // 模态框关闭
        const closeModal = document.getElementById('closeModal');
        if (closeModal) {
            closeModal.addEventListener('click', () => {
                studentManager.closeModal();
            });
        }

        // 保存学生信息
        const saveBtn = document.getElementById('saveStudentBtn');
        if (saveBtn) {
            saveBtn.addEventListener('click', () => {
                studentManager.saveStudent();
            });
        }

        // 添加课程
        const addCourseBtn = document.getElementById('addCourseBtn');
        if (addCourseBtn) {
            addCourseBtn.addEventListener('click', () => {
                studentManager.addCourseField();
            });
        }

        // 添加家庭成员
        const addFamilyBtn = document.getElementById('addFamilyBtn');
        if (addFamilyBtn) {
            addFamilyBtn.addEventListener('click', () => {
                studentManager.addFamilyMemberField();
            });
        }

        // 添加成绩
        const addScoreBtn = document.getElementById('addScoreBtn');
        if (addScoreBtn) {
            addScoreBtn.addEventListener('click', () => {
                studentManager.addScoreField();
            });
        }
    },

    async loadStudents() {
        try {
            console.log('Loading students from backend...');
            const result = studentManager.qtBridge.get_students();
            console.log('Raw result from Qt:', result);

            // 确保结果是数组格式
            if (Array.isArray(result)) {
                studentManager.students = result;
            } else if (result && typeof result === 'object') {
                // 如果返回的是QJsonArray，需要转换
                if (result.length !== undefined) {
                    // 类数组对象，转换为真正的数组
                    studentManager.students = Array.from(result);
                } else {
                    // 可能是单个对象，包装成数组
                    studentManager.students = [result];
                }
            } else {
                // 如果没有数据或数据格式不正确，初始化为空数组
                console.warn('Invalid data format from backend, initializing empty array');
                studentManager.students = [];
            }

            console.log('Students loaded:', studentManager.students.length, 'items');
            studentManager.qtBridge.log_message(`Students loaded: ${studentManager.students.length} items`);
        } catch (error) {
            console.error('Error loading students:', error);
            studentManager.students = []; // 确保在错误情况下也是数组
            studentManager.qtBridge.log_message('Error loading students: ' + error.message);
        }
    },

    renderStudents(studentsToRender = studentManager.students) {
        const container = document.getElementById('studentsContainer');
        if (!container) {
            console.error('Students container not found');
            return;
        }

        container.innerHTML = '';

        // 确保参数是数组
        if (!Array.isArray(studentsToRender)) {
            console.warn('studentsToRender is not an array, converting...');
            studentsToRender = Array.isArray(studentManager.students) ? studentManager.students : [];
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
                const studentCard = studentManager.createStudentCard(student);
                container.appendChild(studentCard);
            } catch (error) {
                console.error('Error creating student card:', error, student);
            }
        });
    },

    createStudentCard(student) {
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
                    <h2 class="student-name">${studentManager.escapeHtml(student.name || '未知姓名')}</h2>
                    <div class="student-id">学号: ${student.id || 0}</div>
                    <div class="status-badge ${studentManager.getStatusClass(student.status || 'Active')}">${studentManager.getStatusText(student.status || 'Active')}</div>
                </div>
                <div class="student-actions">
                    <button class="btn-icon edit" onclick="window.studentManager.editStudent(${student.id})" title="编辑">✏️</button>
                    <button class="btn-icon delete" onclick="window.studentManager.deleteStudent(${student.id})" title="删除">🗑️</button>
                </div>
            </div>
            <div class="student-details">
                ${studentManager.renderBasicInfo(student)}
                ${studentManager.renderContactInfo(student)}
                ${studentManager.renderCoursesInfo(student)}
                ${studentManager.renderScoresInfo(student)}
                ${studentManager.renderFamilyInfo(student)}
            </div>
        `;
        return card;
    },

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    },

    renderBasicInfo(student) {
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
                        <span>${studentManager.calculateAge(student.birthdate || { year: 2000, month: 1, day: 1 })}岁</span>
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
                        <span>${studentManager.escapeHtml(student.major || '未设置专业')}</span>
                    </div>
                </div>
            </div>
        `;
    },

    renderContactInfo(student) {
        const contact = student.contact || { phone: '', email: '' };
        const address = student.address || { province: '', city: '' };

        return `
            <div class="info-section">
                <h3>联系方式</h3>
                <div class="info-grid">
                    <div class="info-item">
                        <label>电话</label>
                        <span>${studentManager.escapeHtml(contact.phone || '未设置')}</span>
                    </div>
                    <div class="info-item">
                        <label>邮箱</label>
                        <span>${studentManager.escapeHtml(contact.email || '未设置')}</span>
                    </div>
                    <div class="info-item">
                        <label>地址</label>
                        <span>${studentManager.escapeHtml(address.province || '')} ${studentManager.escapeHtml(address.city || '')}</span>
                    </div>
                </div>
            </div>
        `;
    },

    renderCoursesInfo(student) {
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
            `<div class="course-tag">${studentManager.escapeHtml(course)}</div>`
        ).join('');

        return `
            <div class="info-section">
                <h3>课程信息</h3>
                <div class="courses-container">
                    ${courseTags}
                </div>
            </div>
        `;
    },

    renderScoresInfo(student) {
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
                    <div class="course-name">${studentManager.escapeHtml(course)}</div>
                    <div class="score-details">
                        <span class="score">${score}分</span>
                        <span class="gpa">GPA: ${gpa}</span>
                    </div>
                </div>
            `;
        }).join('');

        const average = studentManager.calculateAverage(scores);

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
    },

    renderFamilyInfo(student) {
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
                        <span class="member-name">${studentManager.escapeHtml(member.name || '')}</span>
                        <span class="member-relationship">${studentManager.escapeHtml(member.relationship || '')}</span>
                    </div>
                    <div class="member-contact">
                        <span>${studentManager.escapeHtml(contactInfo.phone || '')}</span>
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
    },

    showStudentModal(student = null) {
        const modal = document.getElementById('studentModal');
        const form = document.getElementById('studentForm');

        if (!modal || !form) {
            console.error('Modal or form not found');
            return;
        }

        studentManager.currentEditingId = student ? student.id : null;

        if (student) {
            studentManager.populateForm(student);
            document.getElementById('modalTitle').textContent = '编辑学生信息';
        } else {
            form.reset();
            studentManager.clearDynamicFields();
            document.getElementById('modalTitle').textContent = '添加学生信息';
        }

        modal.style.display = 'block';
    },

    populateForm(student) {
        studentManager.setInputValue('studentId', student.id);
        studentManager.setInputValue('studentName', student.name);
        studentManager.setInputValue('studentSex', student.sex);

        const birthdate = student.birthdate || {};
        studentManager.setInputValue('birthYear', birthdate.year);
        studentManager.setInputValue('birthMonth', birthdate.month);
        studentManager.setInputValue('birthDay', birthdate.day);

        studentManager.setInputValue('admissionYear', student.admissionYear);
        studentManager.setInputValue('major', student.major);

        const contact = student.contact || {};
        studentManager.setInputValue('phone', contact.phone);
        studentManager.setInputValue('email', contact.email);

        const address = student.address || {};
        studentManager.setInputValue('province', address.province);
        studentManager.setInputValue('city', address.city);

        studentManager.setInputValue('status', student.status);

        // 填充动态字段
        studentManager.clearDynamicFields();

        // 填充课程
        if (Array.isArray(student.courses)) {
            student.courses.forEach(course => {
                studentManager.addCourseField(course);
            });
        }

        // 填充家庭成员
        if (Array.isArray(student.familyMembers)) {
            student.familyMembers.forEach(member => {
                studentManager.addFamilyMemberField(member);
            });
        }

        // 填充成绩
        if (student.scores && typeof student.scores === 'object') {
            Object.entries(student.scores).forEach(([course, scoreData]) => {
                studentManager.addScoreField(course, scoreData.score, scoreData.gpa);
            });
        }
    },

    setInputValue(id, value) {
        const element = document.getElementById(id);
        if (element && value !== undefined && value !== null) {
            element.value = value;
        }
    },

    clearDynamicFields() {
        const containers = ['coursesContainer', 'familyContainer', 'scoresContainer'];
        containers.forEach(containerId => {
            const container = document.getElementById(containerId);
            if (container) {
                container.innerHTML = '';
            }
        });
    },

    addCourseField(value = '') {
        const container = document.getElementById('coursesContainer');
        if (!container) return;
        const div = document.createElement('div');
        div.className = 'course-field';
        div.innerHTML = `
            <input type="text" class="course-input" value="${studentManager.escapeHtml(value)}" placeholder="课程名称">
            <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
        `;
        container.appendChild(div);
    },

    addFamilyMemberField(member = null) {
        const container = document.getElementById('familyContainer');
        if (!container) return;
        const div = document.createElement('div');
        div.className = 'family-field';
        const name = member ? member.name || '' : '';
        const relationship = member ? member.relationship || '' : '';
        const phone = member && member.contactInfo ? member.contactInfo.phone || '' : '';
        div.innerHTML = `
            <input type="text" class="family-name" value="${studentManager.escapeHtml(name)}" placeholder="姓名">
            <input type="text" class="family-relationship" value="${studentManager.escapeHtml(relationship)}" placeholder="关系">
            <input type="text" class="family-phone" value="${studentManager.escapeHtml(phone)}" placeholder="电话">
            <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
        `;
        container.appendChild(div);
    },

    addScoreField(course = '', score = '', gpa = '') {
        const container = document.getElementById('scoresContainer');
        if (!container) return;
        const div = document.createElement('div');
        div.className = 'score-field';
        div.innerHTML = `
            <input type="text" class="score-course" value="${studentManager.escapeHtml(course)}" placeholder="课程名称">
            <input type="number" class="score-value" value="${score}" placeholder="分数" min="0" max="100" step="0.1">
            <input type="number" class="score-gpa" value="${gpa}" placeholder="GPA" min="0" max="4" step="0.1">
            <button type="button" class="remove-btn" onclick="this.parentElement.remove()">删除</button>
        `;
        container.appendChild(div);
    },

    saveStudent() {
        const studentData = studentManager.collectFormData();

        if (!studentManager.validateStudentData(studentData)) {
            return;
        }

        try {
            if (studentManager.currentEditingId) {
                // 更新学生
                studentManager.qtBridge.update_student(studentData);
                const index = studentManager.students.findIndex(s => s.id === studentManager.currentEditingId);
                if (index !== -1) {
                    studentManager.students[index] = studentData;
                }
            } else {
                // 添加新学生
                studentManager.qtBridge.add_student(studentData);
                // 确保students是数组
                if (!Array.isArray(studentManager.students)) {
                    studentManager.students = [];
                }
                studentManager.students.push(studentData);
            }

            studentManager.closeModal();
            studentManager.renderStudents();
            studentManager.qtBridge.show_notification('成功', studentManager.currentEditingId ? '学生信息已更新' : '学生信息已添加');
        } catch (error) {
            console.error('Error saving student:', error);
            studentManager.qtBridge.log_message('Error saving student: ' + error.message);
            alert('保存失败：' + error.message);
        }
    },

    collectFormData() {
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
    },

    validateStudentData(data) {
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
    },

    editStudent(studentId) {
        if (!Array.isArray(studentManager.students)) {
            console.error('Students is not an array');
            return;
        }
        const student = studentManager.students.find(s => s.id === studentId);
        if (student) {
            studentManager.showStudentModal(student);
        }
    },

    deleteStudent(studentId) {
        if (confirm('确定要删除这个学生吗？')) {
            try {
                studentManager.qtBridge.delete_student(studentId);
                if (!Array.isArray(studentManager.students)) {
                    studentManager.students = [];
                } else {
                    studentManager.students = studentManager.students.filter(s => s.id !== studentId);
                }
                studentManager.renderStudents();
                studentManager.qtBridge.show_notification('成功', '学生已删除');
            } catch (error) {
                console.error('Error deleting student:', error);
                studentManager.qtBridge.log_message('Error deleting student: ' + error.message);
            }
        }
    },

    closeModal() {
        const modal = document.getElementById('studentModal');
        if (modal) {
            modal.style.display = 'none';
        }
        studentManager.currentEditingId = null;
    },

    filterStudents(searchTerm) {
        if (!Array.isArray(studentManager.students)) {
            console.error('Students is not an array');
            return;
        }
        const filtered = studentManager.students.filter(student =>
            (student.name && student.name.toLowerCase().includes(searchTerm.toLowerCase())) ||
            (student.id && student.id.toString().includes(searchTerm)) ||
            (student.major && student.major.toLowerCase().includes(searchTerm.toLowerCase()))
        );
        studentManager.renderStudents(filtered);
    },

    importData() {
        studentManager.qtBridge.open_file_dialog('导入学生数据', 'JSON Files (*.json)');
    },

    exportData() {
        studentManager.qtBridge.save_file_dialog('导出学生数据', 'JSON Files (*.json)');
    },

    getStatusClass(status) {
        switch(status) {
            case 'Active': return 'status-active';
            case 'Leave': return 'status-leave';
            case 'Graduated': return 'status-graduated';
            default: return 'status-active';
        }
    },

    getStatusText(status) {
        switch(status) {
            case 'Active': return '在读';
            case 'Leave': return '休学';
            case 'Graduated': return '毕业';
            default: return '在读';
        }
    },

    calculateAge(birthdate) {
        if (!birthdate || !birthdate.year) return 0;
        const today = new Date();
        const birth = new Date(birthdate.year, (birthdate.month || 1) - 1, birthdate.day || 1);
        let age = today.getFullYear() - birth.getFullYear();
        const monthDiff = today.getMonth() - birth.getMonth();
        if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
            age--;
        }
        return Math.max(0, age);
    },

    calculateAverage(scores) {
        if (!scores || typeof scores !== 'object') return 0;
        const values = Object.values(scores).map(s => s.score || 0);
        return values.length > 0 ? values.reduce((a, b) => a + b, 0) / values.length : 0;
    }
};

// 初始化应用
document.addEventListener('DOMContentLoaded', () => {
    console.log('DOM loaded, initializing StudentManager...');
    studentManager.init();
    window.studentManager = studentManager;
});
window.addEventListener('load', () => {
    if (!window.studentManager) {
        window.studentManager = studentManager;
    }
});
