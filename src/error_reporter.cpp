#include "error_reporter.hpp"
#include <iostream>
#include <sstream>

ErrorReporter& ErrorReporter::instance() {
    static ErrorReporter inst;
    return inst;
}

void ErrorReporter::set_file(const std::string& filename, const std::string& content) {
    current_file = filename;
    file_lines.clear();
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        file_lines.push_back(line);
    }
}

void ErrorReporter::report(Severity sev, const std::string& msg, const SourceLocation& loc) {
    diagnostics.push_back({sev, msg, loc});
    if (sev == Severity::ERROR || sev == Severity::FATAL) {
        error_count++;
    } else if (sev == Severity::WARNING) {
        warning_count++;
    }
}

void ErrorReporter::error(const std::string& msg, const SourceLocation& loc) {
    report(Severity::ERROR, msg, loc);
}

void ErrorReporter::warning(const std::string& msg, const SourceLocation& loc) {
    report(Severity::WARNING, msg, loc);
}

void ErrorReporter::print_diagnostics() const {
    for (const auto& diag : diagnostics) {
        std::string tag;
        std::string color;
        std::string reset = "\033[0m";

        switch (diag.severity) {
            case Severity::ERROR:
            case Severity::FATAL:
                color = "\033[1;31m";
                tag = "error";
                break;
            case Severity::WARNING:
                color = "\033[1;33m";
                tag = "warning";
                break;
            case Severity::INFO:
                color = "\033[1;34m";
                tag = "info";
                break;
        }

        std::cerr << color << diag.location.to_string() << ": " << tag << ": " << reset << diag.message << "\n";

        int line_idx = diag.location.first_line - 1;
        if (line_idx >= 0 && line_idx < static_cast<int>(file_lines.size())) {
            std::cerr << "    " << file_lines[line_idx] << "\n";
            int col = diag.location.first_column;
            if (col < 1) col = 1;
            std::string caret(col - 1, ' ');
            std::cerr << "    " << color << caret << "^" << reset << "\n";
        }
    }
}

void ErrorReporter::clear() {
    diagnostics.clear();
    file_lines.clear();
    current_file.clear();
    error_count = 0;
    warning_count = 0;
}
