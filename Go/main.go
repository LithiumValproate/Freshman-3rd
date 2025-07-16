// chat-server/main.go

package main

import (
	"log"
	"net/http"

	"github.com/lithiumvalproate/freshman-3rd/Go/src" // <-- 修改导入路径

	"github.com/gorilla/websocket"
)

// WebSocket升级器，允许所有来源的连接（用于开发）
var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin: func(r *http.Request) bool {
		return true // 在生产环境中应进行更严格的检查
	},
}

func main() {
	// 创建一个新的服务器实例
	server := src.NewServer()

	// 启动服务器的中心处理goroutine
	go server.Run()

	// 设置WebSocket处理函数
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			log.Printf("WebSocket upgrade failed: %v", err)
			return
		}
		// 为每个新连接创建一个客户端
		src.NewClient(server, conn)
	})

	log.Println("Go WebSocket server starting on :8081")
	// 启动HTTP服务器
	err := http.ListenAndServe(":8081", nil)
	if err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}
