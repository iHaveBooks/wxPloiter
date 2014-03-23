// logging.cpp
// author: Franc[e]sco
// contributors: <add your name here if you edit this file>

#include "logging.hpp"

#include "utils.hpp"
#include <typeinfo>
#include <iostream>

// a complete logging class I copy-pasted from one of my other projects
namespace utils
{
    // static members
    const char * const logging::tag = "utils::logging";
	const char * logging::filename = "lastsession.log";
	boost::shared_ptr<logging> logging::inst;

    logging::logging()
        : verb(info)
    {
        // initialize empty logging file
        std::ofstream f;

        if (openfile(f, std::fstream::out | std::fstream::trunc))
            f.close();
    }

    logging::~logging()
    {
        // empty
    }

	void logging::setfilename(const char * const filename)
	{
		logging::filename = filename;
	}

	boost::shared_ptr<logging> logging::get()
    {
		if (!inst.get())
			inst.reset(new logging);

        return inst;
    }

    void logging::setverbosity(const logging::verbosity v)
    {
        log(logging::info, tag, strfmt() << "setverbosity: setting log verbosity to " << static_cast<int>(v));
        this->verb = v;
    }

    logging::verbosity logging::getverbosity() const
    {
        return verb;
    }

    bool logging::wtf(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::assert))
            return log(logging::assert, tag, message);

        return true; // no errors, but it won't log anything because of verbosity
    }

    bool logging::e(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::error))
            return log(logging::error, tag, message);

        return true;
    }

    bool logging::w(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::warn))
            return log(logging::warn, tag, message);

        return true;
    }

    bool logging::i(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::info))
            return log(logging::info, tag, message);

        return true;
    }

    bool logging::d(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::debug))
            return log(logging::debug, tag, message);

        return true;
    }

    bool logging::v(const std::string tag, const std::string message) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::verbose))
            return log(logging::verbose, tag, message);

        return true;
    }

    bool logging::wtf(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::assert))
            return log(logging::assert, tag, format);

        return true;
    }

    bool logging::e(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::error))
            return log(logging::error, tag, format);

        return true;
    }

    bool logging::w(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::warn))
            return log(logging::warn, tag, format);

        return true;
    }

    bool logging::i(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::info))
            return log(logging::info, tag, format);

        return true;
    }

    bool logging::d(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::debug))
            return log(logging::debug, tag, format);

        return true;
    }

    bool logging::v(const std::string tag, const std::basic_ostream<char> &format) const
    {
        if (static_cast<int>(verb) >= static_cast<int>(logging::verbose))
            return log(logging::verbose, tag, format);

        return true;
    }

    bool logging::openfile(std::ofstream &f, const std::fstream::openmode mode) const
    {
        f.open(filename, mode);

        if (!f.is_open())
        {
            std::cout << "logging.openfile: failed to open log file." << std::endl;
            return false;
        }

        return true;
    }

    // appends text to the log file
    bool logging::puts(const char * const text) const
    {
        std::ofstream f;

        if (!openfile(f, std::fstream::out | std::fstream::app))
            return false;

        f << text;

        // define LOGCONSOLE to also send log messages to stdout
        #ifdef LOGCONSOLE
        std::cout << text;
        #endif

        if (f.bad())
        {
            std::cout << "logging.openfile: failed to write to log file." << std::endl;
            return false;
        }

        f.close();

        return true;
    }

    // logs something in the format <verbosity> [tag] message
    bool logging::log(const logging::verbosity v, const std::string tag, const std::string message) const
    {
        const char * verbositytag;
        std::ostringstream oss;

        // converts verbosity to text
        switch (v)
        {
        case assert:
            verbositytag = "assert";
            break;

        case error:
            verbositytag = "error";
            break;

        case warn:
            verbositytag = "warn";
            break;

        case info:
            verbositytag = "info";
            break;

        case debug:
            verbositytag = "debug";
            break;

        case verbose:
            verbositytag = "verbose";
            break;

        default:
            verbositytag = "invalid";
            wtf(this->tag, strfmt() << "log: invalid verbosity of " << static_cast<int>(v) << " provided");
            break;
        }

        oss << "<" << verbositytag << "> [" << tag << "] " << message << "\r\n";
        return puts(oss.str().c_str());
    }

    // this overload allows me to format messages on-the-fly like this: log(v, tag, strfmt() << "foo" << bar)
    bool logging::log(const verbosity v, const std::string tag, const std::basic_ostream<char> &format) const
    {
        // obtain the stream's streambuf and cast it back to stringbuf
        std::basic_streambuf<char> * const strbuf = format.rdbuf();

        if (strbuf && typeid(*strbuf) == typeid(std::stringbuf))
        {
            const std::string &str = dynamic_cast<std::stringbuf &>(*strbuf).str();
            return log(v, tag, str);
        }

        wtf(this->tag, "log: invalid stringstream provided");

        return false;
    }
}
