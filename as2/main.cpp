int main() {

  std::ifstream input;
  input.open("merchant.txt");

  if (input.is_open()) {
    preprocess_input(input);
    input.close();
  }
  input.open("preprocessed");

  std::cout << "Welcome to Anish's bootleg RNN!" << std::endl;
  std::cout << "Please enter a window size: " << std::endl;

  int window_size;
  std::cin >> window_size;

  std::cout << "Great! Now enter an output size for your novel: " << std::endl;

  int output_size;
  std::cin >> output_size;

  // this returns an AVLMap
  const auto ret = read_input(input, window_size);
  const std::string out = generate_output(input, ret, window_size, output_size);

  std::cout << out << std::endl;

  input.close();
  return 0;
}
