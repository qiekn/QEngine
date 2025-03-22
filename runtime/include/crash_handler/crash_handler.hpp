#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <cxxabi.h>
#include <csignal>
#include <dlfcn.h>


namespace zeytin {

class CrashHandler {
public:
    static void initialize() {
        install_signal_handlers();
        
        std::set_terminate([]() {
            try {
                std::exception_ptr ex = std::current_exception();
                if (ex) {
                    try {
                        std::rethrow_exception(ex);
                    } catch (const std::exception& e) {
                        std::cerr << "Uncaught exception: " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "Uncaught exception (unknown type)" << std::endl;
                    }
                } else {
                    std::cerr << "Terminate called without an active exception" << std::endl;
                }
            } catch (...) {
                std::cerr << "Error in terminate handler" << std::endl;
            }
            
            std::cerr << "Stack trace at terminate:" << std::endl;
            print_backtrace(std::cerr);
            
            std::abort();
        });
        
        std::cout << "Enhanced crash handler initialized" << std::endl;
    }
    
    static void log_current_backtrace(const std::string& message) {
        std::cerr << message << std::endl;
        print_backtrace(std::cerr);
    }

private:
    static void install_signal_handlers() {
        std::signal(SIGSEGV, signal_handler); 
        std::signal(SIGABRT, signal_handler);
        std::signal(SIGFPE, signal_handler);  
        std::signal(SIGILL, signal_handler); 
        std::signal(SIGTERM, signal_handler); 
        std::signal(SIGINT, signal_handler); 
    }

    static void signal_handler(int sig) {
        std::signal(sig, SIG_DFL);
        
        const char* sig_name = "Unknown signal";
        switch (sig) {
            case SIGSEGV: sig_name = "SIGSEGV (Segmentation fault)"; break;
            case SIGABRT: sig_name = "SIGABRT (Abort)"; break;
            case SIGFPE:  sig_name = "SIGFPE (Floating point exception)"; break;
            case SIGILL:  sig_name = "SIGILL (Illegal instruction)"; break;
            case SIGTERM: sig_name = "SIGTERM (Termination request)"; break;
            case SIGINT:  sig_name = "SIGINT (Interrupt)"; break;
        }

        std::cerr << "\n*** FATAL ERROR: Received signal " << sig << " (" << sig_name << ") ***\n";
        
        try {
            std::cerr << "FATAL ERROR: Received signal " << sig << " (" << sig_name << ")" << std::endl;
            print_backtrace(std::cerr);
        } catch (...) {
            std::cerr << "Error while logging crash info" << std::endl;
        }

        std::cerr << "\nDetailed backtrace:\n";
        print_detailed_backtrace();

        std::raise(sig);
    }

    static void print_backtrace(std::ostream& os) {
        const int max_frames = 128;
        void* callstack[max_frames];
        int frames = backtrace(callstack, max_frames);
        char** symbols = backtrace_symbols(callstack, frames);
        
        if (!symbols) {
            os << "Failed to obtain backtrace symbols" << std::endl;
            return;
        }
        
        os << "Backtrace (" << frames << " frames):" << std::endl;
        
        for (int i = 0; i < frames; ++i) {
            os << "  [" << i << "] " << demangle_symbol(symbols[i]) << std::endl;
        }
        
        free(symbols);
    }

    static void print_detailed_backtrace() {
        const int max_frames = 128;
        void* callstack[max_frames];
        int frames = backtrace(callstack, max_frames);
        char** symbols = backtrace_symbols(callstack, frames);
        
        if (!symbols) {
            std::cerr << "Failed to obtain backtrace symbols" << std::endl;
            return;
        }
        
        std::cerr << "Detailed backtrace (" << frames << " frames):" << std::endl;
        
        char exe_path[1024] = {0};
        if (readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1) == -1) {
            std::cerr << "Warning: Could not determine executable path" << std::endl;
        }
        
        for (int i = 0; i < frames; ++i) {
            std::cerr << "  [" << i << "] " << symbols[i] << std::endl;
            
            try {
                Dl_info info;
                if (dladdr(callstack[i], &info)) {
                    uintptr_t relative_addr = 
                        reinterpret_cast<uintptr_t>(callstack[i]) - 
                        reinterpret_cast<uintptr_t>(info.dli_fbase);
                    
                    if (info.dli_fname && 
                        std::string(info.dli_fname).find("Zeytin") != std::string::npos) {
                        
                        char cmd[512];
                        snprintf(cmd, sizeof(cmd), 
                                 "addr2line -e %s -f -p -C 0x%lx", 
                                 info.dli_fname, relative_addr);
                                 
                        FILE* pipe = popen(cmd, "r");
                        if (pipe) {
                            char buffer[512] = {0};
                            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                                size_t len = strlen(buffer);
                                if (len > 0 && buffer[len-1] == '\n') {
                                    buffer[len-1] = '\0';
                                }
                                
                                if (strstr(buffer, "??:0") == nullptr &&
                                    strstr(buffer, "??:?") == nullptr) {
                                    std::cerr << "      " << buffer << std::endl;
                                }
                            }
                            pclose(pipe);
                        }
                    }
                }
            } catch (...) {
            }
        }
        
        free(symbols);
    }

    static std::string demangle_symbol(const char* symbol) {
        if (!symbol) return "???";
        
        std::string result(symbol);
        
        size_t paren_start = result.find('(');
        size_t paren_end = result.find(')');
        
        if (paren_start != std::string::npos && paren_end != std::string::npos && paren_start < paren_end) {
            std::string func_with_offset = result.substr(paren_start + 1, paren_end - paren_start - 1);
            
            size_t plus_pos = func_with_offset.find('+');
            std::string mangled_name;
            
            if (plus_pos != std::string::npos) {
                mangled_name = func_with_offset.substr(0, plus_pos);
                std::string offset = func_with_offset.substr(plus_pos);
                
                int status;
                char* demangled = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
                
                if (status == 0 && demangled) {
                    result = result.substr(0, paren_start + 1) + demangled + offset + result.substr(paren_end);
                    free(demangled);
                }
            }
        }
        
        return result;
    }
};

} // namespace zeytin
