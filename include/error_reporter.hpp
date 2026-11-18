#ifndef CITU_ERROR_REPORTER_HPP
#define CITU_ERROR_REPORTER_HPP

#include <string>
#include <vector>
#include "location.hpp"

enum class Severity {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

struct DiagnosticMessage {
    Severity severity;
    std::string message;
    SourceLocation location;
};

class ErrorReporter {
private:
    std::string current_file;
    std::vector<std::string> file_lines;
    std::vector<DiagnosticMessage> diagnostics;
    int error_count = 0;
    int warning_count = 0;

public:
    static ErrorReporter& instance();

    void set_file(const std::string& filename, const std::string& content = "");
    void report(Severity sev, const std::string& msg, const SourceLocation& loc);
    void error(const std::string& msg, const SourceLocation& loc);
    void warning(const std::string& msg, const SourceLocation& loc);

    int get_error_count() const { return error_count; }
    int get_warning_count() const { return warning_count; }
    bool has_errors() const { return error_count > 0; }

    void print_diagnostics() const;
    void clear();
};

#endif // CITU_ERROR_REPORTER_HPP
