#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

thread_local bool is_creating_exception = false;
string println_prefix;
map<pair<const char*,size_t>,bool> _fixme_prints;
__thread bool stdout_silence = false;
string object_base::to_code() const {
  throw exception("`to_code` is not implemented");
}
string object_base::to_string() const {
  return stringify(*this);
}
ostream &object_base::to_string_on_stream(ostream &x) const {
  return x << ::to_string(typeid(*this));
}
ostream &operator<<(ostream &stream, const object_base &x) {
  return x.to_string_on_stream(stream);
}
exception::exception() {
  stdout_silence = false;
}
exception::exception(const string &message) : message(message) {
  if (is_creating_exception || uncaught_exceptions())
    return;
  stdout_silence = false;
  is_creating_exception = true;
//#stacktrace-block -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
  stacktrace = get_stacktrace();
//#stacktrace-unblock -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
  is_creating_exception = false;
}

__thread size_t scoped_stacktrace = 0;
scope_stacktrace::scope_stacktrace() {
  void *trace[1024];
  previous_scope = backtrace(trace,1024);
  swap(scoped_stacktrace,previous_scope);
}
scope_stacktrace::~scope_stacktrace() {
  swap(scoped_stacktrace,previous_scope);
}

//#stacktrace-block -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
string stackframe_to_string(string stackframe) {
  static vector<string> blacklist_suffixes = {"functional","std_function.h","sanitizer_common_interceptors.inc"};
  if (stackframe.size()) {
    if (stackframe.back() == '\n')
      stackframe = stackframe.substr(0,stackframe.size() - 1);
    if (stackframe == "??:?" || stackframe == "??:0" || stackframe == "??:")
      return "";
    if (stackframe.back() == ')') {
      size_t index = stackframe.size();
      while (index-- && stackframe[index] != '(');
      if (index != undefined_v)
        stackframe = stackframe.substr(0,index);
    }
    while (stackframe.back() == ' ')
      stackframe = stackframe.substr(0,stackframe.size() - 1);
    if (stackframe.find(':') != undefined_v) {
      size_t index = stackframe.find(':');
      stackframe = normalize_path(stackframe.substr(0,index)) + stackframe.substr(index);
    }
  }
  try {
    size_t colon = stackframe.find(':');
    string filename = stackframe.substr(0,colon);
    if ([&] {
      for (auto &suffix : blacklist_suffixes)
        if (ends_with(filename,"/" + suffix))
          return true;
      return false;
    }())
      return "";
    size_t linenumber = stoi(stackframe.substr(colon + 1,stackframe.size() - colon - 1)) - 1;
    auto file_content = read_file(filename);
    bool blocked = false;
    size_t last_index = 0;
    while (linenumber--) {
      size_t index = file_content.find('\n',last_index);
      string line = file_content.substr(last_index,index - last_index);
      last_index = index + 1;
      if (starts_with(line,"//#stacktrace-block"))
        blocked = true;
      if (starts_with(line,"//#stacktrace-unblock"))
        blocked = false;
    }
    size_t index = file_content.find('\n',last_index);
    while (file_content.at(last_index) == ' ' || file_content.at(last_index) == '\t')
      last_index++;
    file_content = file_content.substr(last_index,index - last_index);
    if (blocked)
      return "";
    stackframe += ":" + file_content;
  } catch (...) {}
  return stackframe;
}
v<string> get_stacktrace() {
  if (_release)
    return v<string>();
  _fixme("check if files has newer date modification than the application, if then think of a way to warn in that single stack trace.");
  v<string> result;
  void *trace[1024];
  size_t trace_size = backtrace(trace,1024);
  if (trace_size == 0)
    return v<string>();
  trace_size -= scoped_stacktrace;
  string command = "addr2line ";
  command += vrange(trace_size)->*expr(stringify(trace[arg] - 5))->*vreduce(expr2(arg0 + " " + arg1));
  command += " -e \"" + get_application_path() + "\" 2> /dev/null";
  auto output = run_command(command);
  while (output.size()) {
    size_t index = output.find('\n');
    string line = output.substr(0,index);
    output = output.substr(index + 1);
    line = stackframe_to_string(line);
    if (line.empty() || (!result.empty() && result.back() == line))
      continue;
    result.push_back(line);
  }
  return result;
}
//#stacktrace-unblock -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES

nanoseconds::nanoseconds() {
  struct timespec ts;
  _assertr(!clock_gettime(CLOCK_MONOTONIC, &ts));
  value = ts.tv_sec * 1000LL * 1000LL * 1000LL + ts.tv_nsec;
}
nanoseconds::nanoseconds(uint64_t value_) : value(value_) {}
nanoseconds &nanoseconds::operator-=(const nanoseconds &other) {value -= other.value; return *this;}
nanoseconds nanoseconds::operator-(const nanoseconds &other) const {return nanoseconds(value - other.value);}
nanoseconds nanoseconds::operator-() const {return nanoseconds(-value);}
nanoseconds &nanoseconds::operator+=(const nanoseconds &other) {value += other.value; return *this;}
nanoseconds nanoseconds::operator+(const nanoseconds &other) const {return nanoseconds(value + other.value);}
nanoseconds nanoseconds::operator+() const {return *this;}
bool nanoseconds::operator<(const nanoseconds &other) const {return value < other.value;}
bool nanoseconds::operator>(const nanoseconds &other) const {return value > other.value;}
bool nanoseconds::operator<=(const nanoseconds &other) const {return value <= other.value;}
bool nanoseconds::operator>=(const nanoseconds &other) const {return value >= other.value;}
nanoseconds::operator bool() const {
    return nanoseconds() < *this;
}
uint64_t nanoseconds::get() const {return value;}

ostream& operator<<(ostream &stream, const exception &exception) {
  if (!exception.message.empty())
    stream << "message: " << terminal::red() << terminal::bold() << exception.message << terminal::reset() << endl;
  if (!exception.stacktrace.empty()) {
    stream << "stacktrace:" << endl;
    for (auto &trace : exception.stacktrace) {
      bool normal_print = true;
      do {
        size_t index = trace.find(':');
        if (index == undefined_v)
          break;
        index = trace.find(':',index + 1);
        if (index == undefined_v)
          break;
        index++;
        stream << trace.substr(0,index) << terminal::yellow() << terminal::bold() << trace.substr(index,trace.size() - index) << terminal::reset() << endl;
        normal_print = false;
      } while(false);
      if (normal_print)
        stream << trace << endl;
    }
  }
  return stream;
}
ostream& operator<<(ostream &stream, const nanoseconds &nanoseconds) {
  auto before_precision = stream.precision();
  if (nanoseconds.get() < 1000)
    stream << nanoseconds.get() << "ns";
  else if (nanoseconds.get() < 1000 * 1000 / 10)
    stream << setprecision(2) << fixed << nanoseconds.get() / 1000.f << defaultfloat << "us";
  else if (nanoseconds.get() < 1000 * 1000)
    stream << setprecision(1) << fixed << nanoseconds.get() / 1000.f << defaultfloat << "us";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000 / 10)
    stream << setprecision(2) << fixed << nanoseconds.get() / 1000.f / 1000.f << defaultfloat << "ms";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000)
    stream << setprecision(1) << fixed << nanoseconds.get() / 1000.f / 1000.f << defaultfloat << "ms";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000 * 60 / 6)
    stream << setprecision(2) << fixed << nanoseconds.get() / 1000.f / 1000.f / 1000.f << defaultfloat << "s";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000 * 60)
    stream << setprecision(1) << fixed << nanoseconds.get() / 1000.f / 1000.f / 1000.f << defaultfloat << "s";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000 * 60 * 100 / 10)
    stream << setprecision(2) << fixed << nanoseconds.get() / 1000.f / 1000.f / 1000.f / 60.f << defaultfloat << "m";
  else if (nanoseconds.get() < 1000LL * 1000 * 1000 * 60 * 100)
    stream << setprecision(1) << fixed << nanoseconds.get() / 1000.f / 1000.f / 1000.f / 60.f << defaultfloat << "m";
  else
    stream << setprecision(2) << fixed << nanoseconds.get() / 1000.f / 1000.f / 1000.f / 60.f / 60.f << defaultfloat << "h";
  stream << setprecision(before_precision);
  return stream;
}
#if _arch64
ostream& operator<<(ostream &stream, uint128_t x) {
  do {
    stream << x % 10;
    x /= 10;
  } while (x != 0);
  return stream;
}
#endif

void fixme(const string &message) {
  println(terminal::yellow(),"fixme: ",terminal::reset(),message);
}

struct test_cases {
  map<string,v<function<void()>>> content;
  bool passed = true;

  bool run(const string &test_case_name_filter) {
    nanoseconds timer = -nanoseconds();
    println("running ",content.size()," test suites, with a total of ",content->*expr(arg.second.size())->*vsum," test cases.");
    for (auto &p : content) {
      size_t index = 0;
      for (auto &test_case : p.second) {
        string test_case_name = stringify(p.first,"-",++index);
        if (test_case_name.find(test_case_name_filter) == string::npos)
          continue;
        println(test_case_name);
        auto local_timer = -nanoseconds();
        auto test_case_results = exception();
        try {
          scope_stacktrace _;
          test_case();
        }
        catch (exception &ex) {
          test_case_results = ex;
        }
        catch (unwind ex) {throw;}
        catch (...) {
          test_case_results = exception("an exception of unknown type has been thrown");
          test_case_results.stacktrace.clear();
        }
        local_timer += nanoseconds();
        if (test_case_results.message.empty())
          print("PASSED");
        else if (test_case_results.message == "expected fail")
          print(terminal::bold(),terminal::yellow(),"EXPECTED FAIL",terminal::reset());
        else if (test_case_results.message == "unexpected pass") {
          passed = false;
          print(terminal::bold(),terminal::green(),"UNEXPECTED PASS",terminal::reset());
        }
        else {
          passed = false;
          println("an exception has been thrown:");
          print(test_case_results);
          print(terminal::bold(),terminal::red(),"FAILED",terminal::reset());
        }
        println(", ",local_timer);
      }
    }
    println(passed ? terminal::green() + "TESTS PASSED" : terminal::red() + "TESTS FAILED",terminal::reset(),", ",timer + nanoseconds());
    return passed;
  }
  bool run() {
    return run("");
  }
};

bool starts_with(const string &source, const string &prefix) {
  return source.size() >= prefix.size() && memcmp(source.c_str(),prefix.c_str(),prefix.size()) == 0;
}
bool ends_with(const string &source, const string &suffix) {
  return source.size() >= suffix.size() && memcmp(source.c_str() + source.size() - suffix.size(),suffix.c_str(),suffix.size()) == 0;
}
prefix_cut cut_prefix(const string &source, const string &prefix) {
  if (!starts_with(source,prefix))
    return {false,string()};
  return {true,source.substr(prefix.size())};
}

string get_application_path() {
  static string local_cache;
  static bool ran = false;
  if (!ran) {
    ran = true;
    const size_t tmp_size = 2048;
    char tmp[tmp_size];
    sprintf(tmp, "/proc/%d/exe", getpid());
    char tmp2[tmp_size];
    int bytes = readlink(tmp, tmp2, tmp_size);
    _check(bytes >= 0,"'readlink' failed");
    local_cache = string(tmp2,tmp2 + bytes);
  }
  return local_cache;
}
string get_application_directory() {
  return path_parent(get_application_path());
}
size_t get_modifications_time(const string &path) {
  struct stat attr;
  if (stat(path.c_str(), &attr))
    throw exception("stat failed");
  return ((size_t)attr.st_mtim.tv_sec) * 1000 * 1000 * 1000 + attr.st_mtim.tv_nsec;
}
string get_working_directory() {
  static string local_cache;
  static bool ran = false;
  if (!ran) {
    ran = true;
    const size_t tmp_size = 2048;
    char tmp[tmp_size];
    _check(getcwd(tmp,tmp_size),"'getcmd' failed");
    local_cache = tmp;
  }
  return local_cache;
}
string normalize_path(const string &path) {
  try {
    string working_directory = get_working_directory();
    prefix_cut cut;
    if (cut = cut_prefix(resolve_path(path),path_concat(working_directory,"")))
      return path_concat(".",cut.postfix);
    return resolve_path(path);
  } catch (...) {
    return path;
  }
}
string path_concat(const string &left, const string &right) {
  if (left.back() == '/')
    return left + right;
  return left + "/" + right;
}
string path_parent(const string &path) {
  if (path.empty())
    return "..";
  if (path.size() == 1)
    return path_concat(path,"..");
  for (size_t i = path.size() - 1; i--; )
    if (path[i] == '/')
      return path.substr(0,i + 1);
  return path_concat(path,"..");
}
string read_file(const string &filename) {
  int fd = open(filename.c_str(),O_RDONLY);
  _checkr(fd >= 0,"couldn't open '" + filename + "' for read");
  destructor ds([&](){close(fd);});
  off_t file_length = lseek(fd,0,SEEK_END);
  _checkr(file_length >= 0,"'lseek' failed");
  _checkr(lseek(fd,0,SEEK_SET) >= 0,"'lseek' failed");
  u<char[]> buf(file_length);
  for (ssize_t bytes_read = 0; bytes_read < file_length; ) {
    ssize_t read_result = read(fd,buf.get() + bytes_read,file_length - bytes_read);
    _checkr(read_result >= 0,"'read' failed");
    bytes_read += read_result;
  }
  return string(buf.get(), buf.get() + file_length);
}
void write_file(const string &filename, const string &content) {
  remove_file(filename);
  int fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC,S_IRWXU | S_IRWXG | S_IRWXO);
  _checkr(fd >= 0,"couldn't open '" + filename + "' for write");
  destructor ds([&](){close(fd);});
  _assertr(write(fd,content.c_str(),content.size()) == ssize_t(content.size()));
}
void remove_file(const string &filename) {
  remove(filename.c_str());
}
bool file_exists(const string &filename) {
  return access(filename.c_str(),F_OK) != -1;
}
string resolve_path(const string &path) {
  char *primitive_char = realpath(path.c_str(),nullptr);
  if (!primitive_char)
    return path;
  destructor ds([&](){free(primitive_char);});
  return primitive_char;
}
static vector<string> directory_list(const string &directory_path, size_t d_type, bool hidden) {
  vector<string> result;
  DIR *dir;
  class dirent *ent;
  dir = opendir(directory_path.c_str());
  destructor ds([&](){closedir(dir);});
  _check(dir,"couldn't open '" + directory_path + "' for iteration");
  while ((errno = 0, ent = readdir(dir)) != NULL) {
    const string file_name = ent->d_name;
    const string full_file_name = path_concat(directory_path,file_name);
    if (file_name == "." || file_name == "..")
      continue;
    if (hidden && file_name[0] == '.')
      continue;
    if (!(ent->d_type | d_type))
      continue;
    result.push_back(full_file_name);
  }
  _check(errno == 0,"'readdir' failed");
  return result;
}
vector<string> list_files(const string &directory_path) {
  return directory_list(directory_path,DT_REG | DT_LNK,false);
}
vector<string> list_directories(const string &directory_path) {
  return directory_list(directory_path,DT_DIR,false);
}

string run_command(const string &command) {
  char buffer[128];
  string result = "";
  shared_ptr<FILE> pipe(popen(command.c_str(), "r"), [](auto p) {if (p) pclose(p);});
  _checkr(pipe,"'popen' failed");
  if (!pipe)
    return "";
  while (!feof(pipe.get()))
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
  return result;
}
string tput(const string &request) {
  if (!is_tty())
    return "";
  static map<string,string> cache;
  static v<string> cache_blacklist {"cols","lines"};
  if (cache_blacklist->*vcontains(request) || cache.find(request) == cache.end() || cache[request] == "")
    cache[request] = run_command("tput " + request);
  return cache[request];
}
bool is_tty() {
  static bool local_cache;
  static bool cached = false;
  if (!cached) {
    cached = true;
    char c[1024];
    local_cache = ttyname_r(1,c,1024) == 0;
  }
  return local_cache;
}
string term_padding(const string &text, size_t pad_size) {
  _fixme("isn't really filtering terminal escape sequence");
  size_t length = 0;
  for (auto &c : text)
    length += c >= 32;
  return text + string(pad_size <= length ? 0 : pad_size - length,' ');
}
string parse_precision(double d, size_t precision) {
  stringstream ss;
  ss << setprecision(precision) << fixed << d << scientific;
  return ss.str();
}
string parse_precision_scientific(double d, size_t precision) {
  stringstream ss;
  ss << setprecision(precision) << scientific << d;
  return ss.str();
}
__thread bool in_async = false;
void async(function<void()> &&task) {
  return async_bulk({task});
}
void async_bulk(const v<function<void()>> &bulk, bool recursive) {
  if (in_async && !recursive) {
    for (auto &p : bulk)
      p();
    return;
  }
  static bool threads_initialized = false;
  static shared_queue<function<void()>> tasks;
  if (!threads_initialized) {
    threads_initialized = true;
    for (size_t i = thread::hardware_concurrency() + 2; i--; )
      thread([&] {
        exception_handler(
          [&] {
            scope_stacktrace _;
            in_async = true;
            while (true) tasks.pop()();
          },
          true
        );
      }).detach();
  }
  tasks.push(bulk);
}

#if _release
void explain_error(const string&, const exception&, bool is_critical) {
  if (!is_critical)
    return;
  println("program has been interrupted unexpectedly, please make a report of this.");
}
#else
void explain_error(const string &what_happened, const exception &exception, bool) {
  println();
  println(what_happened,":");
  print(exception);
}
#endif

static string signal_to_string(int sig) {
  switch (sig) {
    case SIGSEGV:
      return "segmentation fault";
    case SIGILL:
      return "illegal instruction";
    case SIGFPE:
      return "floating-point exception";
    case SIGINT:
      return "keyboard interrupt";
    case SIGABRT:
      return "abort";
    default:
      return "unknown signal";
  }
}
static bool is_critical_signal(int sig) {
  switch (sig) {
    //case SIGABRT: // doesn't work
    case SIGINT:
      return false;
    default:
      return true;
  }
}
static void signals_handler(int sig);
static void signal_handled(int sig);
void setup_signals() {
  signal(SIGSEGV,signals_handler);
  signal(SIGILL,signals_handler);
  signal(SIGFPE,signals_handler);
  signal(SIGABRT,signals_handler);
  if (false && _release) signal(SIGINT,signals_handler);
}
static void signal_handled(int sig) {
  setup_signals();
  if (is_critical_signal(sig)) {
    signal(sig,SIG_DFL);
    raise(sig);
  }
  else {
    _debug_println("unwinding...");
    _exit(0);
  }
}
static void signals_handler(int sig) {
//#stacktrace-block -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
    auto ex = exception(signal_to_string(sig));
//#stacktrace-unblock -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
    explain_error("OS interruption",ex,is_critical_signal(sig));
    println();
    signal_handled(sig);
}
void read_urandom(void *out, size_t byte_count) {
  int fd = open("/dev/urandom",O_RDONLY);
  _checkr(fd >= 0,"could not find /dev/urandom");
  destructor ds([&](){close(fd);});
  ssize_t read_result = read(fd,out,byte_count);
  _checkr(read_result >= 0,"'read' failed");
  //while (byte_count) {
  //  *(unsigned char*)out = rand();
  //  out++;
  //  byte_count--;
  //}
}
size_t random_number() {
  constexpr size_t internal_cache_size = 1024 * 1024 * 2;
  static u<char[]> cache(internal_cache_size);
  static size_t cache_left = 0;
  static_assert(internal_cache_size % sizeof(size_t) == 0,"`internal_cache_size` is not aligned with `size_t`");
  static mutex sync;
  anon_declare(unique_lock<mutex>(sync));
  if (cache_left == 0) {
    read_urandom(cache.get(),internal_cache_size);
    cache_left = internal_cache_size;
  }
  size_t result = *(size_t*)(cache.get() + internal_cache_size - cache_left);
  cache_left -= sizeof(result);
  //_fixme("DONT DO THAT");
  //if (cache_left == 0)
  //  cache_left = internal_cache_size;
  return result;
}
size_t random_number_range(size_t min, size_t max) {
  if (min == max)
    return min;
  return min + random_number() % (max - min + 1);
}
double random_real_range(double min, double max) {
  if (min == max)
    return min;
  return min + (random_number() / ((double)(size_t)-1)) * (max - min);
}

void set_process_memory_limit(size_t byte_count) {
  rlimit x;
  x.rlim_cur = byte_count;
  x.rlim_max = byte_count;
  setrlimit(RLIMIT_DATA,&x);
}

// this is to force order of creation of the global test cases
static test_cases *global_test_cases;
test_suite_::test_suite_(const string &name, const v<function<void()>> &x) {
  static test_cases global_test_cases_f2;
  global_test_cases_f2.content.insert({name,x});
  global_test_cases = &global_test_cases_f2;
}
static string verification_program_correctness_file_path() {
  return path_concat(get_application_directory(),"/program_correctness");
}
void verify_program_correctness(const string &test_case_name_filter) {
  try {
    if (file_exists(verification_program_correctness_file_path()) && get_modifications_time(get_application_path()) < get_modifications_time(verification_program_correctness_file_path()))
      return;
  }
  catch (...) { }
  _checkr(!global_test_cases || global_test_cases->run(test_case_name_filter),"program correctness could not be verified or passing tests needs to be expected to pass");
  write_file(verification_program_correctness_file_path(),"");
}
void clear_verification_program_correctness() {
  remove_file(verification_program_correctness_file_path());
}
bool random_bit() {
  static size_t cache, cache_left = 0;
  if (cache_left == 0) {
    cache = random_number();
    cache_left = sizeof(cache) * CHAR_BIT;
  }
  return (cache >> (--cache_left)) & 1;
}
bool random_bit(double probability) {
  _fixme("this is a hack");
  probability = 1.0 - probability;
  while (true) {
    auto bit = random_bit();
    if (bit)
      if (probability >= 0.5)
        probability = probability * 2.0 - 1.0;
      else
        return true;
    else
      if (probability >= 0.5)
        return false;
      else
        probability = probability * 2.0;
  }
}
void expected_fail(const function<void()> &x) {
  try {
    x();
  }
  catch (...) {
    throw exception("expected fail");
  }
  throw exception("unexpected pass");
}
string to_string(const type_info &x) {
  int status;
  string type_name = x.name();
  char *demangled_name = abi::__cxa_demangle(type_name.c_str(),NULL,NULL,&status);
  if(status == 0) {
      type_name = demangled_name;
      free(demangled_name);
  }
  return type_name;
}
string vector_to_string_formatting(const vector<string> &v) {
  stringstream x;
  apply_to_string_formatting_2(typeid(void),nullptr,x,v->*expr2(pair(stringify("[",arg1,"]"),arg0)));
  return x.str();
}
string prefix_lines_with(const string &source, const string &prefix) {
  stringstream result;
  if (source.find('\n') == string::npos) {
    result << source;
    return result.str();
  }
  auto it = (size_t)0;
  auto end = source.size();
  bool first = true;
  while (it < end) {
    auto it2 = source.find('\n',it);
    if (it2 == string::npos) {
      result << endl << prefix << string(&source[it],&source[end]);
      break;
    }
    if (first)
      first = false;
    else
      result << endl << prefix;
    result << string(&source[it],&source[it2]);
    it = it2 + 1;
  }
  return result.str();
}
ostream &apply_to_string_formatting_2(const type_info &type, void *self, ostream &x, const v<pair<string,string>> &fields) {
  if (type != typeid(void))
    x << to_string(type) << " (" << self << ")";
  else
    x << "vector<?>";
  if (fields.size() > 0)
    {}
  else
    return x;
  for (auto &field : fields)
    x << endl << "  " << field.first << ": " << prefix_lines_with(field.second,"  ");
  return x;
}

namespace terminal {

string bold() {
  return tput("bold");
}
string red() {
  return tput("setaf 1");
}
string green() {
  return tput("setaf 2");
}
string yellow() {
  return tput("setaf 3");
}
string blue() {
  return tput("setaf 4");
}
string magenta() {
  return tput("setaf 5");
}
string cyan() {
  return tput("setaf 6");
}
string white() {
  return tput("setaf 7");
}
string reset() {
  return tput("sgr0");
}
string clear() {
  return tput("clear");
}

}

static string wrap_string(const string &source, size_t max_line_length) {
  string result;
  size_t current_line_length = 0;
  string current_word;
  auto it = source.begin();
  while (it != source.end()) {
    current_word += *it;
    it++;
    if (current_line_length == 0 && current_word.size() == max_line_length) {
      result += current_word + "\n";
      current_word = "";
    }
    else if (max_line_length <= current_line_length + current_word.size()) {
      result += "\n";
      current_line_length = 0;
    }
    if (current_word.size() && current_word.back() == ' ') {
      result += current_word;
      current_line_length += current_word.size();
      current_word = "";
    }
  }
  result += current_word;
  return result;
}

void parse_program_arguments(const v<string> &arguments_, v<program_option> &program_options) {
  _fixme("check parameters sanity");
  v<string> arguments = arguments_->*vdrop(1);
  if (arguments == v<string> {"--help"} || arguments == v<string> {"-h"}) {
    for (auto &p : program_options) {
      print("--",p.name ? *p.name : "");
      if (p.shortcut)
        print(" (-",*p.shortcut,")");
      if (p.arguments.size()) {
        if (p.arguments.size() == 1)
          print(" ",p.arguments.front());
        else
          print(" ",p.arguments);
      }
      print(":");
      println(prefix_lines_with(wrap_string("\n" + p.description,parse<size_t>(tput("cols")) - 2),"  "));
    }
    _exit(1);
  }
  set<optional<string>> took_argument;
  auto it = arguments.begin();
  func(parse_argument)(program_option &option) {
    if (took_argument.find(option.name) != took_argument.end() && option.takes_single_argument)
      throw exception(stringify(option.name ? *option.name : "--"," takes a single argument"));
    if (option.takes_argument) {
      if (it == arguments.end())
        throw exception("expected an argument for " + (option.name ? *option.name : "--"));
      option.arguments.push_back(*it);
      it++;
    }
    else
      option.arguments.push_back("");
    took_argument.insert(option.name);
  };
  while (it != arguments.end()) {
    if (*it == "--") {
      it++;
      break;
    }
    if (starts_with(*it,"--")) {
      auto parameter_name = it->substr(2);
      auto option_it = find_if(program_options.begin(),program_options.end(),[&](const auto &x) {return x.name && *x.name == parameter_name;});
      if (option_it == program_options.end())
        throw exception("parameter " + parameter_name + " does not exists");
      auto &option = *option_it;
      it++;
      parse_argument(option);
    }
    else if (starts_with(*it,"-")) {
      auto parameter_name = it->substr(1);
      if (parameter_name.size() != 1)
        throw exception("`-` parameters cannot have more than one character");
      auto option_it = find_if(program_options.begin(),program_options.end(),[&](const auto &x) {return x.shortcut && *x.shortcut == parameter_name.back();});
      if (option_it == program_options.end())
        throw exception("parameter " + parameter_name + " does not exists");
      auto &option = *option_it;
      it++;
      parse_argument(option);
    }
    else {
      auto option_it = find_if(program_options.begin(),program_options.end(),[](const auto &x) {return !x.name;});
      if (option_it == program_options.end())
        throw exception("can't take an argument to --");
      auto &option = *option_it;
      parse_argument(option);
    }
  }
  while (it != arguments.end()) {
    auto option_it = find_if(program_options.begin(),program_options.end(),[](const auto &x) {return !x.name;});
    if (option_it == program_options.end())
      throw exception("can't take an argument to --");
    auto &option = *option_it;
    parse_argument(option);
  }
  for (auto &option : program_options)
    if (option.required && took_argument.find(option.name) == took_argument.end())
      throw exception(stringify(option.name ? *option.name : "--"," requires argument"));
}
