#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include "tcpserver.h"
#include "tcpsocket.h"
#include "sfs.h"
#include <map>

class HttpServer;

class HttpServerTask
{
public:
    ~HttpServerTask();
  
private:
    friend class HttpServer;
    HttpServerTask(struct tcp_pcb *pcb, HttpServer *server);
    HttpServer *m_server;
    TcpSocket *m_socket;
    Sfs m_sfs;
    static std::map<ByteArray, ByteArray> m_mimeMap;
    
    bool m_busy = false;
    ByteArray method;
    ByteArray url;
//    vector<ByteArray> headers;
    ByteArray body;
    
    int m_taskId;
    void task();
    
    void stop();
    
    void onDisconnect();
    void onReadyRead();
};

class HttpQuery
{
public:
    HttpQuery(const ByteArray &ba);
    const ByteArray &url() const {return m_path;}
    const ByteArray &value(const ByteArray &key) const {return m_params.at(key);}
    const ByteArray &operator[](const ByteArray &key) const
    {
        static ByteArray ba;
        return m_params.count(key)? m_params.at(key): ba;
    }
    ByteArray &operator[](const ByteArray &key) {return m_params[key];}
//    ByteArray &operator[](const char *key) {return m_params[key];}
    bool hasValue(const ByteArray &key) const {return m_params.count(key);}
    
private:
    ByteArray m_path;
    std::map<ByteArray, ByteArray> m_params;
};

class HttpServer : public TcpServer
{
public:
    HttpServer(uint16_t port=80);
    void stop();
    
    Closure<bool (const HttpQuery &, ByteArray &)> onRequest;
  
protected:
    void incomingConnection(struct tcp_pcb *pcb);
    friend class HttpServerTask;
    void removeTask(HttpServerTask *task);
    
private:
    std::vector<HttpServerTask *> m_tasks;
};

#endif