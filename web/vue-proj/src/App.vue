<template>
  <router-view />
</template>

<script setup>
import { ref, onMounted, provide } from 'vue';

const qtBridge = ref(null);
// 新增：学生列表并提供给子组件
const students = ref([]);
provide('students', students);

// 等待 Qt Bridge 初始化
const waitForQtBridge = () => {
  return new Promise((resolve) => {
    // 检查是否在 Qt 环境中
    if (typeof qt !== 'undefined' && qt.webChannelTransport) {
      new QWebChannel(qt.webChannelTransport, (channel) => {
        qtBridge.value = channel.objects.qtBridge;
        console.log('Qt Bridge connected');
        resolve();
      });
    } else if (typeof QWebChannel !== 'undefined') {
      // 备用方案：直接尝试创建WebChannel
      try {
        new QWebChannel(qt.webChannelTransport, (channel) => {
          qtBridge.value = channel.objects.qtBridge;
          console.log('Qt Bridge connected (fallback)');
          resolve();
        });
      } catch (error) {
        console.warn('Qt Bridge not available, using mock data:', error);
        qtBridge.value = createMockBridge();
        resolve();
      }
    } else {
      console.warn('Qt Bridge not available, using mock data');
      qtBridge.value = createMockBridge();
      resolve();
    }
  });
};

// 初始化后拉取学生数据
onMounted(async () => {
  await waitForQtBridge();
  try {
    const result = await qtBridge.value.get_students();
    students.value = Array.isArray(result) ? result : [];
    console.log('Loaded students in App.vue:', students.value.length);
  } catch (e) {
    console.warn('Failed to load students:', e);
    students.value = [];
  }
});
</script>

<style scoped>
/* ...existing code... */
</style>