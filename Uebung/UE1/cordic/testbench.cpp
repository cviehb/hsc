#include "testbench.h"

template <typename T> T to_rad(T const input) {
  const double PI = std::atan(1.0) * 4;
  return input / 180 * PI;
}

template <typename T> T to_grad(T const input) {
  const double PI = std::atan(1.0) * 4;
  return input * 180 / PI;
}

void testbench::check_logs(std::string const &str) {
  // check if error is present in logfile
  std::string word;
  int error = 0;
  std::ifstream logfile(str.c_str());

  if (logfile.good()) {
    while (logfile >> word) {
      if (word == "ERROR") {
        error++;
      }
    }
    cout << "End of test with " << error << " Error(s)!" << endl;
  } else {
    cout << "End of test couldn't check logfile for Error(s)" << endl;
  }
}

// #define DEBUG
void testbench::set_seed(int seed) { given_seed = seed; }

// stimuli function
void testbench::stimulus() {
  size_t const num_tests = 10000;
  double const delta = 0.5;
  const double PI = std::atan(1.0) * 4;
  int seed = time(0);

  // check for predefined seed
  if (given_seed)
    seed = given_seed;

  ofstream logfile;
  std::string logfile_name("cordic_" + std::to_string(seed) + ".log");
  logfile.open(logfile_name.c_str());
  logfile.clear();

  cout << "Test Cordic Algorithm with tests: " << num_tests << endl;
  cout << "Run with seed: " << seed << endl;

  if (!logfile.good())
    cout << "Error creating/opening logfile" << endl;

  logfile << "Test Cordic Algorithm with tests: " << num_tests << endl;
  logfile << "Run with seed: " << seed << endl;

  // initialize rand with seed
  std::srand(seed);

  // initialize test
  wait(SC_ZERO_TIME);

  // run tests in loop with rand vals
  for (size_t i = 0; i < num_tests; ++i) {
    // gen rand test
    double test_value = (double)rand() / (double)(RAND_MAX / (PI / 2));
    corner_o.write(test_value);
    start.write(true);
    wait(SC_ZERO_TIME);
    // log file
    logfile << sc_time_stamp() << "\t"
            << " phi: " << to_grad(test_value);

    // handshake
    wait(rdy.posedge_event());
    start.write(false);
    wait(SC_ZERO_TIME);

    // validate against math.h
    // cos
    double diff =
        (to_grad(test_value) - to_grad(std::acos(result_cos_i.read())));
    if (std::abs(diff) > delta)
      logfile << " ERROR cos: " << to_grad(std::acos(result_cos_i.read()))
              << endl;
    else
      logfile << " cos: " << to_grad(std::acos(result_cos_i.read())) << endl;

    diff = (to_grad(test_value) - to_grad(std::asin(result_sin_i.read())));
    if (std::abs(diff) > delta)
      logfile << " ERROR sin: " << to_grad(std::asin(result_sin_i.read()))
              << endl;
    else
      logfile << " sin: " << to_grad(std::asin(result_sin_i.read())) << endl;

#ifdef DEBUG
    // print debug result
    cout << sc_time_stamp() << "\t"
         << " Phi: " << corner_o.read()
         << "° Iterations = " << cordic::calc_count << " ==> "
         << "cos(" << corner_o.read() << "°) = " << result_cos_i.read() << "("
         << std::acos(result_cos_i.read()) * 180 / PI << "°)"
         << ", sin(" << corner_o.read() << "°) = " << result_sin_i.read() << "("
         << std::asin(result_sin_i.read()) * 180 / PI << "°)" << endl;
#endif
  }
  logfile.close();

  // check results
  check_logs(logfile_name);
}
