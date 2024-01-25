#include "httpserver.h"

std::map<ByteArray, ByteArray> HttpServerTask::m_mimeMap = 
{
    {"txt", "text/plain"},
    {"html", "text/html"},
    {"htm", "text/html"},
    {"css", "text/css"},
    {"js", "text/javascript"},
    {"json", "application/json"},
    {"bmp", "image/bmp"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpg"},
    {"png", "image/png"},
    {"gif", "image/gif"},
};

HttpServerTask::HttpServerTask(struct tcp_pcb *pcb, HttpServer *server) :
    m_server(server)
{
    m_socket = new TcpSocket(pcb);
    m_socket->onDisconnect = EVENT(&HttpServerTask::onDisconnect);
    m_socket->onReadyRead = EVENT(&HttpServerTask::onReadyRead);
//    m_taskId = stmApp()->registerTaskEvent(EVENT(&HttpServerTask::task));
}

HttpServerTask::~HttpServerTask()
{
//    stmApp()->unregisterTaskEvent(m_taskId);
    delete m_socket;
}

void HttpServerTask::onDisconnect()
{
    m_server->removeTask(this);
}

void HttpServerTask::stop()
{
    m_socket->close();
}

void HttpServerTask::task()
{
    if (m_busy)
    {
        ByteArray resp;
    
        if (url == "/")
            url = "/index.html";
        
        ByteArray mime = "text/html";
        
        if (m_sfs.open(url.toStdString()))
        {
            const Sfs::Entry *e = m_sfs.entry();
            body = ByteArray::fromRawData(e->data(), e->size());
            
            ByteArray ext;
            int dot_idx = url.lastIndexOf('.');
            if (dot_idx >= 0)
                ext = url.mid(dot_idx + 1);
            if (m_mimeMap.count(ext))
                mime = m_mimeMap[ext];
            resp = "HTTP/1.1 200 OK\r\n";
        }
        else if (m_server->onRequest && m_server->onRequest(url, body))
        {
            resp = "HTTP/1.1 200 OK\r\n";
        }
        else
        {
            resp = "HTTP/1.1 404 Not found\r\n";
            body = "<h1>File not found</h1>";
        }
        
        char buf[32];
        resp.append("Server: ControlBoard\r\n");
        sprintf(buf, "Content-Length: %d\r\n", body.size());
        resp.append(buf);
        resp.append("Content-Type: " + mime + "\r\n");
        resp.append("Connection: Keep-Alive\r\n");
        resp.append("\r\n"); // header/body delimiter
        m_socket->write(resp);
        m_socket->write(body);
        body.clear();
        
        m_busy = false;
    }
}

void HttpServerTask::onReadyRead()
{
    if (m_busy)
    {
        m_socket->write("HTTP/1.1 202 Accepted\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n");
        return;
    }
        
//    headers.clear();
    
    while (!m_socket->atEnd() && m_socket->canReadLine())
    {
        ByteArray header = m_socket->readLine();
        header.chop(2); // remove \r\n
        if (header.isEmpty())
            break; // all headers are read
        if (header.startsWith("GET"))
        {
            method = "GET";
            header.remove(0, 4);
            int end = header.indexOf(' ');
            header.truncate(end);
            url = ByteArray::fromPercentEncoding(header);
        }
        else if (header.startsWith("POST"))
        {
            method = "POST";
            header.remove(0, 5);
            int end = header.indexOf(' ');
            header.truncate(end);
            url = ByteArray::fromPercentEncoding(header);
        }
        else
        {
//            headers.push_back(header);
        }
//        printf("%s\n", header.toStdString().c_str());
    }
    
    /*ByteArray body =*/ m_socket->readAll();
    
    m_busy = true;
    task();
}
//---------------------------------------------------------------------------

HttpServer::HttpServer(uint16_t port) :
    TcpServer()
{
    listen(port);
}

void HttpServer::stop()
{
    close();
    for (int i=0; i<m_tasks.size(); i++)
        m_tasks[i]->stop(); // should call HttpServerTask::onDisconnect, then HttpServer::removeTask
}

void HttpServer::incomingConnection(struct tcp_pcb *pcb)
{
    HttpServerTask *task = new HttpServerTask(pcb, this);
    m_tasks.push_back(task);
}

void HttpServer::removeTask(HttpServerTask *task)
{
    for (std::vector<HttpServerTask *>::iterator it = m_tasks.begin(); it != m_tasks.end(); it++)
    {
        if (*it == task)
        {
            m_tasks.erase(it);
            break;
        }
    }
    delete task;
}
//---------------------------------------------------------------------------

HttpQuery::HttpQuery(const ByteArray &ba)
{
    std::map<ByteArray, ByteArray> query;
    int idx = ba.indexOf('?');
    if (idx)
    {
        m_path = ba.left(idx);
        while (idx < ba.size())
        {
            idx++;
            int del = ba.indexOf('=', idx);
            int end = ba.indexOf('&', idx);
            if (end < 0)
                end = ba.size();
            if (del < 0)
                del = end;
            ByteArray key = ba.mid(idx, del - idx);
            ByteArray value;
            del++;
            if (del < end)
                value = ba.mid(del, end - del);
            value = ByteArray::fromPercentEncoding(value);
            m_params[key] = value;
            idx = end;
        }
    }
    else
    {
        m_path = ba;
    }
}