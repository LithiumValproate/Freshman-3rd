package main

import (
	"log"
	"net"

	"./im"
)

func main() {
	// 创建服务器
	server := im.NewServer()

	// 监听端口
	listener, err := net.Listen("tcp", ":8081")
	if err != nil {
		log.Fatalf("无法启动服务器: %v", err)
	}
	defer listener.Close()
	log.Println("Go 聊天服务器正在监听: localhost:8081")

	// 循环接受新的客户端连接
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("接受连接失败: %v", err)
			continue
		}

		// 为每个连接启动一个新的 goroutine 来处理
		go server.HandleConnection(conn)
	}
}
