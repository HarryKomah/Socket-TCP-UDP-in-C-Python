#define HTTP_200_RESPONSE \
"HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\
\r\n"

#define HTML_200_MESSAGE \
"<html>\r\n\
<body>\r\n\
<h1> Hello </h1>\r\n\
</body>\r\n\
</html>\r\n"

#define HTTP_404_RESPONSE \
"HTTP/1.1 404 Not Found\r\n\
Content-Length: 230\r\n\
Connection: Closed\r\n\
Content-Type: text/html; charset=iso-8859-1\r\n"

#define HTML_404_MESSAGE \
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n\
<html>\r\n\
<head>\r\n\
   <title>404 Not Found</title>\r\n\
</head>\r\n\
<body>\r\n\
   <h1>Not Found</h1>\r\n\
   <p>The requested URL /t.html was not found on this server.</p>\r\n\
</body>\r\n\
</html>\r\n"

