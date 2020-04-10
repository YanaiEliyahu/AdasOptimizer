#include "mnist.hpp"

struct derivatives_normalizer {
  derivatives_normalizer(size_t derivative_count) {
    second_moments.resize(derivative_count);
  }

  v<float> normalize(const v<float> &derivatives) {
    _assertr(derivatives.size() == second_moments.size());
    v<float> result(derivatives.size());
    counter++;
    float scale = 1.f / (1.f - pow(.999f,counter));
    for (size_t i = 0; i < second_moments.size(); i++) {
      second_moments[i] = second_moments[i] * .999f + sqr(derivatives[i]) * .001f;
      result[i] = derivatives[i] / sqrtf(second_moments[i] * scale);
      if (!isfinite(result[i]))
        result[i] = 0.f;
    }
    return result;
  }
  float normalize(float derivative) {
    return normalize(v<float> {derivative})->*vone;
  }

  v<float> second_moments;
  size_t counter = 0;
};

struct layer {
  layer(bool relu_, size_t input_size_, size_t output_size_, size_t batch_size_ = 4) :
    relu(relu_),
    output_size(output_size_),
    input_size(input_size_ + 1), // bias
    batch_size(batch_size_),
    weights_derivatives_normalizer(input_size * output_size),
    input_step_sizes_derivatives_normalizer(input_size),
    step_size_step_size_derivative_normalizer(1)
  {
    for_each_weight([&](auto...) {
      weights.emplace_back(float(random_real_range(-1.,1.) * 4. / sqrt(input_size)));
      batch_derivatives.emplace_back(0.f);
      decayed_update.emplace_back(0.f);
    });
    input_step_sizes.resize(input_size,.1f / input_size * sqrtf(batch_size));
    input_offset.resize(input_size,0.f);
    decayed_input_step_sizes_update.resize(input_size);
  }
  void for_each_weight(const auto &f) {
    size_t weight_index = 0;
    for (size_t i = 0; i < output_size; i++)
      for (size_t j = 0; j < input_size; j++)
        f(i,j,weight_index++);
  }
  const v<float> &propagate(const v<float> &input) {
    _assertr(input->*expr(isfinite(arg))->*vall);
    last_input = input->*vval(1.f);
    _unwind(last_input.size());
    _unwind(input_size);
    _assertr(last_input.size() == input_size);
    last_output.clear();
    last_output.resize(output_size);
    for_each_weight([&](size_t out, size_t in, size_t i) {last_output[out] += last_input[in] * weights[i];});
    if (relu)
      last_output = last_output->*expr(max(0.f,arg));
    return last_output;
  }
  v<float> backpropagate(v<float> output_derivatives) {
    _assertr(output_derivatives->*expr(isfinite(arg))->*vall);
    _unwind(output_derivatives.size());
    _unwind(output_size);
    _assertr(output_derivatives.size() == output_size);
    counter++;
    if (relu)
      output_derivatives = output_derivatives->*expr2(last_output[arg1] == 0.f ? 0.f : arg0);
    input_offset = input_offset * .99999f + last_input * .00001f;
    input_offset.back() = 0.f; // bias
    auto offsetted_input = last_input - input_offset;
    v<float> input_derivatives(input_size);
    for_each_weight([&](size_t out, size_t in, size_t i) {
      batch_derivatives[i] += output_derivatives[out] * offsetted_input[in];
      input_derivatives[in] += output_derivatives[out] * weights[i];
    });
    input_derivatives.pop_back(); // bias
    if (counter % batch_size != 0)
      return input_derivatives;
    auto weights_update = weights_derivatives_normalizer.normalize(batch_derivatives);
    v<float> input_step_sizes_derivatives(input_size);
    for_each_weight([&](size_t, size_t in, size_t i) {
      weights[i] += weights_update[i] * input_step_sizes[in];
      input_step_sizes_derivatives[in] += decayed_update[i] * batch_derivatives[i];
      decayed_update[i] = decayed_update[i] * .99999f + weights_update[i] * .00001f;
    });
    auto input_step_sizes_update = input_step_sizes_derivatives_normalizer.normalize(input_step_sizes_derivatives);
    input_step_sizes = input_step_sizes + input_step_sizes * input_step_sizes_update * step_size_step_size;
    step_size_step_size = step_size_step_size + step_size_step_size *
      step_size_step_size_derivative_normalizer.normalize(
        (decayed_input_step_sizes_update * input_step_sizes_derivatives)->*vsum
      ) * .0005f;
    decayed_input_step_sizes_update = decayed_input_step_sizes_update * .9999f + input_step_sizes_update * .0001f;
    batch_derivatives->*void_expr(arg = 0.f);
    return input_derivatives;
  }

  bool relu;
  size_t output_size, input_size, batch_size;
  v<float> weights, batch_derivatives, decayed_update, input_step_sizes, input_offset, last_input, last_output, decayed_input_step_sizes_update;
  size_t counter = 0;
  derivatives_normalizer weights_derivatives_normalizer, input_step_sizes_derivatives_normalizer, step_size_step_size_derivative_normalizer;
  float step_size_step_size = .005;
};
int Main(const v<string>&) {
  mnist_loader training("mnist/train-images-idx3-ubyte","mnist/train-labels-idx1-ubyte",60000);

  auto training_input = training.m_images->*expr(arg->*expr(arg / 255.f)->*to_v)->*to_v;
  auto training_output = training.m_labels;

  size_t
    input_size = 28 * 28,
    hidden_size = 64,
    output_size = 10
  ;

  v<layer> layers;
  layers.emplace_back(true,input_size,hidden_size);
  layers.emplace_back(false,hidden_size,output_size);

  for (size_t epoch = 1; ; epoch++) {
    size_t accurates = 0;
    _expr(epoch);
    progress([&](size_t i) {
      auto curr_label = training_output[i];
      auto classifications_logs = layers[1].propagate(layers[0].propagate(training_input[i]));
      auto classifications = classifications_logs->*expr(exp(arg))->*to_v;
      auto classifications_softmax = (classifications / classifications->*vsum)->*expr(median(1e-10,arg,1. - 1e-10))->*to_v;
      accurates += classifications_softmax->*vpair->*vsort->*vreverse->*vtake(1)->*expr(arg.second)->*vone == curr_label;
      v<float> output_derivatives;
      for (size_t i = 0; i < classifications.size(); i++) {
        auto true_distribution = float(i == curr_label);
        output_derivatives.push_back(
          true_distribution / classifications->*vsum * (classifications->*vsum - classifications[i]) +
          (true_distribution - 1.) / classifications->*vsum * classifications[i]
          - classifications_logs[i] * .00001
        );
      }
      layers[0].backpropagate(layers[1].backpropagate(output_derivatives));
    },training.size());
    println("accuracy: ",accurates * 100.f / training.size(),"%");
    println();
  }
  return 0;
}
