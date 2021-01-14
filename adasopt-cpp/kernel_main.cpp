int Main(const v<string>&);

int main(int args_count, char **args) {
  println("program started");
  srand(nanoseconds());
  int return_code = 0;
  set_terminate(do_nothing<>);
  setup_signals();
  if (!_debug)
    set_process_memory_limit((size_t)10 * 1024 * 1024 * 1024);
  scope_stacktrace _;
  try {
    exception_handler(
      [&] {
        scope_stacktrace _;
        return_code = Main(vrange(args_count)->*expr(string(args[arg]))->*to_vector);
      },
      true
    );
  }
  catch (unwind ex) {return_code = ex.return_code;}
  catch (...) {return_code = 128;}
  return return_code;
}
