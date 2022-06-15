#include <CppUTest/CommandLineTestRunner.h>

int main(int argc, char** argv) {
  const char *av_override[] = {"exe", "-v"};

  return CommandLineTestRunner::RunAllTests(2, av_override);
}
