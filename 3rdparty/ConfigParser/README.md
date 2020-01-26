## ConfigParser
A Simple Configuration File Parser in C++11

## Usage
### 1. Config file
    # comment
    key = value
    
    # window options
    width = 640
    height = 480
    title = hello, world!
    vsync = true
    scale = 1.5
    
    # comma separated values
    values = 2, 3, 4

### 2. Code
    unsigned int width, height;
    std::string title;
    bool vsync;
    float scale;
    
    ConfigParser config("config.txt");
    config.get("width", width);
    config.get("height", height);
    config.get("title", title);
    config.get("vsync", vsync);
    config.get("scale", scale);
    
    int x, y, z;
    config.get("values", x, y, z);
    
    config.set("values", 5, 6, 7, 8, 9); // change values
    config.set("window_size", width, height); // create new option
    
    std::vector<int> vector;
    config.get("values", vector);
