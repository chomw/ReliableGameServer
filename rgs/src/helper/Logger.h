// https://github.com/castisdev/logger
#pragma once

#include <boost/locale.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <cstddef>
#include <iostream>
#include <string>

#define RGS_LOG(severity)\
  BOOST_LOG_SEV(my_logger::get(), severity)\
  << boost::filesystem::path(__FILE__).filename().string()\
  << ":" << __LINE__ << ")] "

enum severity_level {
	core,
	debug,
	report,
	info,
	success,
	warning,
	error,
	fail,
	exception,
	critical
};

struct severity_tag;
// The operator is used when putting the severity level to log
inline boost::log::formatting_ostream& operator<< (
	boost::log::formatting_ostream& strm,
	boost::log::to_log_manip<severity_level, severity_tag> const& manip) {
	static const char* strings[] = {
		"Core",
		"Debug",
		"Report",
		"Information",
		"Success",
		"Warning",
		"Error",
		"Fail",
		"Exception",
		"Critical"
	};

  severity_level level = manip.get();
  if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
    strm << strings[level];
  else
    strm << static_cast< int >(level);

  return strm;
}

class LogBackend
: public boost::log::sinks::basic_formatted_sink_backend<
    char, boost::log::sinks::synchronized_feeding> {
 private:
  bool auto_flush_;
  boost::filesystem::ofstream file_;
  boost::filesystem::path target_path_;
  boost::filesystem::path file_path_;
  std::string file_name_suffix_;
  uintmax_t rotation_size_;
  uintmax_t characters_written_;
  boost::gregorian::date current_date_;

 public:
  explicit LogBackend(
      boost::filesystem::path const& target_path,
      std::string const& file_name_suffix,
      uintmax_t rotation_size,
      bool auto_flush)
      : auto_flush_(auto_flush),
      target_path_(target_path),
      file_name_suffix_(file_name_suffix),
      rotation_size_(rotation_size),
      characters_written_(0),
      current_date_(boost::gregorian::day_clock::local_day()){
  }

  void consume(
      boost::log::record_view const& /*rec*/,
      string_type const& formatted_message) {
    if (current_date_ != boost::gregorian::day_clock::local_day()) {
      rotate_file();
    }

    if (!file_.is_open()) {
      file_path_ = generate_filepath();
      boost::filesystem::create_directories(file_path_.parent_path());
      file_.open(file_path_, std::ofstream::out | std::ofstream::app);
      if (!file_.is_open()) {
        // failed to open file
        return;
      }
      characters_written_ = static_cast<std::streamoff>(file_.tellp());
    }
    file_.write(
        formatted_message.data(),
        static_cast<std::streamsize>(formatted_message.size()));
    file_.put('\n');
    characters_written_ += formatted_message.size() + 1;

    if (auto_flush_)
      file_.flush();

    if ((file_.is_open() && (characters_written_ >= rotation_size_)) ||
        (!file_.good())) {
      rotate_file();
    }
  }

 private:
  void rotate_file() {
    file_.close();
    file_.clear();
    characters_written_ = 0;
    current_date_ = boost::gregorian::day_clock::local_day();
  }

  boost::filesystem::path generate_filepath() {
    std::string filename_prefix = datetime_string_with_format("%Y-%m-%d");
    std::string monthly_path_name = datetime_string_with_format("%Y-%m");

    boost::filesystem::path monthly_path(target_path_ / monthly_path_name);
    // e.g. 2014-08-12[1]_example.log
    boost::regex pattern(
        filename_prefix + "[\\[\\]0-9]*" + "_" + file_name_suffix_ + ".log");
    uintmax_t next_index = scan_next_index(monthly_path, pattern);

    std::stringstream filename_ss;
    filename_ss << filename_prefix;
    if (next_index > 0) {
      filename_ss << "[" << next_index << "]";
    }
    filename_ss << "_" + file_name_suffix_;
    filename_ss << ".log";

    return boost::filesystem::path(monthly_path / filename_ss.str());
  }

  std::string datetime_string_with_format(std::string const& format) {
    std::locale loc(
        std::cout.getloc(),
        new boost::posix_time::time_facet(format.c_str()));
    std::stringstream ss;
    ss.imbue(loc);
    ss << boost::posix_time::second_clock::local_time();
    return ss.str();
  }

  uintmax_t scan_next_index(
      boost::filesystem::path const& path,
      boost::regex const& pattern) {
    uintmax_t current_index = 0;
    boost::filesystem::path current_fs;
    if (boost::filesystem::exists(path) &&
        boost::filesystem::is_directory(path)) {
      boost::filesystem::directory_iterator it(path), end;
      for (; it != end; ++it) {
        if (boost::regex_match(it->path().filename().string(), pattern)) {
          uintmax_t index = parse_index(it->path().filename().string());
          if (index >= current_index) {
            current_index = index;
            current_fs = it->path();
          }
        }
      }
    }
    boost::system::error_code ec;
    uintmax_t filesize = boost::filesystem::file_size(current_fs, ec);
    if (!ec) {
      if (filesize >= rotation_size_) {
        ++current_index;
      }
    }
    return current_index;
  }

  uintmax_t parse_index(std::string const& filename) {
    size_t pos_index_begin = filename.find('[');
    size_t pos_index_end = filename.find(']');
    unsigned int index = 0;
    if (pos_index_begin != std::string::npos &&
        pos_index_end != std::string::npos) {
      index = atoi(filename.substr(pos_index_begin + 1,
                                   pos_index_end - pos_index_begin).c_str());
    }
    return index;
  }
};

namespace rgs {
  namespace logger {
    typedef boost::log::sinks::asynchronous_sink<LogBackend> log_async_sink_t;

	inline boost::shared_ptr<log_async_sink_t> init_async_logger(
        const std::string& app_name,
        const std::string& app_version,
		bool console_log = false,
        const std::string& target = "./log",
        int64_t rotation_size = 100 * 1024 * 1024,
        bool auto_flush = true) {
      namespace expr = boost::log::expressions;
      boost::log::add_common_attributes();
      boost::shared_ptr<LogBackend> backend(
          new LogBackend(
              boost::filesystem::path(target),
              app_name,
              rotation_size,
              auto_flush));

	  //sync 생성 및 포맷 초기화
	  boost::shared_ptr<log_async_sink_t> sink(new log_async_sink_t(backend));
	  boost::log::formatter logFormatter = expr::stream
		  << "["
		  << expr::format_date_time<boost::posix_time::ptime>(
		  "TimeStamp",
		  "%Y-%m-%d-%H:%M:%S.%f") << " ("
		  << expr::smessage;

	  sink->set_formatter(logFormatter);

	  std::locale loc = boost::locale::generator()("ko-kr.UTF-8");
	  
	  sink->imbue(loc);
	  boost::log::core::get()->add_sink(sink);

	  if (console_log == true)
	  {
		  //console sync
		  auto consoleSink = boost::log::add_console_log(std::clog);
		  consoleSink->set_formatter(logFormatter);

		  boost::log::core::get()->add_sink(consoleSink);
	  }

      return sink;
    }

	inline void stop_logger(boost::shared_ptr<log_async_sink_t> sink) {
      boost::shared_ptr<boost::log::core> core = boost::log::core::get();
	  
	  sink->stop();
	  sink->flush();
	  core->remove_sink(sink);
      sink.reset();

    }

    inline boost::format formatter_r(boost::format& format) {
      return format;
    }

    template <typename T, typename... Params> inline boost::format formatter_r(
        boost::format& format, T arg, Params... parameters) {
          return formatter_r(format % arg, parameters...);
        }

    inline boost::format formatter(const char* const format) {
      return boost::format(format);
    }

    template <typename T, typename... Params> inline boost::format formatter(
        const char* const format, T arg, Params... parameters) {
          return formatter_r(boost::format(format) % arg, parameters...);
        }
  }
}
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(
    my_logger, boost::log::sources::severity_logger_mt<severity_level>)