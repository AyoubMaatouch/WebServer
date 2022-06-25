#include "helper_tools.hpp"

// function to determine Content-Type based on file extension using a map 

std::string	 get_content_type(std::string s)
{
	static std::map<std::string,std::string> mt;

	if (mt.size())
	{
		std::string ext = s.substr(s.find_last_of('.')+1);
		std::map<std::string,std::string>::iterator it = mt.find(ext);
		return it != mt.end() ? it->second : "application/octet-stream";
	}

	mt["html"] = "text/html";
	mt["htm"] = "text/html";
	mt["shtml"] = "text/html";
	mt["css"] = "text/css";
	mt["xml"] = "text/css";
	mt["gif"]= "image/gif";
	mt["jpeg"]= "image/jpeg";
	mt["jpg"]= "image/jpeg";
	mt["png"]= "image/png";
	mt["js"]= "application/javascript";
	mt["atom"]= "application/atom+xml";
	mt["rss"]= "application/rss+xml";
	
	mt["mml"]= "text/mathml";
	mt["txt"]= "text/plain";
	mt["jad"]= "text/vnd.sun.j2me.app-descriptor";
	mt["wml"]= "text/vnd.wap.wml";
	mt["htc"]= "text/x-component";
	
	mt["png"]= "image/png";
	mt["tif"]= "image/tiff";
	mt["tiff"]= "image/tiff";
	mt["wbmp"]= "image/vnd.wap.wbmp";
	mt["ico"]= "image/x-icon";
	mt["jng"]= "image/x-jng";
	mt["bmp"]= "image/x-ms-bmp";
	mt["svg"]= "image/svg+xml";
	mt["svgz"]= "image/svg+xml";
	mt["webp"]= "image/webp";

	mt["woff"] = "application/font-woff";
	mt["jar"] = "application/java-archive";
	mt["war"] = "application/java-archive";
	mt["ear"] = "application/java-archive";
	mt["json"] = "application/json";
	mt["hqx"] = "application/mac-binhex40";
	mt["doc"] = "application/msword";
	mt["pdf"] = "application/pdf";
	mt["ps"] = "application/postscript";
	mt["eps"] = "application/postscript";
	mt["ai"] = "application/postscript";
	mt["rtf"] = "application/rtf";
	mt["m3u8"] = "application/vnd.apple.mpegurl";
	mt["xls"] = "application/vnd.ms-excel";
	mt["eot"] = "application/vnd.ms-fontobject";
	mt["ppt"] = "application/vnd.ms-powerpoint";
	mt["wmlc"] = "application/vnd.wap.wmlc";
	mt["kml"] = "application/vnd.google-earth.kml+xml";
	mt["kmz"] = "application/vnd.google-earth.kmz";
	mt["7z"] = "application/x-7z-compressed";
	mt["cco"] = "application/x-cocoa";
	mt["jardiff"] = "application/x-java-archive-diff";
	mt["jnlp"] = "application/x-java-jnlp-file";
	mt["run"] = "application/x-makeself";
	mt["pl"] = "application/x-perl";
	mt["pm"] = "application/x-perl";
	mt["prc"] = "application/x-pilot";
	mt["pdb"] = "application/x-pilot";
	mt["rar"] = "application/x-rar-compressed";
	mt["rpm"] = "application/x-redhat-package-manager";
	mt["sea"] = "application/x-sea";
	mt["swf"] = "application/x-shockwave-flash";
	mt["sit"] = "application/x-stuffit";
	mt["tcl"] = "application/x-tcl";
	mt["tk"] = "application/x-tcl";
	mt["der"] = "application/x-x509-ca-cert";
	mt["pem"] = "application/x-x509-ca-cert";
	mt["cert"] = "application/x-x509-ca-cert";
	mt["xpi"] = "application/x-xpinstall";
	mt["xhtml"] = "application/xhtml+xml";
	mt["xspf"] = "application/xspf+xml";
	mt["zip"] = "application/zip";

	mt["bin"] = "application/octet-stream";
	mt["exe"] = "application/octet-stream";
	mt["dll"] = "application/octet-stream";
	mt["deb"] = "application/octet-stream";
	mt["dmg"] = "application/octet-stream";
	mt["iso"] = "application/octet-stream";
	mt["img"] = "application/octet-stream";
	mt["msi"] = "application/octet-stream";
	mt["msp"] = "application/octet-stream";
	mt["msm"] = "application/octet-stream";

	mt["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mt["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mt["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

	mt["mid"] = "audio/midi";
	mt["midi"] = "audio/midi";
	mt["kar"] = "audio/midi";
	mt["mp3"] = "audio/mpeg";
	mt["ogg"] = "audio/ogg";
	mt["m4a"] = "audio/x-m4a";
	mt["ra"] = "audio/x-realaudio";

	mt["3gpp"] = "video/3gpp";
	mt["3gp"] = "video/3gpp";
	mt["ts"] = "video/mp2t";
	mt["mp4"] = "video/mp4";
	mt["mpeg"] = "video/mpeg";
	mt["mpg"] = "video/mpeg";
	mt["mov"] = "video/quicktime";
	mt["webm"] = "video/webm";
	mt["flv"] = "video/x-flv";
	mt["m4v"] = "video/x-m4v";
	mt["mng"] = "video/x-mng";
	mt["asx"] = "video/x-ms-asf";
	mt["asf"] = "video/x-ms-asf";
	mt["wmv"] = "video/x-ms-wmv";
	mt["avi"] = "video/x-msvideo";

	// CGI extentions
	mt["php"] = "cgi/php";
	mt["perl"] = "cgi/perl";
	mt["py"] = "cgi/py";

	std::string ext = s.substr(s.find_last_of('.') + 1);
	std::map<std::string, std::string>::iterator it = mt.find(ext);
	return it != mt.end() ? it->second : "application/octet-stream";
}

std::string to_string(int i)
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

bool replace_in_uri(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


std::string         get_index(std::string& path, std::vector<std::string>& index)
{	
  	struct stat buffer;   
	std::string status = "";

   for (size_t i = 0 ; i < index.size(); i++)
  	{
		if (stat ((path +  index[i]).c_str(), &buffer) == 0)
			return index[i];
		if (errno == EACCES)
			{
				status = "403";
				break;
			}
			else if (errno == ENOENT)
				status = "404";
	}	
	throw status;
	return index[0];
}
bool         get_per(std::string path)
{	
  	struct stat buffer;   
	std::string status = "";

	stat ((path).c_str(), &buffer);
	if (errno == EACCES)
		{
			status = "403";
			throw status ;
		}
		else if (errno == ENOENT)
		{
				status = "404";
				throw status ;
		}
	return true;
}

bool isdir (std::string path)
{
//     struct stat st;
//     if(stat(path.c_str(),&st) == 0)
//         if((st.st_mode) & (S_IFDIR != 0))
//             return true;
//         return false;
		struct stat info;

			int statRC = stat( path.c_str(), &info );
			if( statRC != 0 )
			{
				if (errno == ENOENT)  { return 0; } // something along the path does not exist
				if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
				return -1;
			}

			return ( info.st_mode & S_IFDIR ) ? 1 : 0;
}