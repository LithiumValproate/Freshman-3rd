// chat-server/src/server.go

package src

import (
	"fmt"
	"log"
	"strings"
	"sync"

	"github.com/gorilla/websocket"
)

// Server 结构体管理所有的聊天室和客户端操作
type Server struct {
	rooms    map[string]*Room // 存储所有聊天室，以房间名作为key
	mu       sync.RWMutex     // 用于保护 rooms map 的读写锁
	commands chan Command     // 用于接收客户端命令的通道
}

// Room 结构体代表一个聊天室
type Room struct {
	name      string
	clients   map[*Client]bool // 存储在该房间的所有客户端
	broadcast chan []byte      // 用于向房间内所有客户端广播消息的通道
}

// Client 结构体代表一个连接的客户端
type Client struct {
	server *Server
	conn   *websocket.Conn
	nick   string
	room   *Room // 指向客户端当前所在的房间
	send   chan []byte
}

// Command 结构体用于在客户端和服务器之间传递带指令的操作
type Command struct {
	client *Client
	id     string   // 命令ID，例如 "/join", "/nick"
	args   []string // 命令参数
}

// NewServer 创建并初始化一个新的服务器
func NewServer() *Server {
	return &Server{
		rooms:    make(map[string]*Room),
		commands: make(chan Command),
	}
}

// NewClient 创建并启动一个新的客户端
func NewClient(server *Server, conn *websocket.Conn) {
	client := &Client{
		server: server,
		conn:   conn,
		nick:   "anonymous",
		send:   make(chan []byte, 256),
	}

	// 启动读写goroutine
	go client.readPump()
	go client.writePump()

	// 发送欢迎信息
	client.msg("Welcome to the Go Chat Room!\nAvailable commands:\n /nick <name>\n /join <room>\n /rooms\n /quit")
}

// Run 是服务器的主循环，处理来自所有客户端的命令
func (s *Server) Run() {
	for cmd := range s.commands {
		switch cmd.id {
		case "/nick":
			s.nick(cmd.client, cmd.args)
		case "/join":
			s.join(cmd.client, cmd.args)
		case "/rooms":
			s.listRooms(cmd.client)
		case "/quit":
			s.quit(cmd.client)
		case "/msg":
			s.message(cmd.client, cmd.args)
		}
	}
}

// readPump 从WebSocket连接中读取消息，并将其转换为命令或消息
func (c *Client) readPump() {
	defer func() {
		c.server.quit(c) // 确保客户端断开连接时能正确退出房间
		c.conn.Close()
	}()

	for {
		_, message, err := c.conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("error: %v", err)
			}
			break
		}

		input := strings.TrimSpace(string(message))
		if strings.HasPrefix(input, "/") {
			parts := strings.Split(input, " ")
			cmdID := parts[0]
			args := parts[1:]
			c.server.commands <- Command{client: c, id: cmdID, args: args}
		} else {
			c.server.commands <- Command{client: c, id: "/msg", args: []string{input}}
		}
	}
}

// writePump 将消息从send通道写入WebSocket连接
func (c *Client) writePump() {
	defer c.conn.Close()
	for message := range c.send {
		c.conn.WriteMessage(websocket.TextMessage, message)
	}
}

// msg 是一个向客户端发送消息的辅助函数
func (c *Client) msg(message string) {
	c.send <- []byte(message)
}

// --- Server Command Handlers ---

func (s *Server) nick(c *Client, args []string) {
	if len(args) < 1 {
		c.msg("Usage: /nick <your_nickname>")
		return
	}
	c.nick = args[0]
	c.msg(fmt.Sprintf("Your nickname is now %s", c.nick))
}

func (s *Server) join(c *Client, args []string) {
	if len(args) < 1 {
		c.msg("Usage: /join <room_name>")
		return
	}
	roomName := args[0]

	// 如果已经在房间，先退出
	if c.room != nil {
		s.quit(c)
	}

	s.mu.Lock()
	room, ok := s.rooms[roomName]
	if !ok {
		// 如果房间不存在，则创建新房间
		room = &Room{
			name:      roomName,
			clients:   make(map[*Client]bool),
			broadcast: make(chan []byte),
		}
		s.rooms[roomName] = room
		go s.runRoom(room) // 为新房间启动广播goroutine
	}
	s.mu.Unlock()

	c.room = room
	room.clients[c] = true // 将客户端添加到房间
	room.broadcast <- []byte(fmt.Sprintf("%s has joined the room.", c.nick))
	c.msg(fmt.Sprintf("Welcome to %s!", room.name))
}

func (s *Server) listRooms(c *Client) {
	s.mu.RLock()
	defer s.mu.RUnlock()

	if len(s.rooms) == 0 {
		c.msg("No active rooms.")
		return
	}

	var roomNames []string
	for name := range s.rooms {
		roomNames = append(roomNames, name)
	}
	c.msg(fmt.Sprintf("Available rooms: %s", strings.Join(roomNames, ", ")))
}

func (s *Server) quit(c *Client) {
	if c.room == nil {
		c.msg("You are not in a room.")
		return
	}

	room := c.room
	delete(room.clients, c)
	c.room = nil
	room.broadcast <- []byte(fmt.Sprintf("%s has left the room.", c.nick))

	// 如果房间空了，可以考虑关闭并删除它
	if len(room.clients) == 0 {
		s.mu.Lock()
		delete(s.rooms, room.name)
		s.mu.Unlock()
	}
}

func (s *Server) message(c *Client, args []string) {
	if c.room == nil {
		c.msg("You must join a room to send a message. Use /join <room_name>")
		return
	}
	msg := strings.Join(args, " ")
	c.room.broadcast <- []byte(fmt.Sprintf("%s: %s", c.nick, msg))
}

// runRoom 是每个房间的广播中心
func (s *Server) runRoom(r *Room) {
	for msg := range r.broadcast {
		for client := range r.clients {
			client.send <- msg
		}
	}
}
