# Config File
## Request
> Get the Request object
```cpp
Request req_obj("request string");
```
> The object ```req_obj``` has 2 object
```cpp
Header header = req_obj.header;
Body body = req_obj.body;
```
```cpp
// example
std::string method = header.method;
std::vector<std::string>	accept_language = body.accept_language;
```
**Please check the file ```request.hpp```**