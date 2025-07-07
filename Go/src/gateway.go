package src

import (
	"strings"
)

// 后端服务路由表，根据实际服务端口填写
var BackendServices = map[string]string{
	"/im":    "http://localhost:9001", // IM 服务
	"/bbs":   "http://localhost:9002", // BBS 服务
	"/drive": "http://localhost:9003", // 网盘服务
	//	"/classsys": "http://localhost:9004", // 教务系统
	"/struct": "http://localhost:9005", // 数据管理系统（C++）
}

// 获取后端服务基地址
func GetBackendTarget(path string) string {
	for prefix, target := range BackendServices {
		if strings.HasPrefix(path, prefix) {
			return target
		}
	}
	return ""
}
