package src

import (
	"fmt"
	"log"
	"strings"
	"sync"

	"github.com/gorilla/websocket"
	"github.com/lithiumvalproate/freshman-3rd/Go/imclient" // Import the new imclient package
)

// Client represents a connected client
type Client struct {
	conn          *websocket.Conn // Client's WebSocket connection
	nick          string          // Client's nickname
	currentRoomID uint64          // ID of the room the client is currently in
	participantID string          // Unique ID for the participant in the C++ IM system (e.g., RemoteAddr().String())
}

// A map to store active clients, keyed by their participantID
var activeClients = make(map[string]*Client)
var clientsMutex sync.Mutex // Protects activeClients

// NewServer creates and returns a new Server instance.
// It also initializes the imclient.
func NewServer() *Server {
	// Initialize the C++ IM client with a callback for incoming messages
	imclient.Init(func(participantID, messageContent string) {
		clientsMutex.Lock()
		client, ok := activeClients[participantID]
		clientsMutex.Unlock()

		if ok {
			client.msg(messageContent)
		} else {
			log.Printf("Received message for unknown participant: %s - %s", participantID, messageContent)
		}
	})

	return &Server{} // Server struct is now empty as IM logic is in C++
}

// Server struct is now minimal, as IM logic is handled by C++
type Server struct{}

// HandleConnection is the goroutine for each client connection.
// It handles all logic for that client.
func (s *Server) HandleConnection(conn *websocket.Conn) {
	defer conn.Close()

	participantID := conn.RemoteAddr().String()
	log.Printf("New client connected: %s", participantID)

	client := &Client{
		conn:          conn,
		nick:          "anonymous", // Default nickname
		participantID: participantID,
	}

	clientsMutex.Lock()
	activeClients[participantID] = client
	clientsMutex.Unlock()

	client.msg("Welcome to the Go Chat Room (powered by C++ IM)!\n")
	client.msg("Available commands:\n")
	client.msg("  /nick <your_nickname>  - Set your nickname\n")
	client.msg("  /join <room_name>      - Join or create a chat room\n")
	client.msg("  /rooms                 - View all available chat rooms\n")
	client.msg("  /roominfo              - View current room info (including ID)\n")
	client.msg("  /quit                  - Leave the chat room\n")

	for {
		messageType, p, err := conn.ReadMessage()
		if err != nil {
			if websocket.IsCloseError(err, websocket.CloseGoingAway, websocket.CloseNormalClosure) {
				log.Printf("Client %s disconnected normally", participantID)
			} else {
				log.Printf("Error reading from client %s: %v", participantID, err)
			}
			s.onClientDisconnect(client)
			return
		}

		if messageType == websocket.TextMessage {
			input := strings.TrimSpace(string(p))

			if strings.HasPrefix(input, "/") {
				args := strings.Split(input, " ")
				cmd := strings.TrimSpace(args[0])

				switch cmd {
				case "/nick":
					if len(args) > 1 {
						client.setNick(args[1])
					} else {
						client.msg("Usage: /nick <your_nickname>\n")
					}
				case "/join":
					if len(args) > 1 {
						client.joinRoom(args[1])
					} else {
						client.msg("Usage: /join <room_name>\n")
					}
				case "/rooms":
					client.listRooms()
				case "/roominfo":
					client.showRoomInfo()
				case "/quit":
					client.quitRoom()
				default:
					client.msg(fmt.Sprintf("Unknown command: %s\n", cmd))
				}
			} else {
				if client.currentRoomID == 0 {
					client.msg("You must join a chat room first to send messages. Use /join <room_name>\n")
				} else {
					// Send message via C++ IM system
					success := imclient.SendMessage(client.currentRoomID, client.participantID, fmt.Sprintf("%s: %s", client.nick, input))
					if !success {
						client.msg("Failed to send message.\n")
					}
				}
			}
		}
	}
}

// onClientDisconnect handles cleanup when a client disconnects
func (s *Server) onClientDisconnect(c *Client) {
	if c.currentRoomID != 0 {
		imclient.LeaveRoom(c.currentRoomID, c.participantID)
	}

	clientsMutex.Lock()
	delete(activeClients, c.participantID)
	clientsMutex.Unlock()

	log.Printf("Client %s (%s) disconnected.", c.participantID, c.nick)
}

// Client methods (now interacting with imclient)

func (c *Client) setNick(newNick string) {
	oldNick := c.nick
	c.nick = newNick
	c.msg(fmt.Sprintf("Your nickname changed from %s to %s\n", oldNick, newNick))
	// Note: Nickname change is only client-side for now.
	// If C++ IM needs to know about nickname changes, a new CGO function would be needed.
}

func (c *Client) joinRoom(roomName string) {
	// Try to get room ID by name first
	roomID := imclient.GetRoomID(roomName)
	if roomID == 0 {
		// If room doesn't exist, create it
		roomID = imclient.CreateRoom(roomName)
		if roomID == 0 {
			c.msg(fmt.Sprintf("Failed to create room %s.\n", roomName))
			return
		}
		log.Printf("Created new room: %s (ID: %d)", roomName, roomID)
	}

	// Leave current room if any
	if c.currentRoomID != 0 {
		imclient.LeaveRoom(c.currentRoomID, c.participantID)
	}

	// Join the new room
	success := imclient.JoinRoom(roomID, c.participantID, c.nick)
	if success {
		c.currentRoomID = roomID
		c.msg(fmt.Sprintf("Joined chat room: %s (ID: %d)\n", roomName, roomID))
		log.Printf("Client %s (%s) joined room %s (ID: %d)", c.participantID, c.nick, roomName, roomID)
	} else {
		c.msg(fmt.Sprintf("Failed to join room %s.\n", roomName))
	}
}

func (c *Client) listRooms() {
	roomIDs := imclient.ListRoomIDs()
	if len(roomIDs) == 0 {
		c.msg("No active chat rooms.\n")
		return
	}

	var roomList []string
	for _, id := range roomIDs {
		name := imclient.GetRoomName(id)
		if name != "" {
			roomList = append(roomList, fmt.Sprintf("%s (ID: %d)", name, id))
		}
	}
	c.msg(fmt.Sprintf("Available chat rooms: %s\n", strings.Join(roomList, ", ")))
}

func (c *Client) showRoomInfo() {
	if c.currentRoomID == 0 {
		c.msg("You are not in any chat room.\n")
		return
	}

	roomName := imclient.GetRoomName(c.currentRoomID)
	if roomName == "" {
		c.msg("Error: Could not retrieve current room info.\n")
		return
	}

	c.msg(fmt.Sprintf("Current room info:\n"))
	c.msg(fmt.Sprintf("  Name: %s\n", roomName))
	c.msg(fmt.Sprintf("  ID: %d\n", c.currentRoomID))
	// Member count is not directly exposed by C++ IM bridge yet.
	// A new CGO function would be needed for this.
	c.msg(fmt.Sprintf("  Member count: (not available via C++ bridge yet)\n"))
}

func (c *Client) quitRoom() {
	if c.currentRoomID == 0 {
		c.msg("You are not in any chat room.\n")
		return
	}

	oldRoomID := c.currentRoomID
	roomName := imclient.GetRoomName(oldRoomID)
	success := imclient.LeaveRoom(oldRoomID, c.participantID)
	if success {
		c.currentRoomID = 0
		c.msg(fmt.Sprintf("You have left chat room %s (ID: %d)\n", roomName, oldRoomID))
		log.Printf("Client %s (%s) left room %s (ID: %d)", c.participantID, c.nick, roomName, oldRoomID)
	} else {
		c.msg(fmt.Sprintf("Failed to leave room %s.\n", roomName))
	}
}

// msg is a helper function to send messages to the client
func (c *Client) msg(message string) {
	err := c.conn.WriteMessage(websocket.TextMessage, []byte(message))
	if err != nil {
		log.Printf("Failed to send message to client %s: %v", c.conn.RemoteAddr(), err)
	}
}
