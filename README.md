# AdasOptimizer
ADAS is short for Adaptive Step Size, it's an optimizer that unlike other optimizers that just normalize the derivatives, it fine-tunes the step size, truly making step size scheduling obsolete.

## How to use

```shell
git clone https://github.com/YanaiEliyahu/AdasOptimizer.git && cd AdasOptimizer
```

Tensorflow 1.x and 2.x:
1. For standalone optimizer file see [adasopt.py](adasopt.py).
2. For quick benchmark run [adas.py](adas.py) and [adam.py](adam.py).

For PyTorch's standalone optimizer file see [adasopt_pytorch.py](adasopt_pytorch.py).

There is also a C++ version of Adas in [adasopt-cpp](adasopt-cpp).

Guidelines on how to use this optimizer:
1. Use normalization; the purpose is to remove the bias/mean from the values.
2. It's known that the optimizer doesn't work well with 2 loss functions that minimizing one introduces error in the other, the optimizer would raise the step size and never converge.

## Training Performance

![ADAS vs ADAM](/adas_vs_adam.png)

This is a graph of ADAS (blue) and ADAM (orange)'s inaccuracy percentages in log scale (y-axis) over epochs (x-axis) on MNIST's training dataset using shallow network of 256 hidden nodes.
While ADAM slows down significantly overtime, ADAS converages to 0% inaccuracy (AKA 100% accuracy) in 11 iterations.

To see how ADAM was tested see/run the python script `./adam.py`, it uses tensorflow.
ADAS was compared against other optimizers too (AdaGrad, AdaDelta, RMSprop, Adamax, Nadam) in tensorflow, and none of them showed better results than ADAM, so their performance was left out of this graph.
Increasing ADAM's step size improved the performance in the short-term, but made it worse in the long-term, and vice versa for decreasing it's step size.

## Test/Generalization Performance

![ADAS vs ADAM](/adam_vs_adas_cifar100_mobilenetv2-3.png)

Same as training performance, but the graph means performance on the test set and cifar-100 with vanilla MobileNetV2 with dropout 0.15 on the top layer. The average accuracy of the last 50 epochs here is 26.4% and 37.4% for Adam and Adas respectively, and variances are 0.00082 and 8.88E-6. Conclusions:
1. In the epoch 10th, Adas was already over 27% so it can be said that Adas is ~9x times faster than Adam in this case.
2. generalized 15% better. `(37.4%-26.4%)/(100%-26.4%) = 14.9%`
3. 90x times more stable. (looking at the variances)

## Theory

This section explains how ADAS optimizes step size.

The problem of finding the optimal step size formulates itself into optimizing `f(x + f'(x) * step-size)` by `step-size`.
Which is translated into this formula that updates the `step-size` on the fly: `step-size(n+1) = step-size(n) + f'(x) * f'(x + f'(x) * step_size(n))`.
In english it means optimize step size so the loss decreases the most with each weights update.
The final formula makes sense, because whenever `x` is updated in the same direction, the `step-size` should increase because we didn't make a large enough step, and vice versa for opposite.

You may notice that there's a critical problem in computing the above formula, it requires evaluation of the gradient on the entire dataset twice for each update of `step-size`, which is quite expensive.
To overcome the above problem, compute a running average of `x`'s derivative in SGD-context, this represents the `f'(x)` in the formula, and for each SGD update to `x`,
its derivative represents the `f'(x + f'(x) * step_size(n))`, and then update the `step-size` according to the formula.

## How ADAS works

For each layer in the network:
1. Step size per input neuron (called input step sizes), default is `0.1 / input-nodes-count`.
2. Running averages (default decay value is `.99999`) of weights updates (of the layer).
3. Input offsets (default decay value is `.99999`), is a running averages of the input.
4. Running averages (default decay value is `.9999`) of input step sizes updates.
5. Step size that is used for updating the input step sizes, default is 0.005. (called step size step size, or in short SSSS, it's a step size that is updated too during training like input step sizes)

Layer's weights, input step sizes and SSSS are all updated, each taking the next item in the list as their step size, where SSSS's step size is a constant, default is `0.0005`.
For each update done, the derivatives are normalized like in ADAM's algorithm, just without the momentum part.
For each update to a step size weight is done by `x(n+1) = x(n) + x(n) * u * step-size`, where `x` is the step size itself, `u` is the update done to the step size, `step-size` is the step size's step size.



Backpropagation:
1. Update input offsets. (from `3.`)
2. Take the derivative for the weights after the inputs are offseted by input offsets (`input - input_offsets`). This reminds a lot of batch normalization, it improves the training performance because it mimics second-order optimizations.
3. The above is performed for each backpropagation, below is performed for each batch.
4. Update each weight with it's respective input step size.
5. Calculate the input step sizes' derivative by multiplying the running averages of weights update by the batch's derivative.
6. Update the running averages of weights update.
7. Update each input step size (with the derivative calculated at `5.`).
8. Optimize SSSS by taking the following as the gradient: the sum of the input step sizes' running average of updates multiplied by their current derivative. (Like the formula in the theory section)
9. Update the running averages of input step sizes update.

If you are having hard time understanding with the above words, then try reading [adasopt.py](adasopt.py).
