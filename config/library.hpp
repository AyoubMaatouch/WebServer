#pragma once

/*
**
* HEADERS
**
*/

#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config_file.hpp"
#include "request.hpp"
#include "util.hpp"

/*
**
* VARIABLES
**
*/

#define BODY_CONTENT_FILE "./body_content"