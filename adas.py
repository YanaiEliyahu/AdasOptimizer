#! /usr/bin/python3

from __future__ import absolute_import, division, print_function, unicode_literals

import tensorflow as tf
from adasopt import AdasOptimizer

mnist = tf.keras.datasets.mnist

(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28, 28)),
  tf.keras.layers.Dense(256, activation='relu'),
  tf.keras.layers.Dense(10)
])
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
model.compile(optimizer=AdasOptimizer(), loss=loss_fn, metrics=['accuracy'])

model.fit(x_train, y_train, epochs=50, validation_data=(x_test, y_test))

"""
Epoch 1/50
60000/60000 [==============================] - 6s 97us/sample - loss: 0.2181 - acc: 0.9349 - val_loss: 0.1243 - val_acc: 0.9627
Epoch 2/50
60000/60000 [==============================] - 5s 91us/sample - loss: 0.0885 - acc: 0.9726 - val_loss: 0.0813 - val_acc: 0.9737
Epoch 3/50
60000/60000 [==============================] - 5s 88us/sample - loss: 0.0558 - acc: 0.9826 - val_loss: 0.0753 - val_acc: 0.9792
Epoch 4/50
60000/60000 [==============================] - 6s 98us/sample - loss: 0.0374 - acc: 0.9880 - val_loss: 0.0823 - val_acc: 0.9779
Epoch 5/50
60000/60000 [==============================] - 6s 100us/sample - loss: 0.0207 - acc: 0.9934 - val_loss: 0.0969 - val_acc: 0.9720
Epoch 6/50
60000/60000 [==============================] - 6s 99us/sample - loss: 0.0120 - acc: 0.9966 - val_loss: 0.0767 - val_acc: 0.9794
Epoch 7/50
60000/60000 [==============================] - 6s 100us/sample - loss: 0.0071 - acc: 0.9979 - val_loss: 0.0726 - val_acc: 0.9826
Epoch 8/50
60000/60000 [==============================] - 6s 99us/sample - loss: 0.0035 - acc: 0.9993 - val_loss: 0.0850 - val_acc: 0.9816
Epoch 9/50
60000/60000 [==============================] - 6s 100us/sample - loss: 0.0022 - acc: 0.9995 - val_loss: 0.0860 - val_acc: 0.9823
Epoch 10/50
60000/60000 [==============================] - 6s 99us/sample - loss: 0.0013 - acc: 0.9998 - val_loss: 0.0932 - val_acc: 0.9828
Epoch 11/50
60000/60000 [==============================] - 6s 101us/sample - loss: 0.0011 - acc: 0.9998 - val_loss: 0.0946 - val_acc: 0.9816
Epoch 12/50
60000/60000 [==============================] - 6s 99us/sample - loss: 5.2550e-04 - acc: 1.0000 - val_loss: 0.0973 - val_acc: 0.9827
Epoch 13/50
60000/60000 [==============================] - 6s 100us/sample - loss: 4.4181e-04 - acc: 0.9999 - val_loss: 0.1057 - val_acc: 0.9817
Epoch 14/50
60000/60000 [==============================] - 6s 100us/sample - loss: 3.0610e-04 - acc: 1.0000 - val_loss: 0.1167 - val_acc: 0.9819
Epoch 15/50
60000/60000 [==============================] - 6s 101us/sample - loss: 1.8175e-04 - acc: 1.0000 - val_loss: 0.1211 - val_acc: 0.9813
Epoch 16/50
60000/60000 [==============================] - 6s 99us/sample - loss: 1.1657e-04 - acc: 1.0000 - val_loss: 0.1321 - val_acc: 0.9817
Epoch 17/50
60000/60000 [==============================] - 6s 100us/sample - loss: 2.0975e-04 - acc: 1.0000 - val_loss: 0.1366 - val_acc: 0.9808
Epoch 18/50
60000/60000 [==============================] - 6s 99us/sample - loss: 1.4154e-04 - acc: 1.0000 - val_loss: 0.1392 - val_acc: 0.9822
Epoch 19/50
60000/60000 [==============================] - 6s 100us/sample - loss: 3.7025e-05 - acc: 1.0000 - val_loss: 0.1439 - val_acc: 0.9820
Epoch 20/50
60000/60000 [==============================] - 6s 99us/sample - loss: 2.9200e-05 - acc: 1.0000 - val_loss: 0.1519 - val_acc: 0.9826
Epoch 21/50
60000/60000 [==============================] - 6s 100us/sample - loss: 1.9593e-05 - acc: 1.0000 - val_loss: 0.1542 - val_acc: 0.9825
Epoch 22/50
60000/60000 [==============================] - 6s 98us/sample - loss: 1.2755e-05 - acc: 1.0000 - val_loss: 0.1642 - val_acc: 0.9816
Epoch 23/50
60000/60000 [==============================] - 6s 100us/sample - loss: 1.4300e-05 - acc: 1.0000 - val_loss: 0.1711 - val_acc: 0.9815
Epoch 24/50
60000/60000 [==============================] - 6s 99us/sample - loss: 7.2018e-06 - acc: 1.0000 - val_loss: 0.1757 - val_acc: 0.9822
Epoch 25/50
60000/60000 [==============================] - 6s 99us/sample - loss: 4.3134e-06 - acc: 1.0000 - val_loss: 0.1843 - val_acc: 0.9821
Epoch 26/50
60000/60000 [==============================] - 6s 98us/sample - loss: 3.0774e-06 - acc: 1.0000 - val_loss: 0.1857 - val_acc: 0.9818
Epoch 27/50
60000/60000 [==============================] - 6s 101us/sample - loss: 2.4514e-06 - acc: 1.0000 - val_loss: 0.1979 - val_acc: 0.9810
Epoch 28/50
60000/60000 [==============================] - 6s 103us/sample - loss: 2.7028e-06 - acc: 1.0000 - val_loss: 0.1959 - val_acc: 0.9807
Epoch 29/50
60000/60000 [==============================] - 6s 99us/sample - loss: 1.3409e-06 - acc: 1.0000 - val_loss: 0.2022 - val_acc: 0.9810
Epoch 30/50
60000/60000 [==============================] - 6s 99us/sample - loss: 9.5007e-07 - acc: 1.0000 - val_loss: 0.2058 - val_acc: 0.9808
Epoch 31/50
60000/60000 [==============================] - 6s 100us/sample - loss: 3.6545e-07 - acc: 1.0000 - val_loss: 0.2063 - val_acc: 0.9810
Epoch 32/50
60000/60000 [==============================] - 6s 99us/sample - loss: 3.8336e-07 - acc: 1.0000 - val_loss: 0.2166 - val_acc: 0.9806
Epoch 33/50
60000/60000 [==============================] - 6s 100us/sample - loss: 2.3574e-07 - acc: 1.0000 - val_loss: 0.2183 - val_acc: 0.9809
Epoch 34/50
60000/60000 [==============================] - 6s 99us/sample - loss: 1.6763e-07 - acc: 1.0000 - val_loss: 0.2211 - val_acc: 0.9808
Epoch 35/50
60000/60000 [==============================] - 6s 100us/sample - loss: 3.7193e-07 - acc: 1.0000 - val_loss: 0.2254 - val_acc: 0.9809
Epoch 36/50
60000/60000 [==============================] - 6s 94us/sample - loss: 8.6790e-08 - acc: 1.0000 - val_loss: 0.2275 - val_acc: 0.9810
Epoch 37/50
60000/60000 [==============================] - 4s 70us/sample - loss: 5.9091e-08 - acc: 1.0000 - val_loss: 0.2299 - val_acc: 0.9807
Epoch 38/50
60000/60000 [==============================] - 4s 70us/sample - loss: 5.0967e-08 - acc: 1.0000 - val_loss: 0.2318 - val_acc: 0.9809
Epoch 39/50
60000/60000 [==============================] - 4s 70us/sample - loss: 3.8765e-08 - acc: 1.0000 - val_loss: 0.2352 - val_acc: 0.9805
Epoch 40/50
60000/60000 [==============================] - 4s 70us/sample - loss: 3.6680e-08 - acc: 1.0000 - val_loss: 0.2348 - val_acc: 0.9807
Epoch 41/50
60000/60000 [==============================] - 4s 70us/sample - loss: 2.9448e-08 - acc: 1.0000 - val_loss: 0.2358 - val_acc: 0.9808
Epoch 42/50
60000/60000 [==============================] - 4s 71us/sample - loss: 2.4072e-08 - acc: 1.0000 - val_loss: 0.2382 - val_acc: 0.9804
Epoch 43/50
60000/60000 [==============================] - 4s 71us/sample - loss: 2.1058e-08 - acc: 1.0000 - val_loss: 0.2396 - val_acc: 0.9808
Epoch 44/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.8646e-08 - acc: 1.0000 - val_loss: 0.2401 - val_acc: 0.9807
Epoch 45/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.6421e-08 - acc: 1.0000 - val_loss: 0.2408 - val_acc: 0.9807
Epoch 46/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.4706e-08 - acc: 1.0000 - val_loss: 0.2432 - val_acc: 0.9811
Epoch 47/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.3951e-08 - acc: 1.0000 - val_loss: 0.2433 - val_acc: 0.9806
Epoch 48/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.2700e-08 - acc: 1.0000 - val_loss: 0.2435 - val_acc: 0.9807
Epoch 49/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.1824e-08 - acc: 1.0000 - val_loss: 0.2448 - val_acc: 0.9807
Epoch 50/50
60000/60000 [==============================] - 4s 70us/sample - loss: 1.0904e-08 - acc: 1.0000 - val_loss: 0.2457 - val_acc: 0.9810
"""
