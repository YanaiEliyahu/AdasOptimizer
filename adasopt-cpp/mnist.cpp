#include "mnist.hpp"
#include <fstream>
#include <assert.h>

mnist_loader::mnist_loader(string image_file, string label_file, size_t num) {
  load_images(image_file, num);
  load_labels(label_file, num);
}

mnist_loader::mnist_loader(string image_file, string label_file) : mnist_loader(image_file, label_file, 0) {}

int mnist_loader::to_int(char* p) {
  return ((p[0] & 0xff) << 24) | ((p[1] & 0xff) << 16) |
    ((p[2] & 0xff) <<  8) | ((p[3] & 0xff) <<  0);
}

void mnist_loader::load_images(string image_file, size_t num) {
  ifstream ifs(image_file.c_str(), ios::in | ios::binary);
  char p[4];

  ifs.read(p, 4);
  int magic_number = to_int(p);
  assert(magic_number == 0x803);

  ifs.read(p, 4);
  m_size = to_int(p);
  // limit
  if (num != 0 && num < m_size) m_size = num;

  ifs.read(p, 4);
  m_rows = to_int(p);

  ifs.read(p, 4);
  m_cols = to_int(p);

  for (size_t i=0; i<m_size; ++i) {
    v<byte> image(m_rows * m_cols);
    ifs.read((char*)image.data(), m_rows * m_cols);
    m_images.push_back(move(image));
  }

  ifs.close();
}

void mnist_loader::load_labels(string label_file, size_t num) {
  ifstream ifs(label_file.c_str(), ios::in | ios::binary);
  char p[4];

  ifs.read(p, 4);
  int magic_number = to_int(p);
  assert(magic_number == 0x801);

  ifs.read(p, 4);
  int size = to_int(p);
  // limit
  if (num != 0 && num < m_size) size = num;

  for (int i=0; i<size; ++i) {
    ifs.read(p, 1);
    int label = p[0];
    m_labels.push_back(label);
  }

  ifs.close();
}
