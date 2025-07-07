package _package

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"strings"
	"sync"
	"sync/atomic"
)

// Client 结构体代表一个连接到服务器的客户端
type Client struct {
	conn net.Conn // 客户端的 TCP 连接
	nick string   // 客户端的昵称
	room *Room    // 客户端当前所在的聊天室
}

// Room 结构体代表一个聊天室
type Room struct {
	id      uint64 // 聊天室的唯一ID
	name    string
	members map[net.Addr]*Client // 聊天室内的成员
}

// Server 结构体是整个应用的核心，管理所有状态
type Server struct {
	rooms      map[string]*Room // 服务器上所有的聊天室
	mu         sync.Mutex       // 用于保护 rooms 的互斥锁
	nextRoomID uint64           // 下一个聊天室ID计数器
}

// NewServer 创建并返回一个新的 Server 实例
func NewServer() *Server {
	return &Server{
		rooms:      make(map[string]*Room),
		nextRoomID: 1, // 从1开始分配房间ID
	}
}

// HandleConnection 是为每个客户端连接启动的 goroutine
// 它负责处理该客户端的所有逻辑
func (s *Server) HandleConnection(conn net.Conn) {
	defer conn.Close()
	log.Printf("新客户端接入: %s", conn.RemoteAddr().String())

	// 初始化客户端
	client := &Client{
		conn: conn,
		nick: "anonymous", // 默认昵称
	}

	// 欢迎信息
	client.msg("欢迎来到 Go 聊天室!\n")
	client.msg("可用命令:\n")
	client.msg("  /nick <your_nickname>  - 设置你的昵称\n")
	client.msg("  /join <room_name>      - 加入或创建一个聊天室\n")
	client.msg("  /rooms                 - 查看所有可用聊天室\n")
	client.msg("  /roominfo              - 查看当前房间信息（包括ID）\n")
	client.msg("  /quit                  - 离开聊天室\n")

	// 创建一个读取器来处理来自客户端的输入
	reader := bufio.NewReader(conn)
	for {
		// 读取客户端发送的一行数据
		input, err := reader.ReadString('\n')
		if err != nil {
			// 如果是文件结束符(EOF)，说明客户端断开连接
			if err == io.EOF {
				log.Printf("客户端 %s 断开连接", conn.RemoteAddr().String())
			} else {
				log.Printf("读取客户端 %s 数据时出错: %v", conn.RemoteAddr().String(), err)
			}
			// 客户端断开后，需要将它从所在的聊天室移除
			s.onClientDisconnect(client)
			return
		}

		// 清理输入字符串
		input = strings.TrimSpace(input)

		// 解析命令
		if strings.HasPrefix(input, "/") {
			args := strings.Split(input, " ")
			cmd := strings.TrimSpace(args[0])

			switch cmd {
			case "/nick":
				if len(args) > 1 {
					s.setNick(client, args[1])
				} else {
					client.msg("用法: /nick <your_nickname>\n")
				}
			case "/join":
				if len(args) > 1 {
					s.joinRoom(client, args[1])
				} else {
					client.msg("用法: /join <room_name>\n")
				}
			case "/rooms":
				s.listRooms(client)
			case "/roominfo":
				s.showRoomInfo(client)
			case "/quit":
				s.quitRoom(client)
			default:
				client.msg(fmt.Sprintf("未知命令: %s\n", cmd))
			}
		} else {
			// 如果不是命令，就是一条聊天消息
			if client.room == nil {
				client.msg("你必须先加入一个聊天室才能发送消息。使用 /join <room_name>\n")
			} else {
				s.broadcast(client, fmt.Sprintf("%s: %s\n", client.nick, input))
			}
		}
	}
}

// generateRoomID 生成下一个唯一的房间ID
func (s *Server) generateRoomID() uint64 {
	return atomic.AddUint64(&s.nextRoomID, 1)
}

// setNick 设置客户端的昵称
func (s *Server) setNick(c *Client, nick string) {
	oldNick := c.nick
	c.nick = nick
	c.msg(fmt.Sprintf("你的昵称已从 %s 改为 %s\n", oldNick, nick))
}

// joinRoom 将客户端加入一个聊天室
func (s *Server) joinRoom(c *Client, roomName string) {
	s.mu.Lock()
	defer s.mu.Unlock()

	// 查找聊天室是否存在
	room, ok := s.rooms[roomName]
	if !ok {
		// 如果不存在，则创建一个新的
		roomID := s.generateRoomID()
		room = &Room{
			id:      roomID,
			name:    roomName,
			members: make(map[net.Addr]*Client),
		}
		s.rooms[roomName] = room
		log.Printf("创建新聊天室: %s (ID: %d)", roomName, roomID)
	}

	// 如果客户端已经在某个房间，先将他从中移除
	if c.room != nil {
		delete(c.room.members, c.conn.RemoteAddr())
	}

	// 将客户端加入新房间
	c.room = room
	room.members[c.conn.RemoteAddr()] = c
	c.msg(fmt.Sprintf("已加入聊天室: %s (ID: %d)\n", roomName, room.id))
	log.Printf("客户端 %s (%s) 加入了聊天室 %s (ID: %d)", c.conn.RemoteAddr(), c.nick, roomName, room.id)

	// 向房间里的其他人广播此人加入的消息
	s.broadcast(c, fmt.Sprintf("--- %s 加入了聊天室 ---\n", c.nick))
}

// listRooms 向客户端发送所有可用聊天室的列表
func (s *Server) listRooms(c *Client) {
	s.mu.Lock()
	defer s.mu.Unlock()

	if len(s.rooms) == 0 {
		c.msg("当前没有活动的聊天室。\n")
		return
	}

	var roomList []string
	for name, room := range s.rooms {
		roomList = append(roomList, fmt.Sprintf("%s (ID: %d)", name, room.id))
	}
	c.msg(fmt.Sprintf("可用聊天室: %s\n", strings.Join(roomList, ", ")))
}

// showRoomInfo 显示当前房间的详细信息
func (s *Server) showRoomInfo(c *Client) {
	if c.room == nil {
		c.msg("你不在任何聊天室中。\n")
		return
	}

	s.mu.Lock()
	memberCount := len(c.room.members)
	s.mu.Unlock()

	c.msg(fmt.Sprintf("当前房间信息:\n"))
	c.msg(fmt.Sprintf("  名称: %s\n", c.room.name))
	c.msg(fmt.Sprintf("  ID: %d\n", c.room.id))
	c.msg(fmt.Sprintf("  成员数量: %d\n", memberCount))
}

// quitRoom 将客户端从当前聊天室移除
func (s *Server) quitRoom(c *Client) {
	if c.room == nil {
		c.msg("你不在任何聊天室中。\n")
		return
	}

	s.mu.Lock()
	defer s.mu.Unlock()

	roomName := c.room.name
	roomID := c.room.id
	// 从聊天室成员列表中删除
	delete(c.room.members, c.conn.RemoteAddr())
	c.room = nil
	c.msg(fmt.Sprintf("你已离开聊天室 %s (ID: %d)\n", roomName, roomID))
	log.Printf("客户端 %s (%s) 离开了聊天室 %s (ID: %d)", c.conn.RemoteAddr(), c.nick, roomName, roomID)

	// 向房间里的其他人广播此人离开的消息
	s.broadcast(c, fmt.Sprintf("--- %s 离开了聊天室 ---\n", c.nick))
}

// onClientDisconnect 处理客户端断开连接的清理工作
func (s *Server) onClientDisconnect(c *Client) {
	if c.room != nil {
		s.mu.Lock()
		defer s.mu.Unlock()

		// 从聊天室成员列表中删除
		delete(c.room.members, c.conn.RemoteAddr())

		// 如果房间空了，可以选择删除房间（这里为了简单，暂时不删除）
		// if len(c.room.members) == 0 {
		// 	delete(s.rooms, c.room.name)
		// }

		// 向房间里的其他人广播此人离开的消息
		s.broadcast(c, fmt.Sprintf("--- %s 断开了连接 ---\n", c.nick))
	}
}

// broadcast 将消息发送给指定客户端所在聊天室的所有成员
func (s *Server) broadcast(sender *Client, message string) {
	if sender.room == nil {
		return
	}

	// 注意：这里没有加锁，是因为我们假设 broadcast 调用时，外部逻辑（如 joinRoom, quitRoom）
	// 已经处理好了锁。在消息发送期间，我们不希望阻塞整个服务器。
	// 这是一个简化的设计，更健壮的设计可能会使用 channel。
	for addr, member := range sender.room.members {
		// 不把消息发回给发送者自己
		if sender.conn.RemoteAddr() != addr {
			member.msg(message)
		}
	}
}

// msg 是一个辅助函数，用于向客户端发送消息
func (c *Client) msg(message string) {
	_, err := c.conn.Write([]byte(message))
	if err != nil {
		log.Printf("向客户端 %s 发送消息失败: %v", c.conn.RemoteAddr(), err)
	}
}
