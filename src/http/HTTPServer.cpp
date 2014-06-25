#include "HTTPServer.h"

using boost::asio::ip::tcp;
using namespace std;

static string s_webPath;
 
HTTPServer::HTTPServer(string str_ip, string str_port, string web_path)
{    
    s_webPath = web_path;
    
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(str_ip, str_port);
    tcp::resolver::iterator it = resolver.resolve(query);
    
    m_acceptor = new tcp::acceptor(io_service, *it, true);
        
    start_accept();
}

HTTPServer::~HTTPServer() {
    
}

void HTTPServer::start_accept()
{
    HTTPConnection::pointer new_connection = HTTPConnection::create(io_service);
    
	m_acceptor->async_accept(new_connection->socket(), 
                            boost::bind(&HTTPServer::handle_accept,
	                         this, new_connection, boost::asio::placeholders::error));
}

void HTTPServer::handle_accept(HTTPConnection::pointer new_connection,           
                            const boost::system::error_code& ec)
{
    if(!ec)
    {
        new_connection->start();
    }
	start_accept();
}

void HTTPConnection::start ()
{
    m_socket.async_read_some( boost::asio::buffer(m_request, MAX_HTTP_DATA_LENGTH), 
                               boost::bind(&HTTPConnection::handle_read, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void HTTPConnection::handle_read(const boost::system::error_code& /* ec */,
                    size_t /* bytes_transferred*/)
{
    
    std::vector<std::string> headers;
    boost::split(headers, m_request, boost::is_any_of("\r\n"));
    
    std::vector<std::string> topLine;
    boost::split(topLine, headers[0], boost::is_any_of(" "));
    
    string requestType = topLine[0];
    string url = topLine[1];
    
    std::vector<std::string> urlVsData;
    boost::split(urlVsData, url, boost::is_any_of("?"));
    
    std::vector<std::string> urlParts;
    boost::split(urlParts, urlVsData[0], boost::is_any_of("/"));
    
    std::vector<std::string> dataParts;
    
    if(urlVsData.size() > 1) {
        boost::split(dataParts, urlVsData[1], boost::is_any_of("&"));
    }
    
    errorMimeResponse_t resp = HTTPServer::serve404("Malformed request");
    
    if(urlParts[1] == "admin") {
        resp = HTTPServer::handleAppPage(url);
    } else if(urlParts[1] == "request" && urlVsData.size() > 1) {
        // create a map from URL parameters of form ?a=b&c=d&e=f 
        
        map <string, string> parameters;
        
        for(int i = 0; i < (int) dataParts.size(); ++i) {
            std::vector<std::string> dparts;
            boost::split(dparts, dataParts[i], boost::is_any_of("="));
            parameters[dparts[0]] = dparts[1];
        }
        
        resp = HTTPServer::handleRequest(urlParts[2], parameters, requestType);
    }
    
    stringstream s;
    s << "HTTP/1.1 " << std::get<0>(resp) << " OK\n"
        << "Server: Astron Web Administration\n"
        << "Content-Type: " << std::get<1>(resp) << "\n"
        << "Content-Length: " << std::get<2>(resp).length() << "\n"
        << "Connection: close\n\n" 
            
        << std::get<2>(resp) << "\n";
    
    boost::asio::async_write(m_socket, boost::asio::buffer(s.str()), 
        boost::bind(&HTTPConnection::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

errorMimeResponse_t HTTPServer::serve404(std::string info) {
    stringstream message;
    
    message << "<h1>404 File Not Found</h1>";
    
    if(info.length()) {
        message << "<h2>Additional Info</h2>" << info;
    }
    
    return std::make_tuple(404, "text/html", message.str());
}

errorMimeResponse_t HTTPServer::handleRequest(std::string requestName, std::map <std::string, std::string> params, std::string method) {
                                                                 
    cout << "Request " << requestName << " made: password " << params["password"] << "\n";
    
    return serveRequest(200, "{\"success\" : \"1\"}");
}

errorMimeResponse_t HTTPServer::serveRequest(int error, std::string request) {
    return std::make_tuple(error, "text/plain", request);
}

errorMimeResponse_t HTTPServer::serveFile(std::string mimeType, std::string content) {
    return std::make_tuple(200, mimeType, content);
}

errorMimeResponse_t HTTPServer::handleAppPage(std::string url) {
    cout << "Serve page " << url << "\n";
    
    if(boost::find_first(url, "..") || boost::find_first(url, "~")) {
        cout << "LIKELY HACKING ATTEMPT" << "\n";
        return serveFile("text/html", "<script>alert('Stop hacking');</script>"); // TODO: log IP
    }
    
    if(s_webPath == "FROZEN") {
        return serve404("Frozen website not supported yet! Sorry!");
    }
    
    string mimeType = "text/plain";
    
    // split file extension from rest of URL
    
    std::vector<std::string> fileParts;
    boost::split(fileParts, url, boost::is_any_of(".")) ;
    
    if(fileParts.size() > 1) {
        // determine mime type by extension
        // switch statements can't use strings, unfortunately
        
        if(fileParts[1] == "html") mimeType = "text/html";
        if(fileParts[1] == "css") mimeType = "text/css";
        
        if(fileParts[1] == "js") mimeType = "application/x-javascript";
        
        if(fileParts[1] == "bmp") mimeType = "image/bmp";
        if(fileParts[1] == "jpg") mimeType = "image/jpeg";
        if(fileParts[1] == "png") mimeType = "image/png";
        
        if(fileParts[1] == "wav") mimeType = "audio/x-wav";
        if(fileParts[1] == "mp3") mimeType = "audio/mpeg";
    }
    
    
    stringstream pathStream;
    pathStream << s_webPath << url;
    
    string path = pathStream.str();
    
    if(!boost::filesystem::exists(path))
    {
        return serve404();
    } 
    
    std::ifstream stream;
    stream.open(path, std::ios::in);
    
    if(!stream)
    {
        return serve404("Stream null");
    } 
    
    stringstream s;
    s << stream.rdbuf();
    
    return serveFile(mimeType, s.str());
}