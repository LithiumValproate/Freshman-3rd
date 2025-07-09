<template>
  <div class="student-page">
    <el-row :gutter="20">
      <!-- Left Column: Profile and Actions -->
      <el-col :span="8">
        <el-card class="box-card profile-card">
          <template #header>
            <div class="card-header">
              <span>Student Profile</span>
            </div>
          </template>
          <div class="profile-body">
            <div class="profile-item">
              <strong>Name:</strong> {{ student.name }}
            </div>
            <div class="profile-item">
              <strong>ID:</strong> {{ student.id }}
            </div>
            <div class="profile-item">
              <strong>Major:</strong> {{ student.major }}
            </div>
            <div class="profile-item">
              <strong>Class:</strong> {{ student.class_ }}
            </div>
            <div class="profile-item">
              <strong>Enrollment Year:</strong> {{ student.enrollYear }}
            </div>
            <div class="profile-item">
              <strong>Email:</strong> {{ student.contactInfo.email }}
            </div>
            <div class="profile-item">
              <strong>Phone:</strong> {{ student.contactInfo.phone }}
            </div>
          </div>
        </el-card>

        <el-card class="box-card actions-card">
          <template #header>
            <div class="card-header">
              <span>Actions</span>
            </div>
          </template>
          <div class="actions-body">
            <el-button type="primary" :icon="ChatDotRound">Chat</el-button>
            <el-button type="success" :icon="Collection">BBS</el-button>
            <el-button type="info" :icon="Folder">Cloud Drive</el-button>
          </div>
        </el-card>
      </el-col>

      <!-- Right Column: Scores and Schedule -->
      <el-col :span="16">
        <el-card class="box-card">
          <template #header>
            <div class="card-header">
              <span>Course Scores</span>
            </div>
          </template>
          <el-table :data="student.courseScore" style="width: 100%">
            <el-table-column prop="course" label="Course Name" />
            <el-table-column prop="score" label="Score" width="120" />
            <el-table-column prop="gpa" label="GPA" width="120" />
          </el-table>
        </el-card>

        <el-card class="box-card">
          <template #header>
            <div class="card-header">
              <span>Weekly Schedule</span>
            </div>
          </template>
          <!-- Placeholder for schedule table -->
          <div class="placeholder-text">
            Schedule functionality will be implemented here.
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { reactive } from 'vue';
import {
  ChatDotRound,
  Collection,
  Folder,
} from '@element-plus/icons-vue';

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
</script>

<style scoped>
.student-page {
  padding: 20px;
}
.box-card {
  margin-bottom: 20px;
}
.card-header {
  font-weight: bold;
}
.profile-item {
  margin-bottom: 10px;
}
.actions-body {
  display: flex;
  justify-content: space-around;
}
.placeholder-text {
  text-align: center;
  color: #999;
  padding: 20px;
}
</style>
