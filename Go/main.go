package main

import (
	pkg "github.com/lithiumvalproate/freshman-3rd/Go/src"
	"log"
	"net/http" // New import
	"github.com/gorilla/websocket" // New import
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin: func(r *http.Request) bool {
		// Allow all origins for development. In production, restrict this.
		return true
	},
}

func main() {
	server := pkg.NewServer()

	// WebSocket handler
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			log.Printf("WebSocket upgrade failed: %v", err)
			return
		}
		// Pass the WebSocket connection to the server's handler
		go server.HandleConnection(conn) // This will be adapted in src/chat.go
	})

	log.Println("Go WebSocket server starting on :8081")
	err := http.ListenAndServe(":8081", nil)
	if err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}
