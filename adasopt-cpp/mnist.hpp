#pragma once

// source https://github.com/arpaka/mnist-loader
struct mnist_loader {
  v<v<byte>> m_images;
  v<size_t> m_labels;
  size_t m_size = 0;
  size_t m_rows = 0;
  size_t m_cols = 0;

  void load_images(string file, size_t num=0);
  void load_labels(string file, size_t num=0);
  int to_int(char* p);

  mnist_loader(string image_file, string label_file, size_t num);
  mnist_loader(string image_file, string label_file);

  size_t size() const { return m_images.size(); }
  size_t rows() const { return m_rows; }
  size_t cols() const { return m_cols; }

  const v<byte> &images(int id) { return m_images.at(id); }
  size_t labels(int id) { return (size_t)m_labels.at(id); }
};
