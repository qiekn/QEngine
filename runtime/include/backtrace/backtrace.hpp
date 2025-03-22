#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <cxxabi.h>
#include <map>
#include <mutex>

#include <dlfcn.h>

namespace rttr {
    class type;
    namespace detail {
        class type_data;
    }
}

namespace zeytin {

template<typename T>
bool is_memory_readable(const T* ptr, size_t len = sizeof(T)) {
    if (!ptr) return false;
    
    volatile const char* p = reinterpret_cast<volatile const char*>(ptr);
    
    try {
        unsigned char dummy = 0;
        for (size_t i = 0; i < len; i++) {
            dummy ^= p[i];
        }
        return true;
    } catch (...) {
        return false;
    }
}

class Backtrace {
public:
    static void install_handlers() {
        signal(SIGSEGV, signal_handler); 
        signal(SIGABRT, signal_handler);
        signal(SIGFPE, signal_handler);  
        signal(SIGILL, signal_handler); 
        std::cout << "Enhanced backtrace handlers installed" << std::endl;
    }
    
    static void register_watch_address(const void* ptr, size_t size, const std::string& name) {
        std::lock_guard<std::mutex> lock(watch_mutex);
        watch_addresses[ptr] = {size, name};
    }
    
    static void unregister_watch_address(const void* ptr) {
        std::lock_guard<std::mutex> lock(watch_mutex);
        watch_addresses.erase(ptr);
    }
    
    static void check_rttr_type(const rttr::type* type_ptr, FILE* output) {
        if (!type_ptr) {
            fprintf(output, "RTTR type is null\n");
            return;
        }
        
        fprintf(output, "Examining RTTR type at %p\n", type_ptr);
        
        const void* type_data_ptr = nullptr;
        memcpy(&type_data_ptr, reinterpret_cast<const char*>(type_ptr), sizeof(void*));
        
        fprintf(output, "  Type data: %p\n", type_data_ptr);
        
        if (!is_memory_readable(reinterpret_cast<const char*>(type_data_ptr), 8)) {
            fprintf(output, "  ERROR: Type data appears to be corrupted or points to invalid memory\n");
            fprintf(output, "  This is likely a use-after-free or memory corruption issue\n");
        }
    }

    static void print_backtrace(FILE* output = stderr) {
        const int max_frames = 128;
        void* callstack[max_frames];
        int frames = backtrace(callstack, max_frames);
        
        fprintf(output, "\n=== BACKTRACE ===\n");
        
        char** symbols = backtrace_symbols(callstack, frames);
        if (symbols) {
            fprintf(output, "Raw backtrace:\n");
            for (int i = 0; i < frames; i++) {
                fprintf(output, "  %s\n", symbols[i]);
            }
            free(symbols);
        }
        
        fprintf(output, "\nDetailed backtrace:\n");
        
        char exe_path[1024];
        ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
        if (len != -1) {
            exe_path[len] = '\0';
            
            for (int i = 0; i < frames; i++) {
                if (i < 1) continue;
                
                Dl_info info;
                if (dladdr(callstack[i], &info)) {
                    const char* object_file = info.dli_fname;
                    void* relative_addr = (void*)((char*)callstack[i] - (char*)info.dli_fbase);
                    
                    const char* filename = strrchr(object_file, '/');
                    filename = filename ? filename + 1 : object_file;
                    
                    char addr2line_cmd[512];
                    snprintf(addr2line_cmd, sizeof(addr2line_cmd), 
                            "addr2line -e %s -f -p -C %p", 
                            object_file, relative_addr);
                    
                    FILE* addr2line = popen(addr2line_cmd, "r");
                    if (addr2line) {
                        char line[1024];
                        if (fgets(line, sizeof(line), addr2line)) {
                            size_t line_len = strlen(line);
                            if (line_len > 0 && line[line_len-1] == '\n') {
                                line[line_len-1] = '\0';
                            }
                            
                            if (strstr(line, "??:0") || strstr(line, "??:?")) {
                                fprintf(output, "  #%-2d %s in %s\n", i, filename, symbols ? symbols[i] : "??");
                            } else {
                                fprintf(output, "  #%-2d %s in %s\n", i, filename, line);
                            }
                        } else {
                            fprintf(output, "  #%-2d %s at %p\n", i, filename, callstack[i]);
                        }
                        pclose(addr2line);
                    } else {
                        fprintf(output, "  #%-2d %s at %p\n", i, filename, callstack[i]);
                    }
                } else {
                    fprintf(output, "  #%-2d at %p\n", i, callstack[i]);
                }
            }
        } else {
            fprintf(output, "  Unable to determine executable path\n");
        }
        
        fprintf(output, "=== END BACKTRACE ===\n\n");
        fflush(output);
    }

private:
    static void signal_handler(int sig) {
        fprintf(stderr, "Received signal %d (%s)\n", sig, strsignal(sig));
        
        fprintf(stderr, "\n=== MEMORY ANALYSIS ===\n");
        
        {
            std::lock_guard<std::mutex> lock(watch_mutex);
            for (const auto& watch : watch_addresses) {
                fprintf(stderr, "Checking watched memory region: %s at %p (size: %zu)\n", 
                        watch.second.name.c_str(), watch.first, watch.second.size);
                
                if (!is_memory_readable(reinterpret_cast<const char*>(watch.first), watch.second.size)) {
                    fprintf(stderr, "  WARNING: Memory region is not readable!\n");
                }
            }
        }
        
        fprintf(stderr, "\n=== RTTR ANALYSIS ===\n");
        fprintf(stderr, "This crash appears to be related to RTTR type handling.\n");
        fprintf(stderr, "Common causes:\n");
        fprintf(stderr, "- Accessing a type that has been destroyed (use-after-free)\n");
        fprintf(stderr, "- Memory corruption in the type system\n");
        fprintf(stderr, "- Invalid casting or type operations\n");
        fprintf(stderr, "- Registering or using types after RTTR shutdown\n\n");
        
        print_backtrace();
        
        fprintf(stderr, "\n=== RECOMMENDATIONS ===\n");
        fprintf(stderr, "1. Check your RTTR registration code for memory management issues\n");
        fprintf(stderr, "2. Ensure all type operations occur during the proper lifecycle\n");
        fprintf(stderr, "3. Validate types before using them\n");
        fprintf(stderr, "4. Look at the value of m_type_data (0x6d6f6f7a5f78616d) - this appears corrupt\n");
        fprintf(stderr, "5. Add safety checks in Zeytin::generate_variants()\n\n");
        
        signal(sig, SIG_DFL);
        raise(sig);
    }
    
    struct WatchInfo {
        size_t size;
        std::string name;
    };
    
    static std::mutex watch_mutex;
    static std::map<const void*, WatchInfo> watch_addresses;
};

std::mutex Backtrace::watch_mutex;
std::map<const void*, Backtrace::WatchInfo> Backtrace::watch_addresses;

} 
