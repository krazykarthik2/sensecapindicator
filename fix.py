import re

f=open('examples/indicator_openai/main/controller/chess_engine.cpp', 'r').read()

mock = r"""
#include <string>
#include <ctype.h>
#include <stdlib.h>

class String : public std::string {
public:
    String() : std::string() {}
    String(const char* s) : std::string(s) {}
    String(const std::string& s) : std::string(s) {}
    String(int v) : std::string(std::to_string(v)) {}
    
    int indexOf(const char* s, int pos=0) const { 
        size_t p = find(s, pos); 
        return p == std::string::npos ? -1 : (int)p; 
    }
    int indexOf(char c, int pos=0) const { 
        size_t p = find(c, pos); 
        return p == std::string::npos ? -1 : (int)p; 
    }
    String substring(int start) const { return substr(start); }
    String substring(int start, int end) const { return substr(start, end - start); }
    int toInt() const { return atoi(c_str()); }
    void toUpperCase() { for(auto &c : *this) c = toupper(c); }
    void trim() {
        size_t start = find_first_not_of(" \t\r\n");
        if(start == std::string::npos) { clear(); return; }
        size_t end = find_last_not_of(" \t\r\n");
        *this = substr(start, end - start + 1);
    }
    int length() const { return std::string::length(); }
    char charAt(int i) const { return (*this)[i]; }
    String& operator+=(const String& other) { std::string::operator+=(other); return *this; }
    String& operator+=(const char* other) { std::string::operator+=(other); return *this; }
    bool operator==(const char* other) const { return std::string(*this) == other; }
    bool operator==(const String& other) const { return std::string(*this) == other; }
};
inline String operator+(const char* a, const String& b) { String r=a; r+=b; return r; }
inline String operator+(const String& a, const String& b) { String r=a; r+=b; return r; }
inline String operator+(const String& a, const char* b) { String r=a; r+=b; return r; }

template<class T> String to_String(T val) { return String(std::to_string(val)); }

struct SerialFake {
    void begin(int baud) {}
    template<typename T> void print(T t) {}
    template<typename T> void println(T t) {}
    void println() {}
    int available() { return 0; }
    char read() { return 0; }
    String readString() { return ""; }
    operator bool() const { return true; }
    bool operator!() const { return false; }
};
extern SerialFake Serial;

#define PROGMEM
#define F(X) X
#define B1111 15
#define delayMicroseconds(x) delay((x)/1000)

static uint32_t millis() { return esp_timer_get_time() / 1000; }
static uint32_t micros() { return esp_timer_get_time(); }
static void delay(uint32_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }
long random(long max) { return rand() % max; }
long random(long min, long max) { return min + rand() % (max - min); }
"""

# Reset to git original
import subprocess
subprocess.run(["C:/Users/248w5a5404/AppData/Local/Programs/Git/cmd/git.exe", "checkout", "examples/indicator_openai/main/controller/chess_engine.cpp"])
f=open('examples/indicator_openai/main/controller/chess_engine.cpp', 'r').read()

if '#include <string>' not in f:
    f = '#include <string>\n#include <iostream>\n#include "freertos/FreeRTOS.h"\n#include "freertos/task.h"\n#include "esp_timer.h"\n' + f

f = f.replace('#include <Arduino.h>', '')
f = re.sub(r'using namespace std;', 'using namespace std;\n' + mock + '\nSerialFake Serial;\n', f)
f = f.replace('boolean ', 'bool ')
f = f.replace('to_String(pos[0].best.weight / 100., 2)', 'to_String(pos[0].best.weight / 100.)')

# Just literally delete the entire setup and loop contents so they don't break
f = re.sub(r'void setup\(\) \{.*?\}', 'void setup() {}', f, flags=re.DOTALL)
f = re.sub(r'void loop\(\) \{.*?\}', 'void loop() {}', f, flags=re.DOTALL)

open('examples/indicator_openai/main/controller/chess_engine.cpp', 'w').write(f)
