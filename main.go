package main

import (
	"github.com/gin-gonic/gin"
	"github.com/lithiumvalproate/freshman-3rd/gateway"
	"io"
	"log"
	"net/http"
)

func main() {
	r := gin.Default()

	// 统一转发所有API请求
	r.Any("/api/*proxyPath", func(c *gin.Context) {
		proxyPath := c.Param("proxyPath") // 例如 /im/message
		targetBase := gateway.GetBackendTarget(proxyPath)
		if targetBase == "" {
			c.JSON(http.StatusNotFound, gin.H{"error": "No backend matches this path"})
			return
		}

		// 构造目标URL
		targetURL := targetBase + proxyPath

		req, err := http.NewRequest(c.Request.Method, targetURL, c.Request.Body)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create proxy request"})
			return
		}
		// 复制Header
		for k, v := range c.Request.Header {
			for _, vv := range v {
				req.Header.Add(k, vv)
			}
		}
		client := &http.Client{}
		resp, err := client.Do(req)
		if err != nil {
			c.JSON(http.StatusBadGateway, gin.H{"error": "Backend service unreachable"})
			return
		}
		defer func() {
			if err := resp.Body.Close(); err != nil {
				log.Printf("Error closing response body: %v", err)
			}
		}()

		// 复制响应头
		for k, v := range resp.Header {
			for _, vv := range v {
				c.Writer.Header().Add(k, vv)
			}
		}
		c.Status(resp.StatusCode)
		if _, err := io.Copy(c.Writer, resp.Body); err != nil {
			log.Printf("Error copying response body: %v", err)
		}
	})

	log.Println("Gateway server starting on :8000")
	if err := r.Run(":8000"); err != nil {
		log.Fatal("Failed to start server:", err)
	}
}
